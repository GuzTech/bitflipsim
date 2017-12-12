#include "main.h"

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
	, num_bits_O(num_bits_A + num_bits_B)
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
	if (port == PORTS::A && index >= num_bits_A) {
		cout << "[Error] Index " << index << " of port A is out of "
			 << "bounds for Multiplier_2C_Booth \"" << name <<"\"\n";
		exit(1);
	} else if (port == PORTS::B && index >= num_bits_B) {
		cout << "[Error] Index " << index << " of port B is out of "
			 << "bounds for Multiplier_2C_Booth \"" << name <<"\"\n";
		exit(1);
	} else if (port == PORTS::O && index >= num_bits_O) {
		cout << "[Error] Index " << index << " of port O is out of "
			 << "bounds for Multiplier_2C_Booth \"" << name <<"\"\n";
		exit(1);
	}

	auto error_undefined_port = [&](const auto &wire) {
		cout << "[Error] Trying to connect wire \"" << wire->GetName()
			 << "\" to undefined port of Multiplier_2C_Booth "
			 << "\"" << name << "\"\n";
		exit(1);
	};

	switch (port) {
	case PORTS::A:
		break;
	case PORTS::B:
		break;
	case PORTS::O:
		break;
	default:
		error_undefined_port(wire);
	}
}

void Multiplier_2C_Booth::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		cout << "[Error] Wire bundle \"" << wires->GetName()
			 << " accessed with index " << wire_idx
			 << " but has size " << wires->GetSize() << '\n';
		exit(1);
	}

	const wire_t &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const size_t Multiplier_2C_Booth::GetNumToggles() {
	toggle_count = 0;

	return toggle_count;
}

const vector<wire_t> Multiplier_2C_Booth::GetWires() const {
	vector<wire_t> wires;

	// Add all input wires.
	const vector<wire_t> &input = GetInputWires();
	wires.insert(wires.end(),
				 input.begin(),
				 input.end());

	// Add all internal wires.
	wires.insert(wires.end(),
				 internal_wires.begin(),
				 internal_wires.end());

	// Add all output wires.
	const vector<wire_t> &output = GetOutputWires();
	wires.insert(wires.end(),
				 output.begin(),
				 output.end());

	return wires;
}

const vector<wire_t> Multiplier_2C_Booth::GetInputWires() const {
	vector<wire_t> input_wires;

	return input_wires;
}

const wire_t Multiplier_2C_Booth::GetWire(PORTS port, size_t index) const {
	return nullptr;
}

void Multiplier_2C_Booth::Generate2CBoothHardware() {

}
