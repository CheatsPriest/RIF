#pragma once

#include "char.hpp"

// 64КБ для оптимального кэша
static constexpr size_t CHUNK_SIZE = 524288;
static constexpr size_t DEDUCT_CHUNK_SIZE = 16384;

template<typename Derived>
class BaseReader {
public:

	bool readNextChunk(string& chunk) {
		Derived& self = static_cast<Derived&>(*this);
		return self.readNextChunkImpl(chunk);
	} 
	void rewind() {
		Derived& self = static_cast<Derived&>(*this);
		return self.rewindImpl();
	}

};