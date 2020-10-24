# Project: ZeDemo
# Makefile created by Dev-C++ 4.9.9.2

CPP  = ~/.local/dolcesdk/bin/arm-dolce-eabi-g++
CC   = ~/.local/dolcesdk/bin/arm-dolce-eabi-gcc
WINDRES = windres
RES  = 
OBJ  = engine3d.o env1.o generate3d.o precalcs.o render3d.o bitfonts.o ZeDemo.o effects.o env3.o env2.o sky1.o sky2.o loading.o sky3.o $(RES)
LINKOBJ  = engine3d.o env1.o generate3d.o precalcs.o render3d.o bitfonts.o ZeDemo.o effects.o env3.o env2.o sky1.o sky2.o loading.o sky3.o $(RES)
LIBS =  -L"~/.local/dolcesdk/arm-dolce-eabi/lib" -s -static -lmikmod -lSDL -lpthread -lm -lgcov
INCS =  -I"~/.local/dolcesdk/arm-dolce-eabi/include"
CXXINCS =  -I"~/.local/dolcesdk/arm-dolce-eabi/include"
BIN  = ZeDemo.gpe
CXXFLAGS = $(CXXINCS)   -w -fexpensive-optimizations -O3
CFLAGS = $(INCS)   -w -fexpensive-optimizations -O3
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before ZeDemo.gpe all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LINKOBJ) -o "ZeDemo.gpe" $(LIBS)

engine3d.o: engine3d.c
	$(CC) -c engine3d.c -o engine3d.o $(CFLAGS)

env1.o: env1.c
	$(CC) -c env1.c -o env1.o $(CFLAGS)

generate3d.o: generate3d.c
	$(CC) -c generate3d.c -o generate3d.o $(CFLAGS)

precalcs.o: precalcs.c
	$(CC) -c precalcs.c -o precalcs.o $(CFLAGS)

render3d.o: render3d.c
	$(CC) -c render3d.c -o render3d.o $(CFLAGS)

bitfonts.o: bitfonts.c
	$(CC) -c bitfonts.c -o bitfonts.o $(CFLAGS)

ZeDemo.o: ZeDemo.c
	$(CC) -c ZeDemo.c -o ZeDemo.o $(CFLAGS)

effects.o: effects.c
	$(CC) -c effects.c -o effects.o $(CFLAGS)

env3.o: env3.c
	$(CC) -c env3.c -o env3.o $(CFLAGS)

env2.o: env2.c
	$(CC) -c env2.c -o env2.o $(CFLAGS)

sky1.o: sky1.c
	$(CC) -c sky1.c -o sky1.o $(CFLAGS)

sky2.o: sky2.c
	$(CC) -c sky2.c -o sky2.o $(CFLAGS)

loading.o: loading.c
	$(CC) -c loading.c -o loading.o $(CFLAGS)

sky3.o: sky3.c
	$(CC) -c sky3.c -o sky3.o $(CFLAGS)
