CC := g++
INCLUDE_DIRS := lib/yaml-cpp/include
CFLAGS := -I$(INCLUDE_DIRS) -g -O0 -std=c++1z
LIBS := -lyaml-cpp
LDFLAGS := -Llib/yaml-cpp/build $(LIBS)
OBJDIR := obj
OBJS := $(addprefix $(OBJDIR)/, Component.o FullAdder.o HalfAdder.o RippleCarryAdder.o And.o Or.o Xor.o Nand.o Nor.o Xnor.o Not.o Mux.o Wire.o System.o main.o)
EXECUTABLE := bitflipsim

all: $(OBJS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

$(OBJDIR)/%.o : %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

debug: $(EXECUTABLE)
	gdbgui ./$(EXECUTABLE)

lib/yaml-cpp/build/libyaml-cpp.a:
	cd lib/yaml-cpp; mkdir build; cd build; cmake ..; make

init: lib/yaml-cpp/build/libyaml-cpp.a $(OBJDIR)

.PHONY: clean

clean:
	rm -f $(EXECUTABLE)
	rm -rf obj
