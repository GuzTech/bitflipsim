#include "main.h"

WireBundle::WireBundle(string _name, size_t _size)
	: name(_name)
	, size(_size) {
	wires.reserve(size);
}

const int64_t WireBundle::GetValue() {
	int64_t result = 0;

	for (size_t i = 0; i < size; ++i) {
		const size_t sv = size - i - 1;
		bool val = wires[sv]->GetValue();

		if (val) {
			result |= (1ul << sv);
		}
	}

	return result;
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
		bool bit_val = false;
		if (value & (1 << i)) {
			bit_val = true;
		}

		wires[i]->SetValue(bit_val, propagating);
	}
}
