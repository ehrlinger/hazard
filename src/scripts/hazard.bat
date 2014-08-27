@ECHO OFF
echo %1
%HAZAPPS%\hazard.exe < %1.sas > %1.lst
