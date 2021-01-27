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

        if (AV_CODEC_ID_H264 == avcp->codec_id)
        {
            LOGFN_INFO("AV_CODEC_ID_H264");
        }
        else
        {
            assert(0);
        }
    }

    static QString av_strerror (int errNum)
    {
        char errBuf[AV_ERROR_MAX_STRING_SIZE];
        int ret = ::av_strerror (errNum, errBuf, AV_ERROR_MAX_STRING_SIZE);

        assert(0 == ret);

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

