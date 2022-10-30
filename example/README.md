# Example

A simple demonstration showing how to control an OLED display (SSD1306) via I2C.

## Contents

* 400khz.sh - Run as root, this script switches I2C speed from 100kHz to 400kHz
* disable_kde_compositor.sh - Run as your user, this script disables the compositor function of the KDE desktop, which causes problems when capturing the screen with FFMPEG
* run.sh - Run as root, this script starts FFMPEG to record the screen, piping the raw video data to the Python script, run this script to start the demonstration
* ssd1306.py - A very simple and ugly SSD1306 OLED driver in Python using the smbus python library


![photo](photo.jpg)

[![Demonstration video](http://img.youtube.com/vi/PMtY5OU9V3Q/0.jpg)](http://www.youtube.com/watch?v=PMtY5OU9V3Q "Demonstration video")
