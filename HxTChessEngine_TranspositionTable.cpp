#include "HxTChessEngine.h"

TranspositionTable::TranspositionTable()
{
    currentAge = 0;
	size = 64 * 1024 * 1024;
}

TranspositionTable::TranspositionTable(int sizeMB) : TranspositionTable()
{
	size = sizeMB * 1024 * 1024;
}

void TranspositionTable::init() {
    table.resize(size);
}

void TranspositionTable::clear() {
	std::fill(table.begin(), table.end(), TTEntry{});
}

void TranspositionTable::incrementAge()
{
	currentAge++;
}

size_t TranspositionTable::getIndex(uint64_t zobristKey) const {
	return zobristKey % size;
}

void TranspositionTable::store(uint64_t zobristKey, int16_t score, int8_t depth, TTFlag flag, int8_t from, int8_t to)
{
    size_t index = getIndex(zobristKey);
    TTEntry& entry = table[index];

    bool shouldReplace = entry.zobristKey == 0 ||
        entry.zobristKey == zobristKey ||
        depth >= entry.depth ||
        (currentAge - entry.age) > 2;

    if (shouldReplace) {
        entry.zobristKey = zobristKey;
        entry.score = score;
        entry.depth = depth;
        entry.flag = flag;
        entry.from = from;
        entry.to = to;
        entry.age = currentAge;
    }
}

bool TranspositionTable::probe(uint64_t zobristKey, TTEntry& result) const
{
    size_t index = getIndex(zobristKey);
    const TTEntry& entry = table[index];

    if (entry.zobristKey == zobristKey) {
        result = entry;
        return true;
    }
    return false;
}
