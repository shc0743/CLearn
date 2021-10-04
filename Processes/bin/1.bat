@echo off
:a
processes_x64 suspend sogoucloud.exe
ping 127.0.0.1 -n 2 >nul
goto a
