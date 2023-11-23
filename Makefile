.DEFAULT_GOAL := all

.PHONY = all run clean run_tests debug

CC = c++

BIN_DIR = ./bin
INC_DIR = ./inc
SRC_DIR = ./src
OBJ_DIR = ./obj

TARGET = $(BIN_DIR)/xia4ids
INC_FLAGS = -I $(INC_DIR)

# Address Sanitizer (yum install libasan)
# CC_FLAGS = $(INC_FLAGS) -g -fsanitize=address `root-config --cflags`
# LD_FLAGS = -fsanitize=address `root-config --glibs`

# Normal compile
CC_FLAGS = $(INC_FLAGS) -g -O3 -std=c++17 `root-config --cflags`
LD_FLAGS = `root-config --glibs`


HEADERS = \
			calibrate.hh \
			correlations.hh \
			define_root.hh \
			event_builder.hh \
			event_builder_list.hh \
			event_builder_tree.hh \
			Exceptions.h \
			HelperEnumerations.hpp \
			LDFReader.h \
			merge_sort.hh \
			xia4ids.hh \
			read_cal.hh \
			read_config.hh \
			read_ldf.hh \
			Unpacker.h \
			write_correlations.hh \
			write_gasp.hh \
			write_list.hh \
			write_stats.hh \
			write_time.hh \
			XiaData.h \
			XiaListModeDataMask.h


			SRC = \
			LDFReader.cpp \
			xia4ids.cpp \
			Unpacker.cpp \
			XiaData.cpp \
			XiaDataListModeMask.cpp


SRC_FILES = $(patsubst %.cpp, $(SRC_DIR)/%.cpp, $(SRC))
OBJ_FILES = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRC))
HEADER_FILES = $(patsubst %, $(INC_DIR)/%, $(HEADERS))

all: $(TARGET)
	@echo Everything is now built

$(TARGET): $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@ $(LD_FLAGS)


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADER_FILES)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CC_FLAGS) $< -c -o $@


clean:
	@rm -rf $(OBJ_DIR) $(BIN_DIR)
