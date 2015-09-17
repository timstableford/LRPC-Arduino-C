CXXFLAGS =	-g -Wall -fmessage-length=0 -std=c++11 -DLINUX
OBJDIR = build

SRCS = Object.cpp StreamParser.cpp RPC.cpp StreamParserTest.cpp TCPStreamConnector.cpp SocketRPCTest.cpp
OBJS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRCS))

LIBS = -lboost_system -lboost_thread -lpthread

TARGET = LightweightRPC

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(TARGET): setup $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

test: CXXFLAGS += -DTEST
test: $(TARGET)

setup:
	mkdir -p $(OBJDIR)

clean:
	rm -f $(OBJS) $(TARGET)
