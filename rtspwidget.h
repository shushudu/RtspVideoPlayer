#pragma once

#include <QLabel>
#include <QMutex>

#include "rtspvideostreamdecoder.h"

class RtspWidget : public QLabel
{
    Q_OBJECT

    QTimer * checkNewFrameTimer = nullptr;
    QMutex mtx;
    AVFrame * frameForShow = nullptr;
    QString infoText;

    RtspVideoStreamDecoder * rtsp = nullptr;


public:
    explicit RtspWidget(QWidget *parent = nullptr);

signals:
    void started();
    void stopped();

public slots:
    void start(const QString & rtsp_url)
    {
        rtsp = new RtspVideoStreamDecoder(rtsp_url);

        connect(rtsp, &RtspVideoStreamDecoder::started, this, &RtspWidget::onRtspVideoStreamDecoderStarted, Qt::UniqueConnection);
        connect(rtsp, &RtspVideoStreamDecoder::finished, this, &RtspWidget::onRtspVideoStreamDecoderFinished, Qt::UniqueConnection);
        connect(rtsp, &RtspVideoStreamDecoder::newFrame, this, &RtspWidget::onRtspNewFrame, Qt::UniqueConnection);
        connect(rtsp, &RtspVideoStreamDecoder::infoChanged, this, &RtspWidget::onRtspInfoChanged, Qt::UniqueConnection);

        rtsp->start();
        checkNewFrameTimer->start();
    }

    void stop()
    {
//        checkNewFrameTimer->stop();
        rtsp->stop();
    }

private slots:

    void onRtspVideoStreamDecoderStarted()
    {
        emit started();
    }

    void onRtspVideoStreamDecoderFinished()
    {
        emit stopped();
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
                QLabel::setText("frame format not supported");
                return;
            }
        }

        QPixmap pixmap;

        bool ret = pixmap.convertFromImage (QImage (fr->data[0], fr->width, fr->height,
                                 fr->linesize[0], qif));

        assert(ret);

        QLabel::setPixmap (pixmap.scaled (QLabel::size (), Qt::KeepAspectRatio));
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
                QLabel::setText(infoText);
            }
            else
            {
                QLabel::setText("no video");
            }
        }
    }
};
