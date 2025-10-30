#include "utils.h"
#include <random>
#include <chrono>

uint64_t random64() {
	static std::mt19937_64 rng((uint64_t)std::chrono::high_resolution_clock::now().time_since_epoch().count());
	return rng();
}