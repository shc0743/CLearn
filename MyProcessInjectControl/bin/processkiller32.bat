@echo off
set "pid=%1"
if not "%pid%"=="" goto run
set /p pid=Process ID: 
if "%pid%"=="" exit /b

:run
"%~dp0\InjectDll32" "%~dp0\ExitSelf32.dll" %pid%
pause
