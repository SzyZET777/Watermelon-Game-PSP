BUILD_PRX = 1
TARGET = raylib
OBJS = main.o physics.o graphics.o random.o
INCDIR =
CFLAGS = -Wall -O3 -g
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS= -lraylib \
       -lSDL2_mixer -lmodplug -lvorbisfile -lvorbis -logg -lSDL2 \
       -lpng -lz -lglut -lGLU -lGL \
       -lpspvfpu -lpspgu -lpspfpu \
       -lpspaudio -lpsphprm -lpspctrl -lpsppower -lpsprtc -lpspvram \
       -lstdc++ -lm

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Watermelon Game

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
