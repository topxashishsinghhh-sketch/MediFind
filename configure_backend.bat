@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
cmake -S "C:\Users\AV\AppData\Roaming\Kilo Desktop\chats\session-TaZFiK\backend" -B "C:\Users\AV\AppData\Roaming\Kilo Desktop\chats\session-TaZFiK\backend\build" -G "Visual Studio 17 2022" -A x64
