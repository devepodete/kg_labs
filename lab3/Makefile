all:
	g++ -std=c++17 -fsanitize=address,undefined -c main.cpp
	g++ -fsanitize=address,undefined main.o -o a.out -lsfml-graphics -lsfml-system -lsfml-window -lsfml-audio
	rm main.o
