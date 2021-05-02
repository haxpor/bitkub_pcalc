.PHONY: debug release clean

# TODO: specify bitkub_commonlib directory, you can specify at the command lines if needed
BITKUB_COMMONLIB_DIR = ../bitkub_commonlib

COMPILER = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -I.
LINKFLAGS = $(BITKUB_COMMONLIB_DIR)/bin/release/libbitkub.a
SRC_DIR = src
SOURCES = main.cpp
OBJS = $(addprefix $(SRC_DIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))
OUT = pcalc

release: CXXFLAGS += -I$(BITKUB_COMMONLIB_DIR)/bin/release/include -O3
release: $(OUT)

debug: CXXFLAGS += -I$(BITKUB_COMMONLIB_DIR)/bin/debug/include
debug: LINKFLAGS = $(BITKUB_COMMONLIB_DIR)/bin/debug/libbitkub.a
debug: $(OUT)

$(OUT): $(OBJS)
	$(COMPILER) $(CXXFLAGS) $< $(LINKFLAGS) -o $@

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(COMPILER) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OUT) $(SRC_DIR)/*.o
