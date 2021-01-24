QT       += core gui widgets

TARGET = RTSPVideoPlayer

TEMPLATE = app

PRECOMPILED_HEADER = $$PWD/header.h


HEADERS  += mainwindow.h \
        ffmpeghelpers.h \
        header.h \
        logger.h \
        rtspvideostreamdecoder.h

SOURCES += main.cpp\
        mainwindow.cpp \
        rtspvideostreamdecoder.cpp

FORMS += mainwindow.ui

INCLUDEPATH += $$PWD/3rdparty/ffmpeg41x64/include
LIBS += -L$$PWD/3rdparty/ffmpeg41x64/lib -lavcodec -lswscale -lavformat -lavutil -lavdevice -lavfilter
LIBS += -L$$PWD/3rdparty/ffmpeg41x64/bin
