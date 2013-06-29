TARGET = qdds
TEMPLATE = lib
CONFIG += qt plugin
DESTDIR = ../../../../imageformats

win32:RC_FILE += dds.rc

HEADERS += dds.h \
    ddsheader.h \
    dxt.h
SOURCES += dds.cpp \
    ddsheader.cpp \
    dxt.cpp
