@echo off
mkdir ..\out
pushd ..\out
cl /Feday01_single.exe /Zi ..\source\single_measurement_compare.c /link user32.lib
cl /Feday01_window.exe /Zi ..\source\three_measurement_window.c /link user32.lib
popd
