QT += core gui widgets

TARGET = RtspVideoPlayer

TEMPLATE = app

PRECOMPILED_HEADER = $$PWD/header.h

HEADERS  += mainwindow.h \
        ffmpeghelpers.h \
        header.h \
        jsonappconf.h \
        logger.h \
        rtspvideostreamdecoder.h \
        rtspwidget.h

SOURCES += main.cpp\
        jsonappconf.cpp \
        mainwindow.cpp \
        rtspvideostreamdecoder.cpp \
        rtspwidget.cpp

INCLUDEPATH += $$PWD/3rdparty/ffmpeg/4.3/include
LIBS += -L$$PWD/3rdparty/ffmpeg/4.3/bin -lavcodec -lswscale -lavformat -lavutil

DESTDIR = $$PWD/bin

RC_ICONS = icon.ico

VERSION = 1.22.10.19

DISTFILES += \
    conf.json \
    icon.ico


copy_to_build.path = $$DESTDIR
copy_to_build.files = conf.json

INSTALLS += \
    copy_to_build

