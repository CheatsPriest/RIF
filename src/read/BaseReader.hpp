#pragma once

#include "char.hpp"

// 64КБ для оптимального кэша
static constexpr size_t CHUNK_SIZE = 524288;

template<typename Derived>
class BaseReader {
public:

	bool readNextChunk(string& chunk) {
		Derived& self = static_cast<Derived&>(*this);
		return self.readNextChunkImpl(chunk);
	} 

};