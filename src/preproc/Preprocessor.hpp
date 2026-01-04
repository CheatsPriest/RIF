#pragma once

#include <configs/SearchConfig.hpp>
#include <preproc/PreRegisters.hpp>

class Preprocessor {
private:
	PreRegisters preRegs;

public:
	Preprocessor() {

	}
	void run() {
		preRegs.preprocess();
	}

};