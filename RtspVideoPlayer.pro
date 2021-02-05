QT       += core gui widgets

TARGET = RtspVideoPlayer

TEMPLATE = app

PRECOMPILED_HEADER = $$PWD/header.h

HEADERS  += mainwindow.h \
        ffmpeghelpers.h \
        header.h \
        logger.h \
        rtspvideostreamdecoder.h \
        rtspwidget.h

SOURCES += main.cpp\
        mainwindow.cpp \
        rtspvideostreamdecoder.cpp \
        rtspwidget.cpp

INCLUDEPATH += $$PWD/3rdparty/ffmpeg/4.3/include
LIBS += -L$$PWD/3rdparty/ffmpeg/4.3/bin -lavcodec -lswscale -lavformat -lavutil

DESTDIR = $$PWD/bin

RC_ICONS = icon.ico

VERSION = 1.21.02.05
