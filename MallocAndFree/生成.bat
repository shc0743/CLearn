@echo off
if "%1"=="x86" (
g++ -c MallocAndFree.cpp
g++ -c fn.cpp
g++ -o MallocAndFree fn.o MallocAndFree.o
) else (
g++ -m64 -c MallocAndFree.cpp
g++ -m64 -c fn.cpp
g++ -m64 -o MallocAndFree fn.o MallocAndFree.o
)
del /f /s /q fn.o 
del /f /s /q MallocAndFree.o
if not exist MallocAndFree.exe (
echo Éú³ÉÊ§°Ü
pause
)