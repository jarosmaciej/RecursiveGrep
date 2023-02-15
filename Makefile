CXX = g++

SRCS := $(wildcard src/*.cpp)
OBJS := $(SRCS:.cpp=.o)

TARGET = RecursiveGrep

CXXFLAGS += -I/include

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^

%.o: %.cpp
	$(CXX) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: clean

ifneq ($(MAKECMDGOALS), clean)
  include $(SRCS:.cpp=.d)
endif

%.d: %.cpp
	set -e; $(CXX) -MM $< $(CXXFLAGS) > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
