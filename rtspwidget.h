#pragma once

#include <QLabel>
#include <QMutex>
#include <QDateTime>

#include "rtspvideostreamdecoder.h"

class RtspWidget : public QLabel
{
    Q_OBJECT

    QTimer * checkNewFrameTimer = nullptr;
    QMutex mtx;
    AVFrame * frameForShow = nullptr;
    QString infoText;
    RtspVideoStreamDecoder * rtsp = nullptr;

    QDateTime lastFrameDt;

    int maxFrameDelay = 3000;


    void mousePressEvent(QMouseEvent* event) override;

public:
    explicit RtspWidget(QWidget *parent = nullptr);
    void setUpdateInterval(int msec);
    void setMaxFrameDelay(int msec);

signals:
    void started();
    void stopped();
    void clicked();

public slots:
    void start(const QString & rtsp_url);
    void stop();

private slots:

    void onRtspVideoStreamDecoderStarted();
    void onRtspVideoStreamDecoderFinished();
    void onRtspInfoChanged(QString info);
    void onRtspNewFrame(AVFrame * frame);
    void displayFrame (const AVFrame * fr);
    void onCheckNewFrameTimerTimeout();
};
