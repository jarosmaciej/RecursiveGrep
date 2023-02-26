CXX = g++

SRCS := $(wildcard src/*.cpp)
OBJS := $(SRCS:.cpp=.o)

TARGET = RecursiveGrep

LDFLAGS += -pthread

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: clean
