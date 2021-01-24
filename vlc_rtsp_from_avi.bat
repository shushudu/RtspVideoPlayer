taskkill /F /IM vlc.exe

start /D "C:\Program Files\VideoLAN\VLC\" vlc "D:\1.avi" --repeat --qt-start-minimized --sout "#transcode{venc=x264{keyint=1,profile=baseline,level=3.0,nocabac},width=1280,vcodec=x264,vb100,fps=1000}:rtp{sdp=rtsp://:8554/test}"