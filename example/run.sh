#!/usr/bin/env bash
ffmpeg -framerate 16 -f x11grab -video_size 1920x1080 -i :0+0,0 -f image2pipe -vf scale=128x64 -pix_fmt monob -vcodec rawvideo - | python -u ssd1306.py
