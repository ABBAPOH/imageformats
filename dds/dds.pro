TARGET = qdds
TEMPLATE = lib
CONFIG += qt plugin

win32:RC_FILE += dds.rc

HEADERS += dds.h \
    ddsheader.h \
    qdxt.h
SOURCES += dds.cpp \
    qdxt.cpp
