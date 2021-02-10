#ifndef RTSPVIDEOSTREAMDECODER_H
#define RTSPVIDEOSTREAMDECODER_H

#include <QString>
#include <QThread>

struct AVFormatContext;
struct AVCodecContext;
struct AVStream;
struct AVFrame;
struct AVPacket;

class RtspVideoStreamDecoder: public QThread
{
    Q_OBJECT

    void run() override;

    const QString rtspUrl;

public:
    RtspVideoStreamDecoder(const QString & rtspUrl);

    static AVFrame * convertFrame (AVFrame * fromFrame, AVPixelFormat toFormat, int linesizeAlignment, QString & err);
    static AVFrame * convertFrame (AVFrame * frame, QString & err);

    static void freeFrame(AVFrame ** frame);

    static QImage toQImage(const AVFrame * const fr, QString & err);

public slots:
    void start();

    void stop();

signals:
    void newFrame(AVFrame *);

    void infoChanged(QString info);

private:
    volatile bool interrupt = false;

    static int interruptCallback (void * p);

    AVCodecContext * openStream (AVStream * stream, QString & err);

    AVStream * getVideoStream(AVFormatContext * fc, QString & err);

    AVFormatContext * openRtsp (const QString & rtsp_url, QString & err);

    bool startReceiveFrames (AVFormatContext * fc, AVStream * videoStream, AVCodecContext * avCtx, QString & err);

    bool frameFromDecoder(AVCodecContext * avCtx, AVFrame * frame);

    bool sendPacketToDecoder(AVCodecContext * avCtx, AVPacket * pck, QString & err);

    bool openAndStartRtsp(QString & err);
};

#endif // RTSPVIDEOSTREAMDECODER_H
