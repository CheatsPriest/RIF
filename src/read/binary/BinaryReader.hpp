#pragma once
#include <read/binary/FileStreamReader.hpp>
#include <read/BaseReader.hpp>
class BinaryReader : public BaseReader<BinaryReader> {
private:
	std::unique_ptr<FileStreamReader> reader_ptr;
	bool isEmpty;
	string chunk;

	long long curLen;
	long long curMaxLen;
	size_t position;
	void loadNextChunk() {
        curLen -= curMaxLen;
		isEmpty = !reader_ptr->readNextChunk(chunk);
		curMaxLen = chunk.size();
	}
    std::string path;
public:
	BinaryReader(const std::string& path_) : isEmpty(true), curLen(0), curMaxLen(0), position(0), path(path_){
        reader_ptr=std::make_unique< FileStreamReader>(path);
        loadNextChunk();
	};
    const char_t readSymbolImpl() const {
        if (curLen < curMaxLen) {
            return chunk[curLen];
        }
        return u8'\0';
    }

    bool moveToSymbolImpl(long long dif) {
        position += dif;
        curLen += dif;

        while (curLen >= curMaxLen) {
            loadNextChunk();
        }
        return true;
    }

    bool emptyImpl() const {
        return isEmpty;
    }

    void restartImpl() {
        reader_ptr.reset();
        reader_ptr = std::make_unique< FileStreamReader>(path);
    }

    size_t getPosImpl() const {
        return position;
    }

};