CC=g++
OFLAGS=-O3 -g -Wall
SRCS=main.cc dataarray.cc accessinterface.cc cache.cc mainmemory.cc memoryinterface.cc vpi_memory.cc
HEADERS=param.hh dataarray.hh accessinterface.hh cache.hh mainmemory.hh memoryinterface.hh vpi_memory.hh
OBJS=$(SRCS:.cc=.o)
TARGET=test.out

VPI_SRC=cache-vpi.cc dataarray.cc accessinterface.cc cache.cc mainmemory.cc memoryinterface.cc vpi_memory.cc
VPI_MODULE=cache-vpi

VPI_VVP=test.vvp
VERILOG_SRC=test_cache.v

all: $(TARGET)

.PHONY: run	
run:
	./$(TARGET)	

$(TARGET): $(OBJS) $(HEADERS)
	$(CC) $(OFLAGS) $(OBJS) -o $(TARGET)

$(HEADERS): 
.cc.o:
	$(CC) $(OFLAGS) -c $<

.PHONY: valgrind
valgrind:
	valgrind --leak-check=full ./$(TARGET)

.PHONY: vpi-all
vpi-all: vpi compile vrun

.PHONY: vpi
vpi:
	iverilog-vpi $(VPI_SRC)

.PHONY: compile
compile:
	iverilog -o$(VPI_VVP) $(VERILOG_SRC)

.PHONY: vrun
vrun:
	vvp -M. -m$(VPI_MODULE) $(VPI_VVP)

.PHONY: clean
clean:
	rm -f $(TARGET) $(GEN) *.o *.vvp *.vpi
