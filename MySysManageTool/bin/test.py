import ctypes
h = ctypes.WinDLL('./MySysManageTool64')
r = (h.InitKernelDriver())
print(r)
import os
#os.system("pause")
