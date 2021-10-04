@echo off
:a
processes_x64 kill sgtool.exe
processes_x64 kill sogoucloud.exe
ping 127.0.0.1 -n 5 >nul
goto a
