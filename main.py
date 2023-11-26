import ctypes
import os
import cv2
import numpy as np

os.add_dll_directory(r"C:\Users\ilyak\source\repos\kurs-fall-2023\opencv\build\x64\vc16\bin")
os.add_dll_directory(r"C:\Users\ilyak\source\repos\kurs-fall-2023\opencv\build\x64\vc16\bin")
hllDll = ctypes.CDLL(r"C:\Users\ilyak\source\repos\kurs-fall-2023\x64\Release\ocl-dll.dll")

picPath = r"C:\Users\ilyak\source\repos\kurs-fall-2023\pic.PNG".encode('utf-8')

hllDll.Cols.restype = ctypes.c_int
hllDll.Cols.argtype = ctypes.POINTER(ctypes.c_char)
cols = hllDll.Cols(picPath)
print(cols)

hllDll.Cols.restype = ctypes.c_int
hllDll.Cols.argtype = ctypes.POINTER(ctypes.c_char)
rows = hllDll.Cols(picPath)
print(rows)


array = (ctypes.c_int * cols * rows)()
hllDll.Dilatation.restype = ctypes.POINTER(ctypes.c_int)
hllDll.Dilatation.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_char)]

ret = hllDll.Dilatation(20, picPath)

shape = (rows, cols, 4)
x = np.zeros(shape)
for i in range(0, rows):
    #y = np.array([], dtype=np.uint32)
    for j in range(0, cols):
        if(ret[i*cols + j] == 1):
            x[i][j] = [255, 255, 255, 255]
        else:
            x[i][j] = [0, 0, 0, 255]

a = cv2.imread(r"C:\Users\ilyak\source\repos\kurs-fall-2023\pic.PNG")


vis2 = cv2.cvtColor(x.astype((np.uint8)), cv2.COLOR_RGB2BGR)
cv2.imshow("pic", vis2)
cv2.waitKey(0)

# closing all open windows
cv2.destroyAllWindows()
