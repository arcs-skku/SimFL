CC = g++
INCLUDES = -I${XILINX_XRT}/include -I${XILINX_VIVADO}/include -I${SIMFL}/include
LIBS = -L${XILINX_XRT}/lib -L${SIMFL}/lib -lOpenCL -lSimFL -lpthread -lrt -lstdc++
CFLAGS = -fmessage-length=0 -Wall -O0 -g -std=c++14
TARGET = vadd
SOURCE = vadd.cpp

$(TARGET) : $(SOURCE)
	$(CC) $(INCLUDES) $(CFLAGS) -o $@ $^ $(LIBS)

clean :
	rm $(TARGET)
