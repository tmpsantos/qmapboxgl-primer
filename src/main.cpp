#include "mapview.hpp"

#include <QApplication>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QMapboxGLSettings settings;

    // Use your own. This one will get removed eventually.
    settings.setAccessToken("pk.eyJ1IjoidG1wc2FudG9zIiwiYSI6IkpRS0p1VHcifQ.y5bSLhPlxM21hyiDBizcMg");

    MapView view(settings);

    view.resize(800, 600);
    view.show();

    return app.exec();
}
