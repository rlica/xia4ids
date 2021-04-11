#include <iostream>
#include <cstring>
#include "LDFReader.h"

#define HEAD 1145128264 /// Run begin buffer
#define DATA 1096040772 /// Physics data buffer
#define DIR 542263620   /// "DIR "
#define LDF_DIR_LENGTH 8192 /// Size of DIR buffer
#define LDF_DATA_LENGTH 8193 // Maximum length of an ldf style DATA buffer.
#define ENDFILE 541478725 /// End of file buffer
#define ENDBUFF 0xFFFFFFFF /// End of buffer marker

#define ACTUAL_BUFF_SIZE 8194 /// unit of bytes!

const unsigned int end_spill_size = 20; /// The size of the end of spill "event" (5 words).

DIR_buffer::DIR_buffer() {
    bufftype = DIR;
    buffsize = LDF_DIR_LENGTH;
    buffend = ENDBUFF;
}

HEAD_buffer::HEAD_buffer() {
    bufftype = HEAD;
}

DATA_buffer::DATA_buffer() {
    bufftype = DATA;
    bcount = 0;
    good_chunks = 0;
    missing_chunks = 0;
    buff_pos = 0;
    this->Reset();
}

/// Set initial values.
void DATA_buffer::Reset() {
    curr_buffer = buffer_even;
    next_buffer = buffer_odd;
    buff_pos = 0;
    bcount = 0;
    retval = 0;
    good_chunks = 0;
    missing_chunks = 0;
}

bool DATA_buffer::read_next_buffer(std::ifstream* f_, bool force_) {
    if (!f_ || !f_->good() || f_->eof()) { 
        std::cout << "file is bad, or it's eof" << std::endl;
        return false; 
    }
    // First data buffer, just read like normal
    if (bcount == 0) {
        f_->read((char*)buffer_even, ACTUAL_BUFF_SIZE * 4);
    }

    // Not beginning of data buffer, check if not end of buffer
    else if (buff_pos + 3 <= ACTUAL_BUFF_SIZE - 1 && !force_) {
        // Don't need to scan a new buffer yet, this buffer hasn't reached the end.

        // Skip end of event delimiters
        while (curr_buffer[buff_pos] == ENDBUFF &&
            buff_pos < ACTUAL_BUFF_SIZE - 1) {
            buff_pos++;
        }

        // If we have more good words in this buffer, keep reading it.
        if (buff_pos + 3 < ACTUAL_BUFF_SIZE - 1) {
            return true;
        }
    }

    // Read the buffer into memory.
    if (bcount % 2 == 0) {
        f_->read((char*)buffer_odd, ACTUAL_BUFF_SIZE * 4); // read and store in advance, then give to curr_buffer in next loop
        curr_buffer = buffer_even;
        next_buffer = buffer_odd;
    }
    else {
        f_->read((char*)buffer_even, ACTUAL_BUFF_SIZE * 4); // read and store in advance, then give to curr_buffer in next loop
        curr_buffer = buffer_odd;
        next_buffer = buffer_even;
    }

    // Reset the buffer index.
    buff_pos = 0;

    // Increment the number of buffers read.
    bcount++;

    // Read the buffer header and length. This actually refers to the "sub-buffer", i.e. chunk of the entire data buffer.
    buff_head = curr_buffer[buff_pos++];
    buff_size = curr_buffer[buff_pos++];

    if (!f_->good()) { return false; }
    else if (f_->eof()) { retval = 2; }

    return true;
}

