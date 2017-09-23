CXX=g++
CXXFLAGS=-Ilib/yaml-cpp -g -O0 -std=c++1z

OBJDIR=obj
LDFLAGS=-Llib/yaml-cpp/build
LIBS=-lyaml-cpp

$(OBJDIR)/%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

bitflipsim: $(OBJDIR)/Component.o $(OBJDIR)/HalfAdder.o $(OBJDIR)/FullAdder.o $(OBJDIR)/Wire.o $(OBJDIR)/System.o $(OBJDIR)/main.o
	$(CXX) -o bitflipsim $(OBJDIR)/Component.o $(OBJDIR)/HalfAdder.o $(OBJDIR)/FullAdder.o $(OBJDIR)/Wire.o $(OBJDIR)/System.o $(OBJDIR)/main.o $(LDFLAGS) $(LIBS)

debug: bitflipsim
	gdbgui ./bitflipsim

.PHONY: clean emscripten init

init:
	cd lib/yaml-cpp; mkdir build; cd build; cmake ..; make

emscripten:
	em++ -O2 -std=c++1z *.cpp -o html/bitflipsim.html

clean:
	rm -f bitflipsim
	rm -rf html
	rm -rf obj
	mkdir obj
	mkdir html
