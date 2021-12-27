# test script for library 

import ctypes, time

if __name__=='__main__':
    
    l = ctypes.WinDLL("./ModalDialogBackgroundLib64.dll")
    time.sleep(3)

    obj = l.CreateBackgroundWindowEx(
        'Dialog Window',
        ctypes.WinDLL('user32').GetForegroundWindow(),
        False,
        127,
        1,
        1
    )
    #l.WaitForWindow(obj)
    l.MessageLoop(obj)

    pass
