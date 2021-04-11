#include "XiaData.h"
#include <iostream>
#include <vector>

#define MAX_NUM_CHN 17
#define MAX_NUM_MOD 10

class EventFilters {
private:
	XiaData* decodedEvent;
	std::vector<XiaData*>& decodedList_;
	int tmc[MAX_NUM_MOD][MAX_NUM_CHN];
	int (&stats)[3][MAX_NUM_MOD][MAX_NUM_CHN];
	bool& debug_mode;
	void DetTypeCheck(int i);
	void FlagsFilter(int i);
public:
	EventFilters(std::vector<XiaData*>& a, bool& b, int (&c)[3][MAX_NUM_MOD][MAX_NUM_CHN]) : debug_mode(b), decodedList_(a), stats(c)
	{}
	void ApplyFilters(int a[MAX_NUM_MOD][MAX_NUM_CHN]);
	void ApplyDelay();
};