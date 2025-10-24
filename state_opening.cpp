#include "state.h"
#include <fstream>
#include <sstream>
#include <regex>

openingTree* openingBook;

openingTree::openingTree() {
	parent = nullptr;
	move = "";
}

openingTree::openingTree(openingTree* _parent, std::string _move) : openingTree() {
	parent = _parent;
	move = _move;
}

inline bool openingTree::operator==(const openingTree& other) {
	return move == other.move;
}

void openingTree::addNextMove(std::vector<std::string> _move) {
	if (_move.empty())
		return;

	auto it = std::find_if(nextMoves.begin(), nextMoves.end(),
		[&](const openingTree* child) {
			return child->move == _move[0];
		});

	if (it == nextMoves.end())
		nextMoves.emplace_back(new openingTree(this, _move[0]));

	//Remove first element and recurse
	if (_move.size() > 0) {
		for (int i = 0; i < nextMoves.size(); i++) {
			if (nextMoves.at(i)->move == _move[0]) {
				_move.erase(_move.begin());
				nextMoves.at(i)->addNextMove(_move);
				break;
			}
		}
	}
}

void openingTree::readFromFile(const char* file) {
	std::string line;
	std::vector<std::string> moves;

	std::ifstream i(file, std::ios::in);
	if (i.fail()) {
		return;
	}

	while (getline(i, line)) {
		std::stringstream ss(line);
		std::string token;
		moves.clear();
		while (getline(ss, token, ' '))
			if (!std::regex_match(token, std::regex(R"(\d+\.)")))
				moves.push_back(token);

		addNextMove(moves);
	}

	i.close();
}

openingTree* openingTree::getNext(std::string move) {
	if (move == "" && nextMoves.size() > 0)
		return nextMoves.at(0);

	for (int i = 0; i < nextMoves.size(); i++)
		if (nextMoves.at(i)->move == move)
			return nextMoves.at(i);

	return nullptr;
}