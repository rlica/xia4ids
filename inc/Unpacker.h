#pragma once
#include <map>
#include <utility>
#include <string>
#include <vector>
#include <iostream>
#include "XiaData.h"
#include "XiaListModeDataMask.h"

using namespace std;

class Unpacker {
public:
	/// Default constructor.
	Unpacker() {
		InitializeMaskMap(); }

	bool ReadSpill(std::vector<XiaData*>& decodedList, unsigned int* data, unsigned int nWords, bool is_verbose, bool& debug_mode);
	int DecodeBuffer(std::vector<XiaData*>& result, unsigned int* buf, const unsigned int& vsn, bool& debug_mode);
	std::pair<unsigned int, unsigned int> DecodeWordZero(const unsigned int& word, XiaData& data,
		const XiaListModeDataMask& mask);
	void DecodeWordTwo(const unsigned int& word, XiaData& data, const XiaListModeDataMask& mask);
	unsigned int DecodeWordThree(const unsigned int& word, XiaData& data,
		const XiaListModeDataMask& mask);
	/// Method calculates time stamp from event times and mask info (frequency).
	/// returns <incomplete time stamp (without Cfd fractional time), time stamp>
	pair<double, double> CalculateTimeInSamples(const XiaListModeDataMask& mask,
		const XiaData& data);
private:
	map<unsigned int, pair<string, unsigned int> > maskMap_;///< Maps firmware/frequency to module number.
	void InitializeMaskMap();
};
