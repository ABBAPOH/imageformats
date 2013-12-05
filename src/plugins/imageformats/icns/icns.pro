TARGET = qicns
TEMPLATE = lib
CONFIG += qt plugin
DESTDIR = ../../../../imageformats

win32:RC_FILE += icns.rc

HEADERS += \
    qicnshandler_p.h

SOURCES += \
    main.cpp \
    qicnshandler.cpp
