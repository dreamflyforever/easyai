import cv2
import time
import subprocess
import os
import time
i = 0
t = 0
def main():
    while(1):
        global i
        global t
        img = str(i) + "_out.jpeg"
        i=i+1
        #subprocess.call("adb pull /oem/ws/" + img)
        os.system("adb pull /oem/ws/" + img)
        frame = cv2.imread(img)
        cv2.imshow('capture', frame)
        if cv2.waitKey(1)&0xFF==ord('q'):#按键盘q就停止拍照
            break
        t = t + 1
        os.system("mv " + img + " " + str(t) + ".jpeg")
    cv2.destroyAllWindows()


if __name__ == '__main__':
    while True:
        try:
            main()
        except Exception:
            if (i >= 5):
                i = -1
            print("exception..............................")
            pass
        except KeyboardInterrupt:
            print("key break ..............................")
            break
