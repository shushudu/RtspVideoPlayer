#include "rtspvideostreamdecoder.h"

#include <QString>
#include <QThread>
#include <QImage>
#include <assert.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/buffer.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#include "ffmpeghelpers.h"
#include "logger.h"


void RtspVideoStreamDecoder::run()
{
    QString finishInfoText = "unknown error";

    if (!openAndStartRtsp(finishInfoText))
    {
        LOGFN_ERROR(finishInfoText);
        emit infoChanged(finishInfoText);
    }
    else
    {
        emit infoChanged("RTSP stream gracefully closed");
    }
}


RtspVideoStreamDecoder::RtspVideoStreamDecoder(const QString & rtspUrl):
    rtspUrl(rtspUrl)
{

}

AVFrame * RtspVideoStreamDecoder::convertFrame (AVFrame * fromFrame, AVPixelFormat toFormat, int linesizeAlignment, QString & err)
{
    AVFrame * newFrame = av_frame_alloc ();
    assert(newFrame);

    int ret;

    if ((ret = av_frame_ref (newFrame, fromFrame)) < 0)
    {
        assert(0);
        av_frame_free (&newFrame);
        return nullptr;
    }

    for (size_t i = 0; i < FF_ARRAY_ELEMS (newFrame->buf); ++i)
    {
        if (newFrame->buf[i])
        {
            av_buffer_unref (&newFrame->buf[i]);
            assert (!rgbFrame->buf[i]);
        }
    }

    const int bufSize = av_image_get_buffer_size (toFormat, fromFrame->width, fromFrame->height, linesizeAlignment);
    if (bufSize < 0)
    {
        err = "av_image_get_buffer_size failed";
        assert(0);
        av_frame_unref (newFrame);
        av_frame_free (&newFrame);
        return nullptr;
    }


    AVBufferPool * bufferPool = av_buffer_pool_init (bufSize, nullptr);
    if (!bufferPool)
    {
        assert(0);
        av_frame_unref (newFrame);
        av_frame_free (&newFrame);
        return nullptr;
    }

    if (!(newFrame->buf[0] = av_buffer_pool_get (bufferPool)))
    {
        av_buffer_pool_uninit (&bufferPool);
        assert(0);
        av_frame_unref (newFrame);
        av_frame_free (&newFrame);
        return nullptr;
    }

    newFrame->format = toFormat;

    ret = av_image_fill_arrays (newFrame->data
            , newFrame->linesize
            , newFrame->buf[0]->data
            , AVPixelFormat (newFrame->format)
            , fromFrame->width
            , fromFrame->height
            , linesizeAlignment);

    if (ret < 0)
    {
        assert(0);
        av_buffer_pool_uninit (&bufferPool);
        av_frame_unref (newFrame);
        av_frame_free (&newFrame);
        return nullptr;
    }

    struct SwsContext * rgbSwsCtx = sws_getContext (fromFrame->width, fromFrame->height,
                                     AVPixelFormat (fromFrame->format),
                                     fromFrame->width, fromFrame->height, toFormat,
                                     SWS_FAST_BILINEAR /*| SWS_AREA*/,
                                     nullptr, nullptr, nullptr);
    if  (!rgbSwsCtx)
    {
        assert(0);
        av_buffer_pool_uninit (&bufferPool);
        av_frame_unref (newFrame);
        sws_freeContext (rgbSwsCtx);
        av_frame_free (&newFrame);
        return nullptr;
    }

    ret = sws_scale (rgbSwsCtx, fromFrame->data, fromFrame->linesize, 0, fromFrame->height,
                                  newFrame->data, newFrame->linesize);
    if (ret <= 0)
    {
        assert(0);
        av_buffer_pool_uninit (&bufferPool);
        av_frame_unref (newFrame);
        sws_freeContext (rgbSwsCtx);
        av_frame_free (&newFrame);
        return nullptr;
    }

    av_buffer_pool_uninit (&bufferPool);
    sws_freeContext (rgbSwsCtx);
    return newFrame;
}

AVFrame * RtspVideoStreamDecoder::convertFrame (AVFrame * frame, QString & err)
{
    err = "";

    if (!frame)
    {
        err = "frame is nullptr";
        return nullptr;
    }

    // shavlov // TEST return
    // return frame;

    switch (frame->format)
    {
        case AV_PIX_FMT_RGB24:
        case AV_PIX_FMT_GRAY8:
        //case AV_PIX_FMT_YUVJ420P: // цвет будет потерян
            return frame;
    }

    return convertFrame (frame, AVPixelFormat::AV_PIX_FMT_RGB24, 32, err);
}


void RtspVideoStreamDecoder::freeFrame(AVFrame ** frame)
{
    if (frame)
    {
        av_frame_unref (*frame);
        av_frame_free (frame);
    }
}


