@echo off
rc ZWM.rc
cl /nologo /MD /W3 /GX /O2 /FeZWM.exe ZWM.cpp SearchEngine.cpp ZWM.res
del ZWM.res
del *.obj
pause