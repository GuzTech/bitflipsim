#include "main.h"

WireBundle::WireBundle(string _name, size_t _size, REPR _repr)
	: name(_name)
	, size(_size)
	, repr(_repr) {
	wires.reserve(size);
}

const int64_t WireBundle::GetValue() const {
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

const int64_t WireBundle::Get2CValue() const {
	int64_t result = GetValue();

	switch(repr) {
	case REPR::TWOS_COMPLEMENT: {
		// Modify the result if the MSB is a 1.
		if (wires.back()->GetValue()) {
			result = (-1 & ~((1l << (size - 1)) - 1)) | result;
		}
		break;
	}
	case REPR::ONES_COMPLEMENT: {
		// Modify the result if the MSB is a 1.
		if (wires.back()->GetValue()) {
			result = (-1 & ~((1l << (size - 1)) - 1)) | result;
			result += 1;
		}
		break;
	}
	case REPR::SIGNED_MAGNITUDE: {
		// Modify the result if the MSB is a 1.
		if (wires.back()->GetValue()) {
			result &= ((1l << (size - 1)) - 1);
			result = -result;
		}
		break;
	}
	}

	return result;
}

const optional<size_t> WireBundle::GetWireIndex(const wire_t &wire) const {
	for (size_t i = 0; i < size; ++i) {
		if (wires[i] == wire) {
			return i;
		}
	}

	return nullopt;
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
	switch(repr) {
	case REPR::TWOS_COMPLEMENT: break;
	case REPR::ONES_COMPLEMENT: {
		if (value < 0) {
			value = ~(-value);
		}
		break;
	}
	case REPR::SIGNED_MAGNITUDE: {
		if (value < 0) {
			value = (-value) | (1ull << (size - 1));
		}
		break;
	}
	}

	for (int64_t i = size - 1; i >= 0; --i) {
		bool bit_val = false;
		if (value & (1ull << i)) {
			bit_val = true;
		}

		wires[i]->SetValue(bit_val, propagating);
	}
}
