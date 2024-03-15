## easyai
detect object in rv1103 platform

## compile
./build-linux.sh

## usage
host : adb push install/rv1106_linux_armhf/rknn__demo/*   /oem/ws/  
 adb push 3rdparty/alsa/share/alsa/alsa.conf /oem/ws/share/alsa
board: ./rknn_yolov5_demo ./model/yolov5s-640-640.rknn

## license
MIT
