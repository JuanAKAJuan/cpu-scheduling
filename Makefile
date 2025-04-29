CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

TARGET = scheduler

SOURCES = scheduler.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(SOURCES) -o $(TARGET) $(CXXFLAGS)

clean:
	rm -f $(TARGET) 

.PHONY: all clean
