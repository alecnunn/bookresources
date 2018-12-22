TARGET = SimpleFlocking

OBJFILES = CBoid.o CBox.o CFlock.o custom_time.o main.o mtxlib.o \
	profile.o text.o vector.o

LOADLIBES = -lGL -lglut -lGLU -L/usr/X11R6/lib -lX11 \
	-lXi -lXmu

$(TARGET): $(OBJFILES)
	$(CXX) -o $@ $(OBJFILES) $(LOADLIBES)

