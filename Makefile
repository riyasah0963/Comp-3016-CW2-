# Makefile for Echoes of the Forgotten Realm - Enhanced 3D Edition

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS = -lGL -lGLEW -lglfw -lopenal -lpthread
SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = echoes_game

.PHONY: all clean run install debug release

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

install:
	@echo "To install, copy the '$(TARGET)' executable to your desired location"
	@echo "All game data is built into the executable"

# Debug version with more verbose output
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# Release version with optimizations
release: CXXFLAGS += -O3 -DNDEBUG
release: clean $(TARGET)
