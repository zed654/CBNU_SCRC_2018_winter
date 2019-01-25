import cv2
import numpy as np

def find_yellow(path):
    img = cv2.imread(path, cv2.IMREAD_COLOR)
    hsv=cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    
    lower_yellow = np.array([20, 100, 100])
    higher_yellow = np.array([30, 255, 255])
    
    mask_yellow = cv2.inRange(hsv, lower_yellow, higher_yellow)
    
    res_yell = cv2.bitwise_and(img, img, mask =mask_yellow)
    
    cv2.imshow('original', img)
    cv2.imshow('Yellow', res_yell)
    
    cv2.waitKey(0)
    
    cv2.destroyAllWindows()

find_yellow('/Users/leeseungjoon/Desktop/GitHub_SJLEE/CBNU_SCRC_2018_winter/OpenCV_Practice/insane.jpg')
#find_yellow('/Users/leeseungjoon/Desktop/GitHub_SJLEE/CBNU_SCRC_2018_winter/OpenCV_Practice/bigyellow_02.jpg')
#find_yellow('/Users/leeseungjoon/Desktop/GitHub_SJLEE/CBNU_SCRC_2018_winter/OpenCV_Practice/center_line.jpg')
