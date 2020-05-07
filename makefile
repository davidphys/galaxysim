PROJECT=galaxysim #executable file name

CC=g++
CFLAGS= -Wall -std=c++11
DEBUGFLAGS= -g
RELEASEFLAGS= -O3 -fopenmp
PROJECTOPS= -disk 100000 -nout 100 -ksteps 5 -imgdir out
LIBS =

#directories:
INCLUDE=include
OBJDIR=obj

#needed source files
SRC=$(wildcard src/*.cpp)


OBJSR=$(patsubst src/%.cpp,obj/%.o,$(SRC))

all: $(PROJECT) 
$(PROJECT): $(OBJSR) 
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -I $(INCLUDE) -o $(PROJECT) $^ $(LIBS)

$(OBJSR): obj/%.o : src/%.cpp
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -I $(INCLUDE) -c $< $(LIBS) -o $@

run: $(PROJECT)
	-mkdir out
	./$(PROJECT) $(PROJECTOPS)



clean:
	rm -f obj/*.o $(PROJECT)
