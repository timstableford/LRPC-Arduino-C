CXXFLAGS =	-g -Wall -fmessage-length=0 -std=c++11 -fPIC -DLINUX
OBJDIR = build
TESTDIR = test

SRCS = Object.cpp StreamParser.cpp RPC.cpp TCPStreamConnector.cpp
OBJS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRCS))

TEST_SRCS = $(TESTDIR)/SocketRPCTest.cpp $(TESTDIR)/StreamParserTest.cpp
TEST_OBJS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(TEST_SRCS))

LIBS = -lboost_system -lboost_thread -lpthread

TARGET = lightweightrpc
TEST_TARGET = lwrpc_tests

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(shell pwd) -c -o $@ $<

$(TARGET): setup $(OBJS)
	$(CXX) -shared -o build/lib$(TARGET).so $(OBJS) 
	$(AR) crf build/lib$(TARGET).a $(OBJS)

test: $(TARGET) $(TEST_OBJS)
	$(CXX) -o $(OBJDIR)/$(TEST_TARGET) $(TEST_OBJS) $(LIBS) $(OBJDIR)/lib$(TARGET).a

all: $(TARGET)

setup:
	mkdir -p $(OBJDIR)
	mkdir -p $(OBJDIR)/test

clean:
	rm -rf $(OBJDIR)
