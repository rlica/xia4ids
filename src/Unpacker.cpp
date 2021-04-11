#include "Unpacker.h"
#include <iostream>
#include <vector>
#include <cmath>

using namespace DataProcessing;

bool Unpacker::ReadSpill(std::vector<XiaData*>& decodedList, unsigned int* data, unsigned int nWords, bool is_verbose/*=true*/, bool& debug_mode) {
    const unsigned int maxVsn = 14; // No more than 14 pixie modules per crate
    unsigned int nWords_read = 0;

    int retval = 0; // return value from various functions, for debug purposes

    unsigned int lenRec = 0xFFFFFFFF;
    unsigned int vsn = 0xFFFFFFFF;
    unsigned int lastVsn = 0xFFFFFFFF; // the last vsn read from the data
    bool fullSpill = false; // True if spill had all vsn's

    // While the current location in the buffer has not gone beyond the end
    // of the buffer (ignoring the last three delimiters, continue reading
    while (nWords_read <= nWords) {
        // First we need to skip the delimiter.
        while (data[nWords_read] == 0xFFFFFFFF)
            nWords_read++;

        // Retrieve the record length and the vsn number
        lenRec = data[nWords_read]; // Number of words in this record
        vsn = data[nWords_read + 1]; // Module number
        if (debug_mode) {
            std::cout << "Record length: " << lenRec << std::endl;
            std::cout << "Module number (vsn): " << vsn << std::endl;
        }
        //// Check sanity of record length and vsn
        //if (lenRec > maxWords || (vsn > maxVsn && vsn != 9999 && vsn != 1000)) {
        //    if (is_verbose)
        //        std::cout << "ReadSpill: SANITY CHECK FAILED: lenRec = " << lenRec << ", vsn = " << vsn << ", read "
        //        << nWords_read << " of " << nWords << std::endl;
        //    return false;
        //}

                // If the record length is 6, this is an empty channel.
        // Skip this vsn and continue with the next
        ///@TODO Revision specific, so move to ReadBuffData
        if (lenRec == 6) {
            if (debug_mode)
                std::cout << "Record length is 6, i.e. empty channel!" << std::endl;
            nWords_read += lenRec;
            lastVsn = vsn;
            continue;
        }

        // If both the current vsn inspected is within an acceptable
        // range, begin reading the buffer.
        if (vsn < maxVsn) {
            if (lastVsn != 0xFFFFFFFF && vsn != lastVsn + 1) {
                if (is_verbose)
                    std::cout << "ReadSpill: MISSING BUFFER " << lastVsn + 1 << ", lastVsn = " << lastVsn << ", vsn = "
                    << vsn << ", lenrec = " << lenRec << std::endl;
                //ClearEventList();
                fullSpill = false; // data from a module is missing, hence spill isn't complete?
            }

            // Decode the buffer, pass the buffer only from the first non-delimiter.
            // In this context, buffer means the unit of data that stores the spill (not 8092 bytes unit).
            retval = DecodeBuffer(decodedList, &data[nWords_read], vsn, debug_mode);
            lastVsn = vsn;
            nWords_read += lenRec;
        }
        else if (vsn == 9999) {
			if (debug_mode)
				std::cout << "vsn = 9999, close this ReadSpill loop." << std::endl;
			break;
		}
    }
    if (vsn == 9999 || vsn == 1000) {
		fullSpill = true;
		nWords_read += 2; //skip the vsn and lenRec words.
		lastVsn = 0xFFFFFFFF;
	}
	if (debug_mode) {
		if (nWords_read != nWords)
			std::cout << "ReadSpill: Received spill of " << nWords << " words, but read " << nWords_read << " words\n";
		else
			std::cout << "All words in this spill are read!\n";
		}
    return true;
}

