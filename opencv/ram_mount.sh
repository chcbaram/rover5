#!/bin/bash
sudo mkdir -p /mnt/ramdisk
sudo mount -t tmpfs -o size=10M tmpfs /mnt/ramdisk/
mkdir /mnt/ramdisk/img1
mkdir /mnt/ramdisk/img2
mkdir /mnt/ramdisk/web1
mkdir /mnt/ramdisk/web2

