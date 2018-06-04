#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QMapboxGL>

#include <QOpenGLWidget>
#include <QScopedPointer>
#include <QtGlobal>

class QKeyEvent;
class QMouseEvent;
class QWheelEvent;

class MapView : public QOpenGLWidget
{
    Q_OBJECT

public:
    MapView(const QMapboxGLSettings &);
    ~MapView();

private:
    void changeStyle();
    qreal pixelRatio();

    // QWidget implementation.
    void keyPressEvent(QKeyEvent *ev) final;
    void mousePressEvent(QMouseEvent *ev) final;
    void mouseMoveEvent(QMouseEvent *ev) final;
    void wheelEvent(QWheelEvent *ev) final;

    // QOpenGLWidget implementation.
    void initializeGL() final;
    void paintGL() final;

    QPointF m_lastPos;

    QMapboxGLSettings m_settings;
    QScopedPointer<QMapboxGL> m_map;

    QVariant m_symbolAnnotationId;
    QVariant m_lineAnnotationId;
    QVariant m_fillAnnotationId;

    bool m_sourceAdded = false;
};

#endif
