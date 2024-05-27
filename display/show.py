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
        orig_img = str(i) + "_orig.jpeg"
        i=i+1
        #subprocess.call("adb pull /oem/ws/" + img)
        os.system("adb pull /" + img)
        os.system("adb pull /" + orig_img)
        frame = cv2.imread(img)
        rgb_frame = cv2.imread(orig_img)
        cv2.imshow('capture', frame)
        cv2.imshow('rgb', rgb_frame)
        if cv2.waitKey(1)&0xFF==ord('q'):#按键盘q就停止拍照
            break
        t = t + 1
        if (i >= 6):
            i = 0
        #os.system("mv " + img + " " + str(t) + ".jpg")
        #os.system("mv " + orig_img + " " + str(t) + "_orig.jpg")
    cv2.destroyAllWindows()


if __name__ == '__main__':
    while True:
        try:
            main()
        except Exception:
            if (i >= 6):
                i = 0
            print("exception..............................")
            pass
        except KeyboardInterrupt:
            print("key break ..............................")
            break
