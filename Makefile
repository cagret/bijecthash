CXX=g++
CXXFLAGS=-I. -std=c++11 -O2
LDFLAGS=
EXEC=BijectHash
SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)

.PHONY: all clean

