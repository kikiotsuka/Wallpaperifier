wallpaperify: *.cpp
	g++ *.cpp -lsfml-graphics -lsfml-window -lsfml-system -o wallpaperify $(python-config --libs --cflags)
