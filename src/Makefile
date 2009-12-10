CPP  = g++
OBJ  = main.o wavegeneration.o notes.o keys.o graphics.o general.o tune.o score.o
LINKOBJ  = $(OBJ)
GL = GL
LINK_PATHS = 
LIBS =  -L$(LINK_PATHS) -lSDLmain -lSDL -lSDL_mixer -l$(GL) -lGLU -s
INCS =  -I/usr/include -I/usr/include/SDL
BIN  = shero
CXXFLAGS = $(INCS)   
RM = rm -f
CP = cp

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after


clean: clean-custom
	$(RM) $(OBJ) $(BIN)
	$(RM) ../$(BIN) 

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS)
	cp $(BIN) ..

main.o: main.cpp
	$(CPP) -c main.cpp -o main.o $(CXXFLAGS)

wavegeneration.o: wavegeneration.cpp
	$(CPP) -c wavegeneration.cpp -o wavegeneration.o $(CXXFLAGS)

notes.o: notes.cpp
	$(CPP) -c notes.cpp -o notes.o $(CXXFLAGS)

keys.o: keys.cpp
	$(CPP) -c keys.cpp -o keys.o $(CXXFLAGS)

graphics.o: graphics.cpp
	$(CPP) -c graphics.cpp -o graphics.o $(CXXFLAGS)

general.o: general.cpp
	$(CPP) -c general.cpp -o general.o $(CXXFLAGS)

tune.o: tune.cpp
	$(CPP) -c tune.cpp -o tune.o $(CXXFLAGS)

score.o: score.cpp
	$(CPP) -c score.cpp -o score.o $(CXXFLAGS)
