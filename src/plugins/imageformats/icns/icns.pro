TARGET = qicns
TEMPLATE = lib
CONFIG += qt plugin
DESTDIR = ../../../../imageformats

win32:RC_FILE += icns.rc

HEADERS += \
    main.h \
    qicnshandler.h

SOURCES += \
    main.cpp \
    qicnshandler.cpp
