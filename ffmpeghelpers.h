#pragma once

#include <assert.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include "logger.h"

class ffmpeg
{
public:
    static void dump (AVFormatContext * fc, const QString & tabs = "")
    {
        if (fc)
        {
            LOGFN_INFO(QString("%1open demuxer flags: 0x%2").arg(tabs).arg(QString::number(fc->flags, 16)));
            LOGFN_INFO(QString("%1number of streams: %2").arg(tabs).arg(fc->nb_streams));

            for (size_t i=0; i<fc->nb_streams; ++i)
            {
                LOGFN_INFO(QString("%1stream %2:").arg(tabs).arg(i));
                dump(fc->streams[i], tabs + "  ");
            }

            AVInputFormat * iformat = fc->iformat;
            if (iformat)
            {
                LOGFN_INFO(QString("%1input format:").arg(tabs));
                ffmpeg::dump(iformat, tabs + "  ");
            }
        }
    }


    static void dump (AVCodecParameters * codecpar, const QString & tabs = "")
    {
        if (codecpar)
        {
            LOGFN_INFO(QString("%1stream codec type: %2").arg(tabs).arg(av_get_media_type_string(codecpar->codec_type)));
            LOGFN_INFO(QString("%1codec name: %2").arg(tabs).arg(avcodec_get_name(codecpar->codec_id)));
        }
    }


    static void dump (AVInputFormat * iformat, const QString & tabs = "")
    {
        if (iformat)
        {
            LOGFN_INFO(QString("%1input format name: %2").arg(tabs).arg(iformat->name));
            LOGFN_INFO(QString("%1input format flags: %2").arg(tabs).arg(iformat->flags));
        }
    }

    static void dump (AVStream * stream, const QString & tabs = "")
    {
        if (stream)
        {
            AVCodecParameters *codecpar = stream->codecpar;
            assert(codecpar);
            ffmpeg::dump(codecpar, tabs + "  ");
        }
    }

    static QString avcodec_get_name (enum AVCodecID id)
    {
        const char * name = ::avcodec_get_name(id);
        assert(name);
        return QString(name);
    }

    static QString av_strerror (int errNum)
    {
        char errBuf[AV_ERROR_MAX_STRING_SIZE] = {0};
        int ret = ::av_strerror (errNum, errBuf, AV_ERROR_MAX_STRING_SIZE);

        if( 0 != ret)
        {
            return QString("av_strerror failed for %1").arg(errNum);
        }

        return QString(errBuf);
    }

    static void av_init_packet(AVPacket * pkt)
    {
        assert(pkt);

        ::av_init_packet (pkt);

        assert (pkt->data == nullptr);
        assert (pkt->size == 0);

        pkt->data = nullptr;
        pkt->size = 0;
    }

    static int av_read_frame(AVFormatContext *ctx, AVPacket * pkt)
    {
        assert(ctx);
        assert(pkt != nullptr);
        assert(pkt->buf == nullptr);
        int ret = ::av_read_frame(ctx, pkt);

        return ret;
    }

    static AVFormatContext * avformat_alloc_context(void)
    {
        AVFormatContext * ret = ::avformat_alloc_context ();
        assert (ret);
        return ret;
    }

};

