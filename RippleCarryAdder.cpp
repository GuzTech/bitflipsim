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
		auto fa_prev = full_adders[i - 1];
		auto fa_curr = full_adders[i];

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

void RippleCarryAdder::Connect(PORTS port, wire_t wire, size_t index) {
	if (index >= num_bits) {
		cout << "[Error] Index " << index << " out of bounds for "
			 << "RippleCarryAdder \"" << name << "\"\n";
		exit(1);
	}

	switch (port) {
	case PORTS::A:	  full_adders[index]->Connect(PORTS::A, wire); break;
	case PORTS::B:    full_adders[index]->Connect(PORTS::B, wire); break;
	case PORTS::S:    full_adders[index]->Connect(PORTS::S, wire); break;
	case PORTS::Cin:  full_adders[0]->Connect(PORTS::Cin, wire); break;
	case PORTS::Cout: full_adders[num_bits - 1]->Connect(PORTS::Cout, wire); break;
	default:
		cout << "[Error] Trying to connect to undefined port of RippleCarryAdder "
			 << "\"" << name << "\"n";
		exit(1);
	}
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

	for (auto &fa : full_adders) {
		auto wires = fa->GetWires();
		all_wires.insert(all_wires.end(),
						 wires.begin(),
						 wires.end());
	}

	return all_wires;
}

vector<wire_t> RippleCarryAdder::GetInputWires() {
	vector<wire_t> input_wires;

	for (auto &fa : full_adders) {
		auto wires = fa->GetInputWires();
		input_wires.insert(input_wires.end(),
						   wires.begin(),
						   wires.end());
	}

	return input_wires;
}

vector<wire_t> RippleCarryAdder::GetOutputWires() {
	vector<wire_t> output_wires;

	for (auto &fa : full_adders) {
		auto wires = fa->GetOutputWires();
		output_wires.insert(output_wires.end(),
							wires.begin(),
							wires.end());
	}

	return output_wires;
}