bool DATA_buffer::Read(std::ifstream* file_, char* data_, unsigned int& nBytes,
    unsigned int max_bytes_, bool& full_spill,
    bool& bad_spill, bool& debug_mode) {
    if (!file_ || !file_->is_open() || !file_->good()) {
        retval = 6;
        return false;
    }

    bad_spill = false;

    bool first_chunk = true;
    unsigned int this_chunk_sizeB;
    unsigned int total_num_chunks = 0;
    unsigned int current_chunk_num = 0;
    unsigned int prev_chunk_num;
    unsigned int prev_num_chunks;
    nBytes = 0; // Set the number of output bytes to zero

    while (true) {
        if (!read_next_buffer(file_)) {
            if (debug_mode) {
                std::cout << "debug: failed to read from input data file\n";
            }
            retval = 6;
            return false;
        }

        if (buff_head == DATA) {
            prev_chunk_num = current_chunk_num;
            prev_num_chunks = total_num_chunks;

            this_chunk_sizeB = curr_buffer[buff_pos++];
            total_num_chunks = curr_buffer[buff_pos++];
            current_chunk_num = curr_buffer[buff_pos++];

            if (debug_mode) {
                std::cout << "debug: scanning spill chunk " << current_chunk_num
                    << " of " << total_num_chunks << "\n";
            }

            /// Capture all possible issues related to bad/missing chunks.
            // Check if this is a spill fragment.
            if (first_chunk) { // Check for starting read in middle of spill.
                if (current_chunk_num != 0) {
                    if (debug_mode) {
                        std::cout
                            << "debug: starting read in middle of spill (chunk "
                            << current_chunk_num << " of "
                            << total_num_chunks << ")\n";
                    }

                    // Update the number of dropped chunks.
                    missing_chunks += current_chunk_num;

                    full_spill = false;
                }
                else { full_spill = true; }
                first_chunk = false;
            }
            // Total number of chunks informed by previous and current buffers mismatched.
            else if (total_num_chunks != prev_num_chunks) {
                if (debug_mode) {
                    std::cout << "debug: skipped to new spill with "
                        << total_num_chunks
                        << " spill chunks without reading footer of old spill\n";
                }

                // We are likely out of position in the spill. Scrap this current buffer and skip to the next one.
                read_next_buffer(file_, true);

                // Update the number of dropped chunks.
                missing_chunks += (prev_num_chunks - 1) - prev_chunk_num;

                retval = 4;
                return false;
            }
            // The chunk number jumped (more than 1).
            else if (current_chunk_num !=
                prev_chunk_num + 1) { // Missing a spill chunk.
                if (debug_mode) {
                    if (current_chunk_num == prev_chunk_num + 2) {
                        std::cout << "debug: missing single spill chunk ("
                            << prev_chunk_num + 1 << ")\n";
                    }
                    else { // Missing multiple spill chunks.
                        std::cout << "debug: missing multiple spill chunks ("
                            << prev_chunk_num + 1 << " to "
                            << current_chunk_num - 1 << ")\n";
                    }
                }
                full_spill = false;

                // We are likely out of position in the spill. Scrap this current buffer and skip to the next one.
                read_next_buffer(file_, true);

                // Update the number of dropped chunks.
                missing_chunks += (current_chunk_num - 1) - prev_chunk_num;

                retval = 4;
                return false;
            }

            // Construct the spill.
            // Case 1: this chunk is the last chunk of the DATA buffer, i.e. end of spill.
            if (current_chunk_num == total_num_chunks - 1) { // Spill footer
                // Check if the size of the last buffer, i.e. the end of the spill is correct.
                if (this_chunk_sizeB != end_spill_size) {
                    if (debug_mode) {
                        std::cout << "debug: spill footer (chunk "
                            << current_chunk_num << " of "
                            << total_num_chunks << ") has size "
                            << this_chunk_sizeB << " != 5\n";
                    }

                    // We are likely out of position in the spill. Scrap this current buffer and skip to the next one.
                    read_next_buffer(file_, true);

                    retval = 5;
                    return false;
                }
                else if (debug_mode) {
                    if (full_spill) {
                        std::cout << "debug: finished scanning spill of "
                            << nBytes << " bytes\n";
                    }
                    else {
                        std::cout
                            << "debug: finished scanning spill fragment of "
                            << nBytes << " bytes\n";
                    }
                }

                // Copy data into the output array.
                memcpy(&data_[nBytes], &curr_buffer[buff_pos], 8);
                if (debug_mode) {
                    std::cout << "debug: spill footer words are "
                        << curr_buffer[buff_pos] << " and "
                        << curr_buffer[buff_pos + 1] << std::endl;
                }
                nBytes += 8;
                buff_pos += 2;

                retval = 0;
                return true;
            }
            else { // Normal spill chunk
                unsigned int copied_bytes;
                if (this_chunk_sizeB <= 12) {
                    if (debug_mode) {
                        std::cout << "debug: invalid number of bytes in chunk "
                            << current_chunk_num + 1 << " of "
                            << total_num_chunks << ", "
                            << this_chunk_sizeB << " B!\n";
                    }

                    // Update the number of dropped chunks.
                    missing_chunks++;

                    retval = 4;
                    return false;
                }

                // Otherwise the chunk is good, proceed to store.
                // Update the number of good spill chunks.
                good_chunks++;

                // Number of copied bytes excludes the first 12 first bytes from 
                // chunk_sizeB, total_num_chunks, this_chunk_num
                copied_bytes = this_chunk_sizeB - 12;
                memcpy(&data_[nBytes], &curr_buffer[buff_pos],
                        copied_bytes);
                nBytes += copied_bytes;
                buff_pos += copied_bytes / 4; /// unit of words
            }
        }
        // If buff_head is not DATA, check if it is EOF
        else if (buff_head ==
            ENDFILE) { // Catch the start of the end-of-file buffer and return
            if (next_buffer[0] == ENDFILE) {
                if (debug_mode) {
                    std::cout
                        << "debug: encountered double EOF buffer marking end of file\n";
                }
                retval = 2;
            }
            else {
                if (debug_mode) {
                    std::cout
                        << "debug: encountered EOF buffer marking end of run\n";
                }

                // We need to skip this buffer.
                read_next_buffer(file_, true);

                retval = 1;
            }
            return false;
        }
        // buff_head is neither DATA nor EOF, skip this.
        else { // Skip the entire buffer
            if (debug_mode) {
                std::cout << "debug: encountered non DATA type buffer 0x"
                    << std::hex << buff_head << std::dec << "\n";
                std::cout << "debug: skipping entire remaining " << buff_size
                    << " buffer words!\n";
            }

            // This is not a data buffer. We need to force a scan of the next buffer.
            // read_next_buffer will not scan the next buffer by default because it
            // thinks there are still words left in this buffer to read.
            read_next_buffer(file_, true);

            retval = 3;
            continue;
        }
    }

    return false;
}