#include "mapview.hpp"

#include <QApplication>
#include <QColor>
#include <QDebug>
#include <QFile>
#include <QIcon>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QString>
#include <QWindow>


MapView::MapView(const QMapboxGLSettings &settings)
    : m_settings(settings)
{
}

MapView::~MapView()
{
    // Make sure we have a valid context so we
    // can delete the QMapboxGL.
    makeCurrent();
}

qreal MapView::pixelRatio() {
#if QT_VERSION >= 0x050600
    return devicePixelRatioF();
#elif QT_VERSION >= 0x050000
    return devicePixelRatio();
#else
    return 1;
#endif
}


void MapView::changeStyle()
{
    static uint8_t currentStyleIndex;

    auto& styles = QMapbox::defaultStyles();

    m_map->setStyleUrl(styles[currentStyleIndex].first);
    setWindowTitle(QString("Mapbox GL | Current style: ") + styles[currentStyleIndex].second
        + " | Press 's' to change style, 'l' to add GeoJSON");

    if (++currentStyleIndex == styles.size()) {
        currentStyleIndex = 0;
    }

    m_sourceAdded = false;
}

void MapView::keyPressEvent(QKeyEvent *ev)
{
    switch (ev->key()) {
    case Qt::Key_S:
        changeStyle();
        break;
    case Qt::Key_L: {
            if (m_sourceAdded) {
                return;
            }

            m_sourceAdded = true;

            QFile geojson(":source.geojson");
            geojson.open(QIODevice::ReadOnly);

            // The data source for the route line
            QVariantMap routeSource;
            routeSource["type"] = "geojson";
            routeSource["data"] = geojson.readAll();
            m_map->addSource("routeSource", routeSource);

            // The route layer
            QVariantMap routeCase;
            routeCase["id"] = "route";
            routeCase["type"] = "line";
            routeCase["source"] = "routeSource";

            // Not in all styles, but will work on streets
            QString before = "waterway-label";

            m_map->addLayer(routeCase, before);

            // Style the route
            m_map->setPaintProperty("route", "line-color", QColor("blue"));
            m_map->setPaintProperty("route", "line-width", 20.0);
            m_map->setLayoutProperty("route", "line-join", "round");
            m_map->setLayoutProperty("route", "line-cap", "round");
        }
        break;
    case Qt::Key_Tab:
        m_map->cycleDebugOptions();
        break;
    default:
        break;
    }

    ev->accept();
}

void MapView::mousePressEvent(QMouseEvent *ev)
{
    m_lastPos = ev->localPos();

    if (ev->type() == QEvent::MouseButtonPress) {
        if (ev->buttons() == (Qt::LeftButton | Qt::RightButton)) {
            changeStyle();
        }
    }

    if (ev->type() == QEvent::MouseButtonDblClick) {
        if (ev->buttons() == Qt::LeftButton) {
            m_map->scaleBy(2.0, m_lastPos);
        } else if (ev->buttons() == Qt::RightButton) {
            m_map->scaleBy(0.5, m_lastPos);
        }
    }

    ev->accept();
}

void MapView::mouseMoveEvent(QMouseEvent *ev)
{
    QPointF delta = ev->localPos() - m_lastPos;

    if (!delta.isNull()) {
        if (ev->buttons() == Qt::LeftButton && ev->modifiers() & Qt::ShiftModifier) {
            m_map->setPitch(m_map->pitch() - delta.y());
        } else if (ev->buttons() == Qt::LeftButton) {
            m_map->moveBy(delta);
        } else if (ev->buttons() == Qt::RightButton) {
            m_map->rotateBy(m_lastPos, ev->localPos());
        }
    }

    m_lastPos = ev->localPos();

    ev->accept();
}

void MapView::wheelEvent(QWheelEvent *ev)
{
    if (ev->orientation() == Qt::Horizontal) {
        return;
    }

    float factor = ev->delta() / 1200.;
    if (ev->delta() < 0) {
        factor = factor > -1 ? factor : 1 / factor;
    }

    m_map->scaleBy(1 + factor, ev->pos());
    ev->accept();
}

void MapView::initializeGL()
{
    m_map.reset(new QMapboxGL(nullptr, m_settings, size(), pixelRatio()));
    connect(m_map.data(), SIGNAL(needsRendering()), this, SLOT(update()));

    // Set default location to Helsinki.
    m_map->setCoordinateZoom(QMapbox::Coordinate(60.170448, 24.942046), 13);

    changeStyle();
}

void MapView::paintGL()
{
    m_map->resize(size());
    m_map->setFramebufferObject(defaultFramebufferObject(), size() * pixelRatio());
    m_map->render();
}
