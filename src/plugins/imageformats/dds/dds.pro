TARGET = qdds
TEMPLATE = lib
CONFIG += qt plugin
DESTDIR = ../../../../imageformats

win32:RC_FILE += dds.rc

HEADERS += dds.h \
    ddsheader.h \
    qdxt.h
SOURCES += dds.cpp \
    ddsheader.cpp \
    qdxt.cpp
