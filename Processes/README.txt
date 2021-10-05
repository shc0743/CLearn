Processes

Processes_x64.exe
    Usage: 
        Processes_x64.exe \[((i|-i|--interface)| (start|kill|crash|close|suspend|resume) PrcoessFliter )\]
    Paramters:
        i -i --interface    Open a interface UI.
        start               Start a new process
        kill                kill a process (exit-code:0)
        crash               Crash a process as CreateRemoteThread(hProcess, 0, 0, 0, 0, 0, 0)
        close               Close all of process's window (if there is no visible window,the command will fail)
        suspend             Suspend process
        resume              Resume process
        ProcessFliter       ProcessName or PID

ProcessGUI.exe
    Create a GUI to show processes (useless)
    
ProcessSeparateConf.exe
    Select a process and you can kill,close,crash,suspend or resume it.
    (No command-line supported)

pscrash.exe
    Usage:
        pscrash.exe ProcessFliter
    Crash a prcoess (=Processes_x64.exe crash ProcessFliter)
