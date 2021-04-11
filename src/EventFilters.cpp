#include <stdio.h>
#include <string.h>
#include "EventFilters.h"

void EventFilters::ApplyFilters(int a[MAX_NUM_MOD][MAX_NUM_CHN]) {
	// First we copy the input tmc map.
	memcpy(tmc, a, MAX_NUM_MOD * MAX_NUM_CHN * sizeof(int));
	// Apply all filters to each event in the list.
	for (int i = decodedList_.size() - 1; i >= 0; i--) {
		decodedEvent = decodedList_[i];
		// Update statistics info before applying filters.
		stats[2][decodedEvent->GetModuleNumber()][decodedEvent->GetChannelNumber()]++; // total
		if (decodedEvent->IsPileup() || decodedEvent->IsSaturated()){
			stats[1][decodedEvent->GetModuleNumber()][decodedEvent->GetChannelNumber()]++; // fail
			if (decodedEvent->IsPileup()) {
				stats[0][decodedEvent->GetModuleNumber()][decodedEvent->GetChannelNumber()]++; // pileup
			}
		}
		//DetTypeCheck(i);
		//FlagsFilter(i);
	}
}

void EventFilters::DetTypeCheck(int i) {
	if (tmc[decodedEvent->GetModuleNumber()][decodedEvent->GetChannelNumber()] == 0) {
		// if(debug_mode) {
		// 	std::cout << "Data number: " << i << " has undefined detector type (0)." << std::endl;
		// 	std::cout << "Module number: " << decodedEvent->GetModuleNumber() << "; Channel Number: " << decodedEvent->GetChannelNumber();
		// 	std::cout << "Dectector type: " << tmc[decodedEvent->GetModuleNumber()][decodedEvent->GetChannelNumber()] << std::endl;
		// }
		// decodedList_.erase(decodedList_.begin() + i);
	}
}

void EventFilters::FlagsFilter(int i) {
	if (decodedEvent->IsPileup() || decodedEvent->IsSaturated()) {
		// if (debug_mode)
		// 	std::cout << "Data number: " << i << " is a pileup and/or an out-of-range." << std::endl;
		// decodedList_.erase(decodedList_.begin() + i);
	}
}
