@echo off

@REM if NOT "%1" neq "" (
@REM >"%temp%\e20v4oI6fmsbPX5W.vbs" echo set WshShell = WScript.CreateObject^(^"WScript.Shell^"^)
@REM >>"%temp%\e20v4oI6fmsbPX5W.vbs" echo WshShell.Run chr^(34^) ^& %0 ^& chr^(34^) ^& ^" --window=hide^",0
@REM start /d "%temp%" e20v4oI6fmsbPX5W.vbs
@REM exit /b 0
@REM )

MyConfirmBox ����ʲ���? --count=10 --btn2=�ǵ�(^&Y) --btn1=������(^&N) --title=ȷ�� --topalert=��ȷ����������:
if %ERRORLEVEL%==3 (
echo msgbox "�ش���ȷ,��ϲ!!!!" > %tmp%\3b6d1bc1b12432cb.vbs
start /wait wscript %tmp%\3b6d1bc1b12432cb.vbs
exit /b 0
)
if %ERRORLEVEL%==2 (
echo msgbox "�ش����!!" > %tmp%\19879564d7a02fec.vbs
start /wait wscript %tmp%\19879564d7a02fec.vbs
exit /b 1
)