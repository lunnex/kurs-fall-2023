import ctypes
import os
import cv2
import numpy as np
import sys
from PyQt6.QtWidgets import QApplication, QMainWindow
from gui import Ui_MainWindow


class Login(QMainWindow):
    def __init__(self):
        super().__init__()

        # use the Ui_login_form
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.ui.pushButton.clicked.connect(self.execute)
        # show the login window
        self.show()

    def execute(self):
        os.add_dll_directory(r"E:\kurs-fall-2023\opencv\build\x64\vc16\bin")
        os.add_dll_directory(r"E:\kurs-fall-2023\opencv\build\x64\vc16\bin")

        hllDll = ctypes.CDLL(r"E:\kurs-fall-2023\x64\Release\sequence-dll.dll")

        if(self.ui.rb_sequence.isChecked()):
            hllDll = ctypes.CDLL(r"E:\kurs-fall-2023\x64\Release\sequence-dll.dll")
        elif(self.ui.rb_omp.isChecked()):
            hllDll = ctypes.CDLL(r"E:\kurs-fall-2023\x64\Release\omp-dll.dll")
        elif (self.ui.rb_ocl.isChecked()):
            hllDll = ctypes.CDLL(r"E:\kurs-fall-2023\x64\Release\ocl-dll.dll")

        print("\n")
#hllDll = ctypes.CDLL(r"C:\Users\ilyak\source\repos\kurs-fall-2023\x64\Release\ocl-dll.dll")

        picPath = r"E:\kurs-fall-2023\pic3.PNG".encode('utf-8')

        hllDll.Cols.restype = ctypes.c_int
        hllDll.Cols.argtype = ctypes.POINTER(ctypes.c_char)
        cols = hllDll.Cols(picPath)
        #print(cols)

        hllDll.Cols.restype = ctypes.c_int
        hllDll.Cols.argtype = ctypes.POINTER(ctypes.c_char)
        rows = hllDll.Cols(picPath)
        #print(rows)


        array = (ctypes.c_int * cols * rows)()

        if (self.ui.rb_dilatation.isChecked()):
            hllDll.Dilatation.restype = ctypes.POINTER(ctypes.c_int)
            hllDll.Dilatation.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_char)]
            ret = hllDll.Dilatation(self.ui.horizontalSlider.value(), picPath)
        else:
            hllDll.Erosion.restype = ctypes.POINTER(ctypes.c_int)
            hllDll.Erosion.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_char)]
            ret = hllDll.Erosion(self.ui.horizontalSlider.value(), picPath)

        shape = (rows, cols, 4)
        x = np.zeros(shape)
        for i in range(0, rows):
            #y = np.array([], dtype=np.uint32)
            for j in range(0, cols):
                if(ret[i*cols + j] == 1):
                    x[i][j] = [255, 255, 255, 255]
                else:
                    x[i][j] = [0, 0, 0, 255]


        vis2 = cv2.cvtColor(x.astype((np.uint8)), cv2.COLOR_RGB2BGR)
        cv2.imshow("pic", vis2)
        cv2.waitKey(0)

        # closing all open windows
        cv2.destroyAllWindows()

if __name__ == '__main__':
    app = QApplication(sys.argv)
    login_window = Login()
    sys.exit(app.exec())







