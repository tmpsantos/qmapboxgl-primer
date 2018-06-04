TEMPLATE = app
TARGET = qmapboxgl-primer

QT += opengl

CONFIG += c++14

HEADERS += \
    src/mapview.hpp

SOURCES += \
    src/main.cpp \
    src/mapview.cpp

RESOURCES += \
    resources/resources.qrc

INCLUDEPATH += \
    deps/qmapboxgl/include

linux {
    LIBS += \
        -L$$PWD/deps/qmapboxgl/lib -lqmapboxgl \
        -Wl,-rpath,\'$$PWD/deps/qmapboxgl/lib\'
}

win32 {
    LIBS += \
        $$PWD/deps/qmapboxgl/lib/qmapboxgl.lib
}
