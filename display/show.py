import cv2
import time
import subprocess
import os
i = 0
def main():
    while(1):
        global i
        img = str(i) + "_out.jpeg"
        #subprocess.call("adb pull /oem/ws/" + img)
        os.system("adb pull /oem/ws/" + img)
        frame = cv2.imread(img)
        cv2.imshow('capture', frame)
        i=i+1
        if (i == 50):
            i = 0
        if cv2.waitKey(1)&0xFF==ord('q'):#按键盘q就停止拍照
            break
        os.system("rm " + img)
    cv2.destroyAllWindows()


if __name__ == '__main__':
    while True:
        try:
            main()
        except Exception:
            pass
        except KeyboardInterrupt:
            break
