OBJ = ana
sourcefile = main.cc UserDefine.hh

ROOTCFLAGS  = $(shell root-config --cflags)
ROOTLIBS    = $(shell root-config --libs) -lSpectrum -lMathMore -lGenVector

CFLAGS = -g -Wall -O2 

GXX = g++ 

all:$(OBJ)

$(OBJ): $(sourcefile)
	$(GXX) $(CFLAGS) $(ROOTCFLAGS) -o $@ $(filter %.cc ,$(sourcefile)) $(ROOTLIBS)
clean:
	rm -f *~ *.o $(OBJ) *Dict*
