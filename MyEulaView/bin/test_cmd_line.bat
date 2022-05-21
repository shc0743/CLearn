@echo off
set p=%random%.tmp
start /wait rundll32 MyEulaViewDLL_x64.dll,EulaView_HTML_CommandLine %* > %p%
echo ERRORLEVEL: %ERRORLEVEL%
echo;
type %p%
del /f /s /q %p% >nul 2>&1
