@echo off
title Pipe Debugger
echo =================
echo   Pipe Debugger
echo =================
echo;
set/p pipe=Pipe name: 
echo ----data----
:rd
set/p data=
echo %data% > %pipe%
sleep 200
set/p data= < %pipe%
echo %data%
goto rd
