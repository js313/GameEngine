build:
	g++ -Wall -std=c++17 -I"./libs/" src/*.cpp -lSDL2 -lSDL2_image -llua5.4 -o gameengine;

run:
	./gameengine

clean:
	rm gameengine