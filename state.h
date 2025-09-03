#pragma once
#include <string>

/*
8
7
6
5
4
3
2
1
 A B C D E F G H

A1 -> 0 0
A2 -> 0 1
A7 -> 0 6
A8 -> 0 8
G1 -> 6 0
H1 -> 7 0
H8 -> 7 7
*/

class state {
public:
	int  width = 8, height = 8;
	unsigned char** board;

	state();
	state(int w, int h);
	~state();

	bool init();
	bool initStdBoard();

	std::string toString();
};