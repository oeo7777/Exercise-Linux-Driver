
CC= gcc
LD= ld
AR= ar
CXX= g++
CFLAG = cc -c -fPIC 
CSFLAG = -shared -o
LFLAG = -lhello_call_wo -L./

objects = world.o hello_call_wo.o call_he_wo.o
#export LD_LIBRARY_PATH=/home/sam/code/driver_code/lib_so_test
#export 此在console 使用才成功

all: 
	make world
	make hello_call_wo
	make call_he_wo

world:
	$(CFLAG) world.c 
	cc $(CSFLAG) libworld.so world.o

hello_call_wo: world
	$(CFLAG) hello_call_wo.c
	cc $(CSFLAG) libhello_call_wo.so hello_call_wo.o -lworld -L.

#-rpath multi path 使用方式如下
call_he_wo: world hello_call_wo
	cc call_he_wo.c -lhello_call_wo -lworld -L. -Wl,-rpath='$$ORIGIN:$$ORIGIN/hide' -Wl,--unresolved-symbols=ignore-all
	export LD_LIBRARY_PATH=/home/sam/code/driver_code/lib_so_test
	

clean:
	rm -f $(objects) *.so*

