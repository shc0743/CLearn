@cd/d "%~dp0"
@if /i %PROCESSOR_IDENTIFIER:~0,3%==x86 (int3_x86 %*) else (int3_x64 %*)