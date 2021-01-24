#pragma once

#include <QApplication>
#include <QHBoxLayout>
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
#include <libavcodec/avcodec.h>
//#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavutil/buffer.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

}
