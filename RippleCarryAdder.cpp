#include "main.h"

/*
  Ripple-carry adder implementation. Instantiates num_bits amount of
  Full-adders and connects them together.

  Schematic:

  Cin ---------
              |
          ---------
          |  Cin  |
  A0 -----|A     S|----- S0
          |   FA  |
  B0 -----|B     C|---
          |  Cout |  |
          ---------  |
                     |
              --------
              |
          ---------
          |  Cin  |
  A1 -----|A     S|----- S1
          |   FA  |
  B1 -----|B     C|---
          |  Cout |  |
          ---------  |
                     |
              --------
              |
          ---------
          |  Cin  |
  An -----|A     S|----- Sn
          |   FA  |
  Bn -----|B     C|----- Cout
          |  Cout |
          ---------
*/

RippleCarryAdder::RippleCarryAdder(string _name, size_t _num_bits)
	: Component(_name, _num_bits)
	, num_bits(_num_bits)
{
	// Reserve space.
	full_adders.reserve(num_bits);

	// Create the full-adders.
	for (size_t i = 0; i < num_bits; ++i) {
		string fa_name(_name);
		fa_name += "_fa_";
		fa_name += to_string(i);
		
		full_adders.push_back(make_shared<FullAdder>(fa_name));
	}

	// Create the wires between the carry ports of the full-adders.
	for (size_t i = 1; i < num_bits; ++i) {
		string wire_name(_name);
		wire_name += "_fa_wire_";
		wire_name += to_string(i - 1);
		wire_name += "_to_";
		wire_name += to_string(i);
		
		auto wire = make_shared<Wire>(wire_name);
		const auto &fa_prev = full_adders[i - 1];
		const auto &fa_curr = full_adders[i];

		fa_prev->Connect(PORTS::Cout, wire);
		fa_curr->Connect(PORTS::Cin, wire);
	}
}

void RippleCarryAdder::Update(bool propagating) {
	if (needs_update || !propagating) {
		for (size_t i = 0; i < longest_path; ++i) {
			for (auto &fa : full_adders) {
				fa->Update(propagating);
			}
		}

		needs_update = false;
	}
}

void RippleCarryAdder::Connect(PORTS port, const wire_t &wire, size_t index) {
	if (index >= num_bits) {
		cout << "[Error] Index " << index << " out of bounds for "
			 << "RippleCarryAdder \"" << name << "\"\n";
		exit(1);
	}

	switch (port) {
	case PORTS::A:	  full_adders[index]->Connect(PORTS::A, wire); break;
	case PORTS::B:    full_adders[index]->Connect(PORTS::B, wire); break;
	case PORTS::O:    full_adders[index]->Connect(PORTS::O, wire); break;
	case PORTS::Cin:  full_adders[0]->Connect(PORTS::Cin, wire); break;
	case PORTS::Cout: full_adders[num_bits - 1]->Connect(PORTS::Cout, wire); break;
	default:
		cout << "[Error] Trying to connect to undefined port of RippleCarryAdder "
			 << "\"" << name << "\"\n";
		exit(1);
	}
}

void RippleCarryAdder::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		cout << "[Error] Wire bundle \"" << wires->GetName()
			 << " accessed with index " << wire_idx
			 << " but has size " << wires->GetSize() << '\n';
		exit(1);
	}

	const wire_t &wire = (*wires.get())[wire_idx];
	Connect(port, wire, port_idx);
}

size_t RippleCarryAdder::GetNumToggles() {
	toggle_count = 0;

	for (auto &fa : full_adders) {
		toggle_count += fa->GetNumToggles();
	}

	return toggle_count;
}

vector<wire_t> RippleCarryAdder::GetWires() {
	vector<wire_t> all_wires;

	// Add the A and B inputs of all the full adders.
	const auto &inputs = GetInputWires();
	const auto &outputs = GetOutputWires();

	all_wires.insert(all_wires.end(),
					 inputs.begin(),
					 inputs.end());
	all_wires.insert(all_wires.end(),
					 outputs.begin(),
					 outputs.end());

	// Finally add the internal wires.
	for (size_t i = 0; i < num_bits - 1; ++i) {
		const auto &wire = full_adders[i]->GetWire(PORTS::Cout);
		all_wires.push_back(wire);
	}

	return all_wires;
}

vector<wire_t> RippleCarryAdder::GetInputWires() {
	vector<wire_t> input_wires;

	// Get the A and B inputs of all the full adders.
	for (size_t i = 0; i < num_bits; ++i) {
		const auto &wire_A = full_adders[i]->GetWire(PORTS::A);
		const auto &wire_B = full_adders[i]->GetWire(PORTS::B);

		input_wires.push_back(wire_A);
		input_wires.push_back(wire_B);
	}

	// Get the carry in input from the first full adder.
	input_wires.push_back(full_adders[0]->GetWire(PORTS::Cin));

	return input_wires;
}

vector<wire_t> RippleCarryAdder::GetOutputWires() {
	vector<wire_t> output_wires;

	// Get the sum outputs of all the full adders.
	for (size_t i = 0; i < num_bits; ++i) {
		const auto &wire = full_adders[i]->GetWire(PORTS::O);
		output_wires.push_back(wire);
	}

	// Get the carry out output of the last full adder.
	output_wires.push_back(full_adders[num_bits - 1]->GetWire(PORTS::Cout));

	return output_wires;
}

wire_t RippleCarryAdder::GetWire(PORTS port, size_t index) {
	if (index >= num_bits) {
		cout << "[Error] Index " << index << " out of bound for "
			 << "RippleCarryAdder \"" << name << "\"\n";
		exit(1);
	}

	switch (port) {
	case PORTS::A:
	case PORTS::B:
	case PORTS::O:
	case PORTS::Cin:
	case PORTS::Cout:
		return full_adders[index]->GetWire(port);
	default:
		cout << "[Error] Trying to retrieve undefined port of RippleCarryAdder "
			 << "\"" << name << "\"\n";
		exit(1);
	}
}
