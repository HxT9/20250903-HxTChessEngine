#pragma once

class bitBoard {
public:
	__int64 data = 0;
	inline bool get(int i) { return (data >> i) & 1; }
	inline void set(int i) { data |= (1ULL << i); }
	inline void reset(int i) { data &= ~(1ULL << i); }
};