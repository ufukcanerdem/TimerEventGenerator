CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -pthread
TARGET = main
OBJS = main.o timer.o
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp timer.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
