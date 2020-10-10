all:
	g++ -std=c++17 -fsanitize=address,undefined -c main.cpp
	g++ main.o -o a.out -lsfml-graphics -lsfml-system -lsfml-window -lsfml-audio -fsanitize=address,undefined
	rm main.o