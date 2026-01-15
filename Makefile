CC := gcc
CXX := g++
CFLAGS := -Wall -Wextra -I. -Ieffect -Ieffect/fireverb -Ieffect/freeverb -Ieffect/freeverb/Components
CXXFLAGS := -Wall -Wextra -std=c++11 -I. -Ieffect -Ieffect/fireverb -Ieffect/freeverb -Ieffect/freeverb/Components
LDFLAGS := -lasound -lm -lstdc++
TARGET = karaok

OBJS = karaok.o ringbuf.o fir_reverb.o \
       effect/freeverb/Components/allpass.o \
       effect/freeverb/Components/comb.o \
       effect/freeverb/Components/revmodel.o \
       effect/freeverb/Freeverb.o \
       effect/freeverb/freeverbwrap.o \
       effect/freeverb/convert_format.o \
	   effect/reverb_engine.o


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@

# C文件
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# C++文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 特殊处理reverb.o（因为路径问题）
fir_reverb.o: effect/fireverb/fir_reverb.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
