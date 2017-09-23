CXX=g++
CXXFLAGS=-I. -g -O0 -std=c++1z

OBJDIR=obj
LIBS=

$(OBJDIR)/%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

bitflipsim: $(OBJDIR)/Component.o $(OBJDIR)/HalfAdder.o $(OBJDIR)/FullAdder.o $(OBJDIR)/Wire.o $(OBJDIR)/System.o $(OBJDIR)/main.o
	$(CXX) -o bitflipsim $(OBJDIR)/Component.o $(OBJDIR)/HalfAdder.o $(OBJDIR)/FullAdder.o $(OBJDIR)/Wire.o $(OBJDIR)/System.o $(OBJDIR)/main.o $(LIBS)

debug: bitflipsim
	gdbgui ./bitflipsim

.PHONY: clean emscripten

emscripten:
	em++ -O2 -std=c++1z *.cpp -o html/bitflipsim.html

clean:
	rm -f bitflipsim
	rm -rf html
	rm -rf obj
	mkdir obj
	mkdir html
