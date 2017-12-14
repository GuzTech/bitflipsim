#include "main.h"
#include <cassert>

/*
  Twos-complement Radix-4 Booth multiplier based on
  "High-Speed Booth Encoded Parallel Multiplier Design" by Yeh and Jen.
*/

Multiplier_2C_Booth::Multiplier_2C_Booth(string _name,
										 size_t _num_bits_A,
										 size_t _num_bits_B)
	: Component(_name)
	, num_bits_A(_num_bits_A)
	, num_bits_B(_num_bits_B)
	, num_bits_O(_num_bits_A + _num_bits_B)
	, num_encoders((_num_bits_B + 1) / 2)
	, num_decoders_per_row(_num_bits_A - 1)
{
	if (num_bits_A == 0) {
		cout << "[Error] Size of port A of Multiplier_2C_Booth \"" << _name
			 << "\" must be larger than zero.\n";
		exit(1);
	} else if (num_bits_B == 0) {
		cout << "[Error] Size of port B of Multiplier_2C_Booth \"" << _name
			 << "\" must be larger than zero.\n";
		exit(1);
	}

	Generate2CBoothHardware();
}

void Multiplier_2C_Booth::Update(bool propagating) {
	if (needs_update || !propagating) {

		needs_update = false;
	}
}

void Multiplier_2C_Booth::Connect(PORTS port, const wire_t &wire, size_t index) {
	CheckIfIndexIsInRange(port, index);

	auto error_undefined_port = [&](const auto &wire) {
		cout << "[Error] Trying to connect wire \"" << wire->GetName()
			 << "\" to undefined port of Multiplier_2C_Booth "
			 << "\"" << name << "\"\n";
		exit(1);
	};

	switch (port) {
	case PORTS::A:
		input_wires.emplace_back(wire);
		break;
	case PORTS::B:
		input_wires.emplace_back(wire);
		break;
	case PORTS::O:
		output_wires.emplace_back(wire);
		break;
	default:
		error_undefined_port(wire);
	}
}

void Multiplier_2C_Booth::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		cout << "[Error] Wire bundle \"" << wires->GetName()
			 << " accessed with index " << wire_idx
			 << " but has size " << wires->GetSize() << ".\n";
		exit(1);
	}

	const auto &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const wire_t Multiplier_2C_Booth::GetWire(PORTS port, size_t index) const {
	CheckIfIndexIsInRange(port, index);

	

	return nullptr;
}

void Multiplier_2C_Booth::Generate2CBoothHardware() {
	// Generate the encoders.
	string name_prefix = name + "_enc_";
	for (size_t i = 0; i < num_encoders; ++i) {
		encoders.emplace_back(make_shared<BoothEncoderRadix4>(name_prefix + to_string(i)));
	}

	// Generate the decoders.
	name_prefix = name + "_dec_";
	for (size_t i = 0; i < num_encoders; ++i) {
		decoders.emplace_back(
			make_shared<Radix4BoothDecoder>(
				name_prefix + to_string(i), num_decoders_per_row));
	}

	// Generate the Carry-Save adders. First adder is slightly larger
	// due to more sign-extension bits.
	name_prefix = name + "_csa_";
	cs_adders.emplace_back(
		make_shared<CarrySaveAdder>(
			name_prefix + '0', num_bits_A + 4));

	for (size_t i = 1; i < (num_encoders - 1); ++i) {
		cs_adders.emplace_back(
			make_shared<CarrySaveAdder>(
				name_prefix + to_string(i), num_bits_A + 3));
	}

	assert(encoders.size() == decoders.size());
}

void Multiplier_2C_Booth::CheckIfIndexIsInRange(PORTS port, size_t index) const {
	if (port == PORTS::A && index >= num_bits_A) {
		cout << "[Error] Index " << index << " of port A is out of "
			 << "bounds for Multiplier_2C_Booth \"" << name << "\".\n";
		exit(1);
	} else if (port == PORTS::B && index >= num_bits_B) {
		cout << "[Error] Index " << index << " of port B is out of "
			 << "bounds for Multiplier_2C_Booth \"" << name << "\".\n";
		exit(1);
	} else if (port == PORTS::O && index >= num_bits_O) {
		cout << "[Error] Index " << index << " of port O is out of "
			 << "bounds for Multiplier_2C_Booth \"" << name << "\".\n";
		exit(1);
	}
}
