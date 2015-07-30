wallpaperify: *.cpp
	g++ *.cpp -lsfml-graphics -lsfml-window -lsfml-system -o wallpaperify $(python-config --libs --cflags)
windows: *.cpp
	i686-w64-mingw32-g++ *.cpp -lsfml-graphics -lsfml-window -lsfml-system -o windows/wallpaperify.exe -D SYS_WINDOWS
clean:
	rm *.o wallpaperify
