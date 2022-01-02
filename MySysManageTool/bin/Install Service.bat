@echo off
cd /d "%~dp0"
SysManageTool_UsermodeHost_x64 --service-options --uninstall-service --name=MySysManageToolSvc
SysManageTool_UsermodeHost_x64 --service-options --install-service --svc-name=MySysManageToolSvc --display-name="My System Manage Tool Service" >stdout.tmp 2>stderr.tmp
type std*.tmp
pause
del /f /s /q std*.tmp