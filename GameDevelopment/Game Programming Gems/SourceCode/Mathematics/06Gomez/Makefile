TARGET = wave

OBJFILES = body.o coordinate_frame.o main.o 

LOADLIBES = -lGL -lglut -lGLU -L/usr/X11R6/lib -lX11 \
	-lXi -lXmu

$(TARGET): $(OBJFILES)
	$(CXX) -o $@ $(OBJFILES) $(LOADLIBES)