QImage RtspVideoStreamDecoder::toQImage(const AVFrame * const fr, QString & err)
{
    if(!fr)
    {
        err = QString("no frame");
        return QImage();
    }

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

        case AV_PIX_FMT_YUVJ420P:
            qif = QImage::Format_Grayscale8;
            break;

        default:
        {
            err = QString("frame format %1 not supported").arg(frameFormat);
            return QImage();
        }
    }

    QImage ret (fr->data[0], fr->width, fr->height, fr->linesize[0], qif);


    if(ret.isNull())
    {
        err = QString("frame convert from format %1 as %2 failed").arg(frameFormat).arg(qif);
        return ret;
    }

    return ret;
}


void RtspVideoStreamDecoder::start()
{
    QThread::start(QThread::NormalPriority);
}


void RtspVideoStreamDecoder::stop()
{
    interrupt = true;
}


int RtspVideoStreamDecoder::interruptCallback (void * p)
{
    assert(p);

    RtspVideoStreamDecoder * r = reinterpret_cast <RtspVideoStreamDecoder *> (p);
    if (r->interrupt)
    {
        LOGFN_ERROR ("interrupt request sended");
        return 1;
    }

    return 0;
}


AVCodecContext * RtspVideoStreamDecoder::openStream (AVStream * stream, QString & err)
{
    err = "";
    assert(stream);

    AVCodecParameters * codecpar = stream->codecpar;
    assert(codecpar);
    assert(codecpar->codec_type == AVMEDIA_TYPE_VIDEO);

    AVCodec * codec = avcodec_find_decoder (codecpar->codec_id);
    if (!codec)
    {
        err = QString ("no decoder for codec %1").arg (avcodec_get_name (codecpar->codec_id));
        return nullptr;
    }

    AVCodecContext * avCtx = avcodec_alloc_context3 (codec);
    if (!avCtx)
    {
        err = QString("avcodec_alloc_context3 failed:");
        return nullptr;
    }

    int ret = avcodec_parameters_to_context (avCtx, stream->codecpar);
    if (ret < 0)
    {
        err = QString("avcodec_parameters_to_context failed: %1").arg(ffmpeg::av_strerror(ret));
        avcodec_free_context(&avCtx);
        return nullptr;
    }

    AVDictionary * opts = nullptr;

    ret = av_dict_set (&opts, "threads", "auto", 0);
    assert(ret>=0);

    ret = av_dict_set (&opts, "refcounted_frames", "1", 0);
    assert(ret>=0);

    ret = avcodec_open2 (avCtx, codec, &opts);
    if (ret < 0)
    {
        err = QString ("avcodec_open2 failed %1").arg (ffmpeg::av_strerror(ret));
        av_dict_free (&opts);
        avcodec_free_context(&avCtx);
        return nullptr;
    }

    AVDictionaryEntry * t = av_dict_get (opts, "", NULL, AV_DICT_IGNORE_SUFFIX);
    if (t)
    {
        err = QString ("codec-private option '%1' not found").arg (t->key);
        av_dict_free (&opts);
        avcodec_free_context(&avCtx);
        return nullptr;
    }
    av_dict_free (&opts);

    stream->discard = AVDISCARD_DEFAULT; //AVDISCARD_NONKEY

    return avCtx;
}


