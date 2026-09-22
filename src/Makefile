BUILD_PRX = 1
TARGET = raylib
OBJS = main.o physics.o graphics.o random.o
INCDIR =
CFLAGS = -Wall -O3 -g
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS= -lraylib -lpng -lz  -lglut -lGLU -lGL -lpspvfpu -lpspgu -lpspfpu

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Watermelon Game

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
