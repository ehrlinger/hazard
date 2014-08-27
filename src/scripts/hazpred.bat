@ECHO OFF
echo %1
%HAZAPPS%\hazpred.exe < %1.sas > %1.lst
