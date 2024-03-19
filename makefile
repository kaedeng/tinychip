# COMMENTS BEGIN WITH A HASH

# THE NAME OF YOUR EXECUTABLE
TARGET = Chip8
# ALL CPP COMPILABLE IMPLEMENTATION FILES THAT MAKE UP THE PROJECT
SRC_FILES = $(wildcard ./src/*.cpp)

CXX = g++
CXXFLAGS = -O2 -I./include $(A) 
CXXFLAGS_DEBUG = -g
CXXFLAGS_ERRORS = -Werror -Wall -Wextra -Wconversion -Wdouble-promotion -Wunreachable-code -Wshadow -Wpedantic -pedantic-errors
CPPVERSION = -std=c++17
LIBS= $(Z) -lSDL2 -D_REENTRANT -lSDL2_image

BUILD = build
OBJECTS = $(subst ./src/, ./$(BUILD)/, $(SRC_FILES:.cpp=.o))

ifeq ($(shell echo "Windows"), "Windows")
	TARGET := $(TARGET).exe 
	DEL = del /F $(subst /,\,$(OBJECTS))
	Q = 
	X = if not exist $@
	Y = 
	Z = -lmingw32 -lSDL2main
	A = -L./libWin
else
	DEL = rm -rf $(OBJECTS)
	Q = "
	X = 
	Y = -p
	Z = 
	A = -L./libLin
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

./$(BUILD)/%.o: ./src/%.cpp | $(BUILD)
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(CPPVERSION) $(CXXFLAGS_DEBUG) $(CXXFLAGS_ERRORS) $(LIBS)

clean:
	$(DEL) $(TARGET)  Makefile.bak

$(BUILD):
	$(X) mkdir $(Y) $@

depend:
	@sed -i.bak '/^# DEPENDENCIES/,$$d' Makefile
	@$(DEL) sed*
	@echo $(Q)# DEPENDENCIES$(Q) >> Makefile
	@$(CXX) -MM $(SRC_FILES) >> Makefile


.PHONY: all clean depend build

# DEPENDENCIES
main.o: main.cpp
