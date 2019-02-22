PRGNAME     = potator

# define regarding OS, which compiler to use
EXESUFFIX = 
TOOLCHAIN = 
CC          = gcc
CCP         = g++
LD          = gcc

# add SDL dependencies
SDL_LIB     = 
SDL_INCLUDE = 

#Defines available 
#-DNO_SOUND    : No sound
#-DNO_MUSIC    : Sound effects but no music
#-DDISPLAY_FPS : Framerate counter
#-DNO_OPENGL   : No OpenGL (seems to be buggy right now...)


F_OPTS		= -I/usr/include/SDL -DNO_OPENGL
CC_OPTS		= -O0 -g3 -fomit-frame-pointer -fdata-sections -ffunction-sections $(F_OPTS)
CFLAGS		= -I$(SDL_INCLUDE) $(CC_OPTS)
CXXFLAGS	=$(CFLAGS) 
LDFLAGS     = -lSDLmain -lSDL -lSDL_image -lSDL_mixer -lm -Wl,--as-needed -Wl,--gc-sections -flto -lstdc++ -lGL

# Files to be compiled
SRCDIR    = ./game ./interf ./sound ./video ./ ./util
VPATH     = $(SRCDIR)
SRC_C   = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
SRC_CP   = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.cpp))
OBJ_C   = $(notdir $(patsubst %.c, %.o, $(SRC_C)))
OBJ_CP   = $(notdir $(patsubst %.cpp, %.o, $(SRC_CP)))
OBJS     = $(OBJ_C) $(OBJ_CP)

# Rules to make executable
$(PRGNAME)$(EXESUFFIX): $(OBJS)  
	$(LD) $(CFLAGS) -o $(PRGNAME)$(EXESUFFIX) $^ $(LDFLAGS)

$(OBJ_C) : %.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_CP) : %.o : %.cpp
	$(CCP) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(PRGNAME)$(EXESUFFIX) *.o
