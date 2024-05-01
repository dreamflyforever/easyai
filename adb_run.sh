adb shell mkdir -p /oem/ws
adb push install/rv1106_linux_armhf/rknn__demo/easyai  /oem/ws
adb shell mkdir -p /oem/ws/model
adb push install/rv1106_linux_armhf/rknn__demo/model/check_people.wav /oem/ws/model
adb push install/rv1106_linux_armhf/rknn__demo/model/check_sleep.wav /oem/ws/model
adb push install/rv1106_linux_armhf/rknn__demo/model/normal1.wav /oem/ws/model
adb push install/rv1106_linux_armhf/rknn__demo/model/normal0.wav /oem/ws/model
adb push install/rv1106_linux_armhf/rknn__demo/model/normal2.wav /oem/ws/model
adb push install/rv1106_linux_armhf/rknn__demo/model/bb.wav /oem/ws/model
adb push install/rv1106_linux_armhf/rknn__demo/model/welcome.wav /oem/ws/model
adb push install/rv1106_linux_armhf/rknn__demo/model/labels_list.txt /oem/ws/model

adb push kernel/model/leftright.wav /oem/ws/model
adb push kernel/model/child.wav /oem/ws/model
adb push kernel/model/mobile.wav /oem/ws/model
adb push kernel/model/talking.wav /oem/ws/model
adb push kernel/model/verify.wav /oem/ws/model

adb push 3rdparty/alsa/share/alsa/alsa.conf /oem/ws/share/alsa/alsa.conf
adb push install/rv1106_linux_armhf/rknn__demo/model/sleep_v51.rknn /userdata/
adb push RkLunch.sh /oem/usr/bin/RkLunch.sh
adb push lib/librknnmrt.* /oem/usr/lib/
adb shell reboot
sleep 10
cd display; python show.py
