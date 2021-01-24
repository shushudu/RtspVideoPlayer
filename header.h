#pragma once

#include <QApplication>
#include <QCoreApplication>
#include <QHBoxLayout>
#include <QHash>
#include <QImage>
#include <QLabel>
#include <QMutex>
#include <QObject>
#include <QPixmap>
#include <QString>
#include <QThread>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

extern "C" {
#include "3rdparty/ffmpeg41x64/include/libavcodec/avcodec.h"
#include "3rdparty/ffmpeg41x64/include/libavdevice/avdevice.h"
#include "3rdparty/ffmpeg41x64/include/libavfilter/avfilter.h"
#include "3rdparty/ffmpeg41x64/include/libavformat/avformat.h"
#include "3rdparty/ffmpeg41x64/include/libavutil/avutil.h"
#include "3rdparty/ffmpeg41x64/include/libavutil/buffer.h"
#include "3rdparty/ffmpeg41x64/include/libavutil/error.h"
#include "3rdparty/ffmpeg41x64/include/libavutil/frame.h"
#include "3rdparty/ffmpeg41x64/include/libavutil/imgutils.h"
#include "3rdparty/ffmpeg41x64/include/libavutil/log.h"
#include "3rdparty/ffmpeg41x64/include/libavutil/opt.h"
#include "3rdparty/ffmpeg41x64/include/libavutil/pixfmt.h"
#include "3rdparty/ffmpeg41x64/include/libavutil/rational.h"
#include "3rdparty/ffmpeg41x64/include/libswscale/swscale.h"
}