int Unpacker::DecodeBuffer(std::vector<XiaData*>& result, unsigned int* buf, const unsigned int& vsn, bool& debug_mode) {
    XiaListModeDataMask mask_; ///< Object providing the masks necessary to decode the spill data.

    if (debug_mode)
        std::cout << "Checking vsn, vsn = " << vsn << std::endl;
    // Using the defined map from vsn to (firmware, frequency), set firmware and frequency to the mask object.
    if (maskMap_.size() != 0) {
        auto found = maskMap_.find(vsn);
        if (found == maskMap_.end())
            throw invalid_argument("Unpacker::ReadBuffer - Unable to locate VSN = " + to_string(vsn)
                + " in the maskMap. Ensure that it's defined in your configuration file!");
        mask_.SetFirmware((*found).second.first);
        mask_.SetFrequency((*found).second.second);
        if (debug_mode)
            std::cout << "Both firmware and frequency are set!" << std::endl;
    }
    else
        std::cout << "Mask map is empty!" << std::endl;
    // As the mask now contains firmware and frequency information, we can start decoding the spill.
    unsigned int* bufStart = buf;
    ///@NOTE : These two pieces here are the Pixie Module Data Header. They
    /// tell us the number of words read from the module (bufLen) and the VSN
    /// of the module (module number).
    unsigned int bufLen = *buf++;
    unsigned int modNum = *buf++;
    if (debug_mode){
        std::cout << "Buffer (unit of spill data) length is " << bufLen << std::endl;
        std::cout << "Module number of this data signal (event) is " << modNum << std::endl;
    }
    bool read_header_mode = true; //if this is true, we only parse first 4 words.
    while (buf < bufStart + bufLen) {
        XiaData* data = new XiaData();
        bool hasExternalTimestamp = false;

        // Decode 4 words HEADER.
        pair<unsigned int, unsigned int> lengths =
            DecodeWordZero(buf[0], *data, mask_);
        unsigned int headerLength = lengths.first;
        unsigned int eventLength = lengths.second;

        data->SetEventTimeLow(buf[1]); //Word One
        DecodeWordTwo(buf[2], *data, mask_);
        unsigned int traceLength = DecodeWordThree(buf[3], *data, mask_);

        // Reset energy to 0 in case of pileup or out-of-range.
        if (data->IsSaturated() || data->IsPileup())
            data->SetEnergy(0);

        // Calculate time stamp.
        pair<double, double> times = CalculateTimeInSamples(mask_, *data);
        data->SetTimeSansCfd(times.first); // Filter ticks. For 250MHz: 8ns.
        data->SetTime(times.second); // ADC ticks. For 250MHz: 4ns.

        // Sanity check for event length (formula in PIXIE manual)
        // One last check to ensure event length matches what we think it
        // should be.
        if (traceLength / 2 + headerLength != eventLength) {
            cerr << "XiaListModeDataDecoder::ReadBuffer : Event"
                "length (" << eventLength << ") does not correspond to "
                "header length (" << headerLength
                << ") and trace length ("
                << traceLength / 2 << ")" << endl;
            result = vector<XiaData*>();
            return -1;
        }
        else {//Advance the buffer past the header and to the trace
            // if (debug_mode) {
            //     cout << "Decoded 4 words of HEADER" << endl;
            //     cerr << "Sanity check passed: trace-header-event lengths are compatible!" << endl;
            //     cout << "Trace length = " << traceLength << endl;
            //     cout << "Header length = " << headerLength << endl;
            //     cout << "Event length = " << eventLength << endl;
            // }
            buf += headerLength;
        }

        // Trace parsing is done here if necessary!!!
        result.push_back(data);
        // result[i++] = data;

        // if (read_header_mode) {
        //     if (debug_mode)
        //         cout << "Parsing 4 words of HEADER only, task done!" << endl;
        //     return 0;
        // }

        buf += (eventLength - headerLength); // skip the rest of the event, read the next signal.

        // else keep going to parse trace data.
    }

    return 0;
}

