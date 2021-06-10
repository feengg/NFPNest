QT += core gui widgets

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

include($$PWD/nest/LB_Nest.pri)

HEADERS += \
    NestThread.h \
    Strip.h \
    mainwindow.h

SOURCES += \
    NestThread.cpp \
    Strip.cpp \
    main.cpp \
    mainwindow.cpp
	
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    mainwindow.ui
