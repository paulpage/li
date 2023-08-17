@echo off

REM 4101 and 4189 are for unused variables
SET CommonCompilerFlags=/nologo /MTd /GR- /EHa- /Od /Oi /WX /W4 /wd4100 /wd4101 /wd4189 /FC /Zi
REM /wd4201 /wd4189 /wd4505

SET SDL_PATH=C:\dev\lib\sdl2\bin\x64
SET COMPILE_FLAGS=/I lib /I \dev\lib\sdl2\SDL-release-2.26.1\include\
SET LINK_FLAGS=/link /DEBUG:FULL /SUBSYSTEM:CONSOLE kernel32.lib user32.lib winmm.lib gdi32.lib opengl32.lib shell32.lib
REM SET LINK_FLAGS=/link /DEBUG:FULL /SUBSYSTEM:WINDOWS kernel32.lib user32.lib winmm.lib gdi32.lib opengl32.lib shell32.lib
REM cl src\main.c src\platform.c %SDL_PATH%\SDL2.lib %SDL_PATH%\SDL2main.lib %COMPILE_FLAGS% %CommonCompilerFlags% %LINK_FLAGS%
cl src\tetris.c src\platform.c %SDL_PATH%\SDL2.lib %SDL_PATH%\SDL2main.lib %COMPILE_FLAGS% %CommonCompilerFlags% %LINK_FLAGS%
