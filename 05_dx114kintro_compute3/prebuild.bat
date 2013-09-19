@echo off
set OLDDIR=%CD%
cd /d %~dp0
python minify.py
chdir %d %OLDDIR%
exit 0