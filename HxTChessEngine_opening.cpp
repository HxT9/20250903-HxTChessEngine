#include "HxTChessEngine.h"
#include <fstream>
#include <sstream>
#include <regex>

OpeningTree::OpeningTree() {
	parent = nullptr;
	move = "";
}

OpeningTree::OpeningTree(OpeningTree* _parent, std::string _move) : OpeningTree() {
	parent = _parent;
	move = _move;
}

inline bool OpeningTree::operator==(const OpeningTree& other) {
	return move == other.move;
}

void OpeningTree::addNextMove(std::vector<std::string> _move) {
	if (_move.empty())
		return;

	auto it = std::find_if(nextMoves.begin(), nextMoves.end(),
		[&](const OpeningTree* child) {
			return child->move == _move[0];
		});

	if (it == nextMoves.end())
		nextMoves.emplace_back(new OpeningTree(this, _move[0]));

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

void OpeningTree::readFromFile(const char* file) {
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

OpeningTree* OpeningTree::getNext(std::string move) {
	if (move == "" && nextMoves.size() > 0)
		return nextMoves.at(0);

	for (int i = 0; i < nextMoves.size(); i++)
		if (nextMoves.at(i)->move == move)
			return nextMoves.at(i);

	return nullptr;
}