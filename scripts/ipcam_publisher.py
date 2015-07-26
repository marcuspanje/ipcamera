import roslib
import rospy
import sys
import numpy as np
import cv2
import urllib


def ipcam_pub(url):
   response = urllib.urlopen(url)
   print response.read()

   img.


if __name__ == '__main__':
   ipcam_pub(sys.argv[1])
   





