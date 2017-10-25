#include "main.h"

WireBundle::WireBundle(string _name, size_t _size)
	: name(_name)
	, size(_size) {
	wires.reserve(size);
}

void WireBundle::Init() {
	for (size_t i = 0; i < size; ++i) {
		wires.push_back(
			make_shared<Wire>(
				name + "[" + to_string(i) + "]",
				shared_from_this()));
	}
}

void WireBundle::SetValue(int64_t value, bool propagating) {
	for (int i = size - 1; i >= 0; --i) {
		if (value & (1 << i)) {
			wires[i]->SetValue(true, propagating);
		}
	}
}
