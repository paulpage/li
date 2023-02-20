SET SDL_PATH=C:\dev\lib\sdl2\bin\x64
SET COMPILE_FLAGS= /I lib /I \dev\lib\sdl2\SDL-release-2.26.1\include\
SET LINK_FLAGS=/link /SUBSYSTEM:WINDOWS kernel32.lib user32.lib winmm.lib gdi32.lib opengl32.lib shell32.lib
cl src\main.c src\platform.c %SDL_PATH%\SDL2.lib %SDL_PATH%\SDL2main.lib %COMPILE_FLAGS% %LINK_FLAGS%
