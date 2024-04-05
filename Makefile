CC := g++
SANITIZER := #-fsanitize=memory -fsanitize-memory-track-origins
INCLUDE_DIRS := -Ilib/yaml-cpp/include -Ilib/ctemplate/src -Ilib/ordered-map
CFLAGS := $(INCLUDE_DIRS) -O3 -std=c++1z -Werror $(SANITIZER)
LIBS := -lyaml-cpp -static -lctemplate_nothreads -lstdc++fs
LDFLAGS := -Llib/yaml-cpp/build -Llib/ctemplate/.libs $(LIBS) $(SANITIZER)
OBJDIR := obj
OBJS := $(addprefix $(OBJDIR)/, Utils.o Component.o FullAdder.o HalfAdder.o RippleCarryAdder.o RippleCarryAdderSubtracter.o RippleCarrySubtracter.o CarrySaveAdder.o Multiplier_2C.o Multiplier_Smag.o BoothEncoderRadix4.o Radix4BoothDecoder.o Multiplier_2C_Booth.o SmagTo2C.o And.o And3.o Or.o Or3.o Xor.o Nand.o Nor.o Nor3.o Xnor.o Not.o Mux.o WireBundle.o Wire.o System.o main.o)
EXECUTABLE := bitflipsim

all: $(OBJS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

$(OBJDIR)/%.o : src/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

debug: $(EXECUTABLE)
	gdbgui ./$(EXECUTABLE)

lib/yaml-cpp/build/libyaml-cpp.a:
	cd lib/yaml-cpp; mkdir build; cd build; cmake ..; make

lib/ctemplate/.libs/libctemplate_nothreads.a:
	cd lib/ctemplate; ./autogen.sh; ./configure; make

init: lib/yaml-cpp/build/libyaml-cpp.a lib/ctemplate/.libs/libctemplate_nothreads.a $(OBJDIR)

.PHONY: clean

clean:
	rm -f $(EXECUTABLE)
	rm -rf obj
