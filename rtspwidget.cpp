#include "rtspwidget.h"

#include <QLabel>
#include <QTimer>

RtspWidget::RtspWidget(QWidget *parent) : QLabel(parent)
{
    QLabel::setAlignment (Qt::AlignHCenter | Qt::AlignVCenter);
    QLabel::setWordWrap (true);
    QLabel::setStyleSheet("QLabel { background-color : black; color : white; }");
    QLabel::setText ("ready to start");
    QLabel::setMinimumSize(QSize(32,32));

    checkNewFrameTimer = new QTimer(this);
    connect(checkNewFrameTimer, &QTimer::timeout, this, &RtspWidget::onCheckNewFrameTimerTimeout, Qt::UniqueConnection);
    checkNewFrameTimer->setInterval(500);

}


void RtspWidget::setUpdateInterval(int msec)
{
    checkNewFrameTimer->setInterval(msec);
}

void RtspWidget::setMaxFrameDelay(int msec)
{
    maxFrameDelay = msec;
}

void RtspWidget::start(const QString & rtsp_url)
{
    rtsp = new RtspVideoStreamDecoder(rtsp_url);

    connect(rtsp, &RtspVideoStreamDecoder::started, this, &RtspWidget::onRtspVideoStreamDecoderStarted, Qt::UniqueConnection);
    connect(rtsp, &RtspVideoStreamDecoder::finished, this, &RtspWidget::onRtspVideoStreamDecoderFinished, Qt::UniqueConnection);
    connect(rtsp, &RtspVideoStreamDecoder::newFrame, this, &RtspWidget::onRtspNewFrame, Qt::UniqueConnection);
    connect(rtsp, &RtspVideoStreamDecoder::infoChanged, this, &RtspWidget::onRtspInfoChanged, Qt::UniqueConnection);

    rtsp->start();
    checkNewFrameTimer->start();
}

void RtspWidget::stop()
{
    rtsp->stop();
}

void RtspWidget::onRtspVideoStreamDecoderStarted()
{
    emit started();
}

void RtspWidget::onRtspVideoStreamDecoderFinished()
{
    emit stopped();
}

void RtspWidget::onRtspInfoChanged(QString info)
{
    infoText = info;
}

void RtspWidget::onRtspNewFrame(AVFrame * frame)
{
    AVFrame * toFree = nullptr;

    mtx.lock();
    toFree = frameForShow;
    frameForShow = frame;
    mtx.unlock();

    if (frameForShow)
    {
        lastFrameDt = QDateTime::currentDateTime();
    }

    if (toFree)
    {
        av_frame_unref (toFree);
        av_frame_free (&toFree);
    }
}

void RtspWidget::displayFrame (const AVFrame * fr)
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

void RtspWidget::onCheckNewFrameTimerTimeout()
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
            QDateTime now = QDateTime::currentDateTime();
            if ((now.toMSecsSinceEpoch() - lastFrameDt.toMSecsSinceEpoch()) > maxFrameDelay)
            {
                QLabel::setText("no video");
            }
        }
    }
}
