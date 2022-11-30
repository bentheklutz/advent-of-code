@echo off
mkdir ..\out
pushd ..\out
cl /Feday02_simple.exe /Zi ..\source\navigation.c /link user32.lib
cl /Feday02_amended.exe /Zi ..\source\navigation-amended.c /link user32.lib
popd
