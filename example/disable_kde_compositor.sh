#!/usr/bin/env bash

# This script temporarily disables the compositor function of the KDE plasma desktop as it causes glitches when recording the screen with ffmpeg

qdbus org.kde.KWin /Compositor suspend
