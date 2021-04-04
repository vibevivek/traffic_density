all:
	g++ main.cpp -o code.out -pthread -std=c++11 `pkg-config --cflags --libs opencv`
allMac:
	g++ main.cpp -o code.out -pthread -std=c++11 `pkg-config --cflags --libs opencv4`