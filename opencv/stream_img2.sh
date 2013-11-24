#!/bin/bash
mjpg_streamer -i "input_file.so -f /mnt/ramdisk/img2" -o "output_http.so -p 8081 -w /mnt/ramdisk/web2"
