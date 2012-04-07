
#include <cstdlib>
#include <cstdio>

extern "C" {
#include <librtmp/log.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

void die(char *msg)
{
    printf("ERROR: %s.\n", msg);
    exit(1);
}

int main(int argc, char **argv)
{
    RTMP_LogSetLevel(RTMP_LOGALL);
    av_log_set_level(AV_LOG_DEBUG);

    char *jtv = "rtmp://199.9.252.15/app playpath=jtv_cJBKovkzQJ2PXJfr live=1 swfUrl=http://www-cdn.jtvnw.net/widgets/live_site_player.r444c3068c252158ce9cc5110fd2f32cad5fea60d.swf";
    av_register_all();
    avformat_network_init();

    AVFormatContext *fmtContext;

    if (av_open_input_file(&fmtContext, jtv, NULL, 0, NULL) != 0) {
        die("Could not open video stream");
    }

    if (av_find_stream_info(fmtContext) < 0) {
        die("Couldn't find stream info");
    }

    printf("Success!\n");

    return 0;
}
