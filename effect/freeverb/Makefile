
CXX = g++
CXXFLAGS = -Wall -IComponents -IFreeverbwrap
TARGET := Freeverbtest

SRCS := Components/allpass.cpp Components/comb.cpp Components/revmodel.cpp \
		Freeverb.cpp FreeverbMain.cpp

OBJS := $(SRCS:.cpp=.o)

$(info "SRCS are $(SRCS))
$(info OBJS are $(OBJS))

all: $(TARGET)


$(TARGET) : $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^


%.o : %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -rf $(OBJS) $(TARGET)

.PHONY : clean all
