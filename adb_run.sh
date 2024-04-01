adb shell mkdir -p /oem/ws
adb push install/rv1106_linux_armhf/rknn__demo/rknn_yolov5_demo  /oem/ws
adb shell mkdir -p /oem/ws/model
adb push install/rv1106_linux_armhf/rknn__demo/model/check_people.wav /oem/ws/model
adb push install/rv1106_linux_armhf/rknn__demo/model/welcome.wav /oem/ws/model
adb push install/rv1106_linux_armhf/rknn__demo/model/coco_80_labels_list.txt /oem/ws/model
adb push 3rdparty/alsa/share/alsa/alsa.conf /oem/ws/share/alsa/alsa.conf
adb push install/rv1106_linux_armhf/rknn__demo/model/yolov5s-640-640.rknn /userdata/
adb shell RkLunch-stop.sh
adb shell /oem/ws/rknn_yolov5_demo /userdata/yolov5s-640-640.rknn
