@echo off

@REM if NOT "%1" neq "" (
@REM >"%temp%\e20v4oI6fmsbPX5W.vbs" echo set WshShell = WScript.CreateObject^(^"WScript.Shell^"^)
@REM >>"%temp%\e20v4oI6fmsbPX5W.vbs" echo WshShell.Run chr^(34^) ^& %0 ^& chr^(34^) ^& ^" --window=hide^",0
@REM start /d "%temp%" e20v4oI6fmsbPX5W.vbs
@REM exit /b 0
@REM )

MyConfirmBox 妈妈肥不肥? --count=10 --btn2=是的(^&Y) --btn1=她不肥(^&N) --title=确认 --topalert=请确认以下内容:
if %ERRORLEVEL%==3 (
echo msgbox "回答正确,恭喜!!!!" > %tmp%\3b6d1bc1b12432cb.vbs
start /wait wscript %tmp%\3b6d1bc1b12432cb.vbs
exit /b 0
)
if %ERRORLEVEL%==2 (
echo msgbox "回答错误!!" > %tmp%\19879564d7a02fec.vbs
start /wait wscript %tmp%\19879564d7a02fec.vbs
exit /b 1
)