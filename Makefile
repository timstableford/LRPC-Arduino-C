CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -std=c++11 -DLINUX
OBJDIR = build

SRCS = Object.cpp StreamParser.cpp RPC.cpp StreamParserTest.cpp TCPStreamConnector.cpp LightweightRPC.cpp
OBJS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRCS))

LIBS = -lboost_system -lboost_thread

TARGET = LightweightRPC

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(TARGET): setup $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

test: CXXFLAGS += -DTEST
test: clean $(TARGET)

setup:
	mkdir -p $(OBJDIR)

clean:
	rm -f $(OBJS) $(TARGET)
