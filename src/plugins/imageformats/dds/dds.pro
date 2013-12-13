TARGET = qdds
TEMPLATE = lib
CONFIG += qt plugin
DESTDIR = ../../../../imageformats

win32:RC_FILE += dds.rc

HEADERS += \
    ddsheader.h \
    qddshandler.h

SOURCES += \
    ddsheader.cpp \
    main.cpp \
    qddshandler.cpp
