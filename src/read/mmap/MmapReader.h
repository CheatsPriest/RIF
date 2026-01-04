#pragma once
#include <read/BaseReader.hpp>

#ifdef _WIN32
#include <read/mmap/MemoryMapWindows.h>
#elif __linux__
#include <read/mmap/MemoryMapLinux.h>
#endif


class MmapReader : public BaseReader<MmapReader> {
private:

	size_t curLen;
	size_t maxLen;
#ifdef _WIN32
	MemoryMapWindows reader;
#elif __linux__
	MemoryMapLinux reader;
#endif

public:
	MmapReader(const std::string& filename) : reader(filename), maxLen(0), curLen(0){
		maxLen = reader.size();
	}
	const char_t readSymbolImpl() inline const  {
		return reader.readCharAt(curLen);
	}
	bool moveToSymbolImpl(long long dif) {
		curLen += dif;
		if (curLen >= maxLen)return false;
		return true;
	}
	size_t getLenImpl() const {
		return curLen;
	}
	bool emptyImpl() const {
		return curLen >= maxLen;
	}
	const char_t* getDataImpl() const {
		return reader.dataAt(curLen);
	}
	size_t sizeImpl() const {
		return maxLen;
	}
	size_t getPosImpl() const {
		return curLen;
	}
};