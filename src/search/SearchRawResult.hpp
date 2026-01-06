#pragma once

#include <char.hpp>


struct RawResult {
	size_t start;
	size_t end;
	bool operator<(const RawResult& rhs) const {
		return start < rhs.start;
	}
};