AVStream * RtspVideoStreamDecoder::getVideoStream(AVFormatContext * fc, QString & err)
{
    assert(fc);
//    assert(!fc->pb);

    int ret = av_find_best_stream (fc, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (AVERROR_STREAM_NOT_FOUND == ret)
    {
        err = "video stream not found";
        return nullptr;
    }
    else if (AVERROR_DECODER_NOT_FOUND == ret)
    {
        err = "decoder for video stream not found";
        return nullptr;
    }
    else if (ret < 0)
    {
        err = QString ("av_find_best_stream failed: %1").arg (ffmpeg::av_strerror(ret));
        return nullptr;
    }
    unsigned int usedStreamIndex = (unsigned int)ret;


    LOGFN_INFO(QString("stream index: %1").arg(usedStreamIndex));
    LOGFN_INFO(QString("number of streams: %1").arg(fc->nb_streams));

    assert(fc->nb_streams > usedStreamIndex);

    AVStream * videoStream = fc->streams[usedStreamIndex];

    ffmpeg::dumpAVCodecParameters (videoStream->codecpar);

    return videoStream;
}


AVFormatContext * RtspVideoStreamDecoder::openRtsp (const QString & rtsp_url, QString & err)
{
    interrupt = false;

    AVFormatContext * fc = avformat_alloc_context();
    if (!fc)
    {
        err = "avformat_alloc_context failed";
        return nullptr;
    }

    fc->interrupt_callback.callback = interruptCallback;
    fc->interrupt_callback.opaque = this;

    AVDictionary * format_opts = nullptr;
//        int ret = av_dict_set (&format_opts, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
//        assert(ret>=0);

    int ret = avformat_open_input (&fc, rtsp_url.toUtf8 ().constData (), nullptr, &format_opts);

    av_dict_free (&format_opts);

    if (ret < 0)
    {
        assert(!fc);
        err = QString ("avformat_open_input failed '%1': %2").arg (rtsp_url).arg (ffmpeg::av_strerror(ret));
        return nullptr;
    }

    assert (fc);

//        av_format_inject_global_side_data (fc);

    return fc;
}


bool RtspVideoStreamDecoder::startReceiveFrames (AVFormatContext * fc, AVStream * videoStream, AVCodecContext * avCtx, QString & err)
{
    assert(fc);
    assert(videoStream);
    assert(avCtx);


    AVPacket pkt;
    pkt.data = nullptr;
    pkt.size = 0;

    ffmpeg::av_init_packet (&pkt);

    bool finishing = false;
    while (!finishing)
    {
        int ret = ffmpeg::av_read_frame (fc, &pkt);
        if (ret < 0)
        {
            if (AVERROR_EXIT == ret)
            {
                err = "interrupted";
                return false;
            }
            else if (ret == AVERROR_EOF || avio_feof (fc->pb))
            {
                LOGFN_INFO("AVERROR_EOF");

                av_packet_unref (&pkt);

                ffmpeg::av_init_packet (&pkt);

                pkt.data = nullptr;
                pkt.size = 0;

                LOGFN_WARN("last packet received");

                finishing = true;
            }
            else
            {
                if (fc->pb && fc->pb->error)
                {
                    assert(0);
                    LOGFN_ERROR(QString ("%1 (IO error %2)").arg (ffmpeg::av_strerror(ret), QString ().setNum (fc->pb->error)));
                }

                LOGFN_ERROR(ffmpeg::av_strerror(ret));
                assert(0);
            }


        }
        else  if (ret != AVERROR_EXIT
                  && (pkt.stream_index) == videoStream->index
                  && !(videoStream->disposition & AV_DISPOSITION_ATTACHED_PIC)
                  )
        {
            sendPacketToDecoder(avCtx, &pkt);
            av_packet_unref (&pkt);

            AVFrame * frame = av_frame_alloc ();

            if(!frameFromDecoder(avCtx, frame))
            {
                LOGFN_INFO("no ready frames from decoder yet");
            }
            else
            {
                emit newFrame(frame);
            }
        }
        else
        {
            LOGFN_WARN("av_packet_unref");
            av_packet_unref (&pkt);
        }
    }


    LOGFN_INFO ("closing...");
    //close ();

    return true;
}


bool RtspVideoStreamDecoder::frameFromDecoder(AVCodecContext * avCtx, AVFrame * frame)
{
    assert (avCtx);
    assert (frame);

    int res = avcodec_receive_frame (avCtx, frame);
    switch (res)
    {
    case 0:
        {
            assert(frame);
            return true;
        }
        break;

    case AVERROR (EAGAIN):
        LOGFN_INFO("no frames, try again");
        break;

    case AVERROR_EOF:
        LOGFN_INFO("avcodec_flush_buffers...");
        avcodec_flush_buffers (avCtx);
        break;

    default:
        LOGFN_ERROR("receive_frame failed");
    }

    return false;
}


bool RtspVideoStreamDecoder::sendPacketToDecoder(AVCodecContext * avCtx, AVPacket * pck)
{
    assert(pck);
    assert(avCtx);


    int ret = avcodec_send_packet (avCtx, pck);

    if (0 == ret)
    {
        //av_packet_unref (pck);
        return true;
    }


    if(AVERROR (EAGAIN) == ret)
    {
        assert(0);
    }
    else if(AVERROR_EOF == ret)
    {
        assert(0);
    }
    else
    {
        assert(0);
    }

    return false;
}


bool RtspVideoStreamDecoder::openAndStartRtsp(QString & err)
{
    err = "";

    emit infoChanged("open rtsp...");

    AVFormatContext * fc = openRtsp (rtspUrl, err);
    if (!fc)
    {
        return false;
    }

    emit infoChanged("get video stream...");

    AVStream * videoStream = getVideoStream(fc, err);
    if (!videoStream)
    {
        avformat_close_input(&fc);
        return false;
    }

    emit infoChanged("open video stream...");

    AVCodecContext * avCtx = openStream (videoStream, err);
    if(!avCtx)
    {
        avformat_close_input(&fc);
        return false;
    }

    emit infoChanged("");

    if(!startReceiveFrames (fc, videoStream, avCtx, err))
    {
        avcodec_free_context(&avCtx);
        avformat_close_input(&fc);
        return false;
    }

    avcodec_free_context(&avCtx);
    avformat_close_input(&fc);
    return true;
}
