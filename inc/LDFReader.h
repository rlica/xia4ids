//#ifndef __LDF_READER_H__
//#define __LDF_READER_H__
#include <fstream>
#include <iostream>
#include <cstring>

#define ACTUAL_BUFF_SIZE 8194

class BufferType {
protected:
    unsigned int bufftype;
    unsigned int buffsize;
    unsigned int buffend;
    unsigned int zero;

    /// Returns only false if not overloaded
    bool Write(std::ofstream* file_);

public:
    unsigned int GetBufferType() { return bufftype; }

    unsigned int GetBufferSize() { return buffsize; }

    unsigned int GetBufferEndFlag() { return buffend; }

    ///// Return true if the first word of the current buffer is equal to this buffer type
    //bool ReadHeader(std::ifstream* file_);
};

class DIR_buffer : public BufferType {
private:
    unsigned int unknown[3];
    unsigned int runnum;
public:
    DIR_buffer();
    void SetRunNum(unsigned int readrunnum) { runnum = readrunnum; }
    void SetUnknown(unsigned int* readunknown) { memcpy(unknown, readunknown, 3*sizeof(unsigned int)); }
    unsigned int* GetUnknown() { return unknown; }
    unsigned int GetRunNum() { return runnum; }
};

class HEAD_buffer : public BufferType {
private:
    char facility[9];
    char format[9];
    char type[17];
    char date[17];
    char run_title[81];
    unsigned int run_num;

public:
    HEAD_buffer();
};

class DATA_buffer : public BufferType {
private:
    int retval; /// The error code for the read method.

    unsigned int buffer_even[ACTUAL_BUFF_SIZE]; /// Container for even ldf buffer.
    unsigned int buffer_odd[ACTUAL_BUFF_SIZE]; /// Container for odd ldf buffer.
 
    unsigned int* curr_buffer; /// Pointer to the current ldf buffer.
    unsigned int* next_buffer; /// Pointer to the next ldf buffer.

    unsigned int bcount; /// The total number of ldf buffers read from file.
    unsigned int buff_head; /// The ldf buffer header ID.
    unsigned int buff_size; /// Total size of ldf buffer (in 4 byte words).
    unsigned int good_chunks; /// Count of the number of good spill chunks which were read.
    unsigned int missing_chunks; /// Count of the number of missing spill chunks which were dropped.

    unsigned int buff_pos; /// The actual position in the current ldf buffer.
public:
    DATA_buffer();
    bool read_next_buffer(std::ifstream* f, bool force = false); //force is used to force read next buffer when the current one is bad
    void SetRetVal(int inputval) { retval = inputval; }
    bool Read(std::ifstream* file_, char* data_, unsigned int& nBytes_,
        unsigned int max_bytes_, bool& full_spill,
        bool& bad_spill, bool& debug_mode);

    unsigned int GetBuffHead() { return buff_head; }
    unsigned int GetBuffSize() { return buff_size; }
    unsigned int GetBuffPos() { return buff_pos; }
    unsigned int* GetCurrBuffer() { return curr_buffer; }
    unsigned int* GetNextBuffer() { return next_buffer; }
    /// Return the number of good spill chunks which were read.
    unsigned int GetNumChunks() { return good_chunks; }
    /// Return the number of missing or dropped spill chunks.
    unsigned int GetNumMissing() { return missing_chunks; }
    int GetRetval() { return retval; }

    void IncrementBuffPos() { buff_pos++; }
    void IncrementGoodChunks() { good_chunks++; }
    void AddBuffPos(int buff_pos_steps) { buff_pos += buff_pos_steps; }
    void AddMissingChunks(unsigned int new_missing_chunks) { missing_chunks += new_missing_chunks; }
    virtual void Reset();/// Set initial values.
};

class EOF_buffer : public BufferType {};

class LDF_file {
private:
    std::ifstream input_file; /// Main input file (binary file)
    std::streampos file_length; /// Main input file length (in bytes).
    std::string file_name;

public:
    LDF_file(const std::string a) : file_name(a) {}
    std::ifstream& GetFile() { return input_file; }
    std::string GetName() { return file_name; }
    void SetLength(std::streampos length) { file_length = length; }
    std::streampos GetFileLength() { return file_length; }
};

//#endif