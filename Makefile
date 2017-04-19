OBJS = pa5.o ppmFile.o

all: $(OBJS)
	nvcc -arch=sm_20 $(OBJS) -o pa5.x

%.o: %.c
	nvcc -x cu -arch=sm_20 -I. -dc $< -o $@

clean:
	rm *.o *.x *blured*

cleanblurr:
	rm blur/*
