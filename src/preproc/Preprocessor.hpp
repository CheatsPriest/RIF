#pragma once

#include <configs/SearchConfig.hpp>
#include <preproc/PreRegisters.hpp>
#include <preproc/PreSynonyms.hpp>

class Preprocessor {
private:
	PreRegisters preRegs;
	PreSynonyms preSynoms;

public:
	Preprocessor() {

	}
	void run() {
		preRegs.preprocess();
		preSynoms.run();
	}

};