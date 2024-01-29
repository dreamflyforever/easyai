import cv2
import time
def main():
    i=0
    while(1):
        frame = cv2.imread(str(i) + "_out.jpeg")
        cv2.imshow('capture', frame)
        i=i+1
        if (i == 50):
            i = 0
        if cv2.waitKey(100)&0xFF==ord('q'):#按键盘q就停止拍照
            break
    cv2.destroyAllWindows()


if __name__ == '__main__':
    while True:
        try:
            main()
        except Exception:
            pass
        except KeyboardInterrupt:
            break