std::pair<unsigned int, unsigned int> Unpacker::DecodeWordZero(const unsigned int& word, XiaData& data,
    const XiaListModeDataMask& mask) {

    data.SetChannelNumber(word & mask.GetChannelNumberMask().first);
    data.SetSlotNumber((word & mask.GetSlotIdMask().first) >> mask.GetSlotIdMask().second);
    data.SetCrateNumber((word & mask.GetCrateIdMask().first) >> mask.GetCrateIdMask().second);
    data.SetPileup((word & mask.GetFinishCodeMask().first) != 0);

    //We have to check if we have one of these three firmwares since they
    // have the Trace-Out-of-Range flag in this word.
    switch (mask.GetFirmware()) {
    case R17562:
    case R20466:
    case R27361:
        data.SetSaturation((bool)((word & mask.GetTraceOutOfRangeFlagMask().first)
            >> mask.GetTraceOutOfRangeFlagMask().second));
        break;
    default:
        break;
    }

    return make_pair((word & mask.GetHeaderLengthMask().first) >> mask.GetHeaderLengthMask().second,
        (word & mask.GetEventLengthMask().first) >> mask.GetEventLengthMask().second);
}

void Unpacker::DecodeWordTwo(const unsigned int& word, XiaData& data, const XiaListModeDataMask& mask) {
    data.SetEventTimeHigh(word & mask.GetEventTimeHighMask().first);
    data.SetCfdFractionalTime((word & mask.GetCfdFractionalTimeMask().first) >> mask.GetCfdFractionalTimeMask().second);
    data.SetCfdForcedTriggerBit((bool)((word & mask.GetCfdForcedTriggerBitMask().first) >> mask.GetCfdForcedTriggerBitMask().second));
    data.SetCfdTriggerSourceBit((bool)((word & mask.GetCfdTriggerSourceMask().first) >> mask.GetCfdTriggerSourceMask().second));
}

unsigned int Unpacker::DecodeWordThree(const unsigned int& word, XiaData& data,
    const XiaListModeDataMask& mask) {
    data.SetEnergy(word & mask.GetEventEnergyMask().first);

    //Reverse the logic that we used in DecodeWordZero, since if we do not
    // have these three firmwares we need to check this word for the
    // Trace-Out-of-Range flag.
    switch (mask.GetFirmware()) {
    case R17562:
    case R20466:
    case R27361:
        break;
    default:
        data.SetSaturation((bool)((word & mask.GetTraceOutOfRangeFlagMask().first)
            >> mask.GetTraceOutOfRangeFlagMask().second));
        break;
    }

    return ((word & mask.GetTraceLengthMask().first) >> mask.GetTraceLengthMask().second);
}

void Unpacker::InitializeMaskMap() {
    maskMap_.insert(make_pair(0, make_pair("42950", 250)));
    maskMap_.insert(make_pair(1, make_pair("42950", 250)));
}

pair<double, double> Unpacker::CalculateTimeInSamples(const XiaListModeDataMask& mask,
    const XiaData& data) {
    double filterTime = data.GetEventTimeLow() + data.GetEventTimeHigh() * pow(2., 32);

    double cfdTime = 0, multiplier = 1;
    if (mask.GetFrequency() == 100)
        cfdTime = data.GetCfdFractionalTime() / mask.GetCfdSize();

    if (mask.GetFrequency() == 250) {
        multiplier = 2;
        cfdTime = data.GetCfdFractionalTime() / mask.GetCfdSize() - data.GetCfdTriggerSourceBit();
    }

    if (mask.GetFrequency() == 500) {
        multiplier = 5; // Following Pixie manual v 3.03, which is different from PAASS version that follows v 3.07.
        cfdTime = data.GetCfdFractionalTime() / mask.GetCfdSize() + data.GetCfdTriggerSourceBit() - 1;
        //From the Pixie Manual v 3.07 it seems that the 500Mhz has 4 interlaced ADCs so its list mode has a 2bit CfdTriggerSource.
        //These methods will need to be updated to account for this, and soon. (T.T. King Feb,7 2019)
    }

    //Moved here so we can use the multiplier to adjust the clock tick units. So GetTime() returns the ADC ticks and GetTimeSansCfd() returns Filter Ticks
    //(For 250MHZ) This way GetTime() always returns 4ns clock ticks, and GetTimeSansCfd() returns the normal 8ns ticks
    if (data.GetCfdFractionalTime() == 0 || data.GetCfdForcedTriggerBit())
        return make_pair(filterTime, filterTime * multiplier);

    return make_pair(filterTime, filterTime * multiplier + cfdTime);
}
