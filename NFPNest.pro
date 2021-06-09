QT += core gui widgets

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
    Strip.h \
    mainwindow.h \
    util/LB_BaseUtil.h \
    util/LB_Coord2D.h \
    util/LB_NFPHandle.h \
    util/LB_Polygon2D.h \
    util/LB_Rect2D.h \
    util/NestThread.h

SOURCES += \
    Strip.cpp \
    main.cpp \
    mainwindow.cpp \
    util/LB_NFPHandle.cpp \
    util/LB_Polygon2D.cpp \
    util/NestThread.cpp
	
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    mainwindow.ui
