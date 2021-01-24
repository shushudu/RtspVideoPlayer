#pragma once

#include "rtspvideostreamdecoder.h"
#include "ui_mainwindow.h"

#include <QLabel>
#include <QMutex>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QLabel * lbl = nullptr;

    void createCamera(const QString & rtsp);

    QTimer * checkNewFrameTimer = nullptr;

    QMutex mtx;
    AVFrame * frameForShow = nullptr;
    QString infoText;

private slots:
    void onThreadStarted();

    void onRtspVideoStreamDecoderFinished()
    {
        //checkNewFrameTimer->stop();
    }

    void onRtspInfoChanged(QString info)
    {
        infoText = info;
    }

    void onRtspNewFrame(AVFrame * frame)
    {
        AVFrame * toFree = nullptr;

        mtx.lock();
        toFree = frameForShow;
        frameForShow = frame;
        mtx.unlock();

        if (toFree)
        {
            av_frame_unref (toFree);
            av_frame_free (&toFree);
        }
    }


    void displayFrame (const AVFrame * fr)
    {
        assert (fr);

        const int frameFormat = fr->format;

        QImage::Format qif;

        switch (frameFormat)
        {
            case AV_PIX_FMT_RGB24:
                qif = QImage::Format_RGB888;
                break;

            case AV_PIX_FMT_GRAY8:
                qif = QImage::Format_Grayscale8;
                break;

            default:
            {
                lbl->setText("frame format not supported");
                return;
            }
        }

        QPixmap pixmap;

        bool ret = pixmap.convertFromImage (QImage (fr->data[0], fr->width, fr->height,
                                 fr->linesize[0], qif));

        assert(ret);

        lbl->setPixmap (pixmap.scaled (lbl->size (), Qt::KeepAspectRatio));
    }

    void onCheckNewFrameTimerTimeout()
    {
        AVFrame * toShow = nullptr;

        mtx.lock();
        toShow = frameForShow;
        frameForShow = nullptr;
        mtx.unlock();

        if (toShow)
        {
            QString err;
            AVFrame * convertedFrame = RtspVideoStreamDecoder::convertFrame (toShow, err);

            if (!convertedFrame)
            {
                assert(0);
            }

            if (convertedFrame != toShow)
            {
                av_frame_unref (toShow);
                av_frame_free (&toShow);
            }

            displayFrame(convertedFrame);

            av_frame_unref (convertedFrame);
            av_frame_free (&convertedFrame);
        }
        else
        {
            if (!infoText.isEmpty())
            {
                lbl->setText(infoText);
            }
            else
            {
                lbl->setText("no video");
            }
        }
    }
};
