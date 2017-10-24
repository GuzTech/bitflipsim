#include "main.h"

WireBundle::WireBundle(string _name, size_t _size)
	: name(_name)
	, size(_size) {
	wires.reserve(size);

	for (size_t i = 0; i < size; ++i) {
		wires.push_back(make_shared<Wire>(name + "_" + to_string(i), make_shared<WireBundle>(*this)));
	}
}
