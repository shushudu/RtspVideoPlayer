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


    static void dumpAVCodecParameters (AVCodecParameters * avcp)
    {
        assert (avcp);

        if (AVMEDIA_TYPE_VIDEO == avcp->codec_type)
        {
            LOGFN_INFO("AVMEDIA_TYPE_VIDEO");
        }
        else
        {
            assert(0);
        }


        LOGFN_INFO(ffmpeg::avcodec_get_name(avcp->codec_id));
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
            assert(0);
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

