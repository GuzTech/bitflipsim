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

RippleCarryAdder::RippleCarryAdder(std::string _name, std::size_t _num_bits)
	: Component(_name, _num_bits)
	, num_bits(_num_bits)
{
	// Reserve space.
	full_adders.reserve(num_bits);

	// Create the full-adders.
	for (std::size_t i = 0; i < num_bits; ++i) {
		std::string fa_name(_name);
		fa_name += "_fa_";
		fa_name += std::to_string(i);
		
		full_adders.push_back(std::make_shared<FullAdder>(fa_name));
	}

	// Create the wires between the carry ports of the full-adders.
	for (std::size_t i = 1; i < num_bits; ++i) {
		std::string wire_name(_name);
		wire_name += "_fa_wire_";
		wire_name += std::to_string(i - 1);
		wire_name += "_to_";
		wire_name += std::to_string(i);
		
		auto wire = std::make_shared<Wire>(wire_name);
		auto fa_prev = full_adders[i - 1];
		auto fa_curr = full_adders[i];

		fa_prev->Connect(PORTS::Cout, wire);
		fa_curr->Connect(PORTS::Cin, wire);
	}
}

void RippleCarryAdder::Update(bool propagating) {
	if (needs_update || !propagating) {
		for (std::size_t i = 0; i < longest_path; ++i) {
			for (auto &fa : full_adders) {
				fa->Update(propagating);
			}
		}

		needs_update = false;
	}
}

void RippleCarryAdder::Connect(PORTS port, wire_t wire, std::size_t index) {
	if (index >= num_bits) {
		std::cout << "[Error] Index " << index << " out of bounds for "
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
		std::cout << "[Error] Trying to connect to undefined port of RippleCarryAdder "
				  << "\"" << name << "\"n";
		exit(1);
	}
}

std::size_t RippleCarryAdder::GetNumToggles() {
	toggle_count = 0;
	for (auto &fa : full_adders) {
		toggle_count += fa->GetNumToggles();
	}

	return toggle_count;
}

std::vector<wire_t> RippleCarryAdder::GetWires() {
	std::vector<wire_t> all_wires;

	for (auto &fa : full_adders) {
		auto wires = fa->GetWires();
		all_wires.insert(all_wires.end(),
						 wires.begin(),
						 wires.end());
	}

	return all_wires;
}

std::vector<wire_t> RippleCarryAdder::GetInputWires() {
	std::vector<wire_t> input_wires;

	for (auto &fa : full_adders) {
		auto wires = fa->GetInputWires();
		input_wires.insert(input_wires.end(),
						   wires.begin(),
						   wires.end());
	}

	return input_wires;
}

std::vector<wire_t> RippleCarryAdder::GetOutputWires() {
	std::vector<wire_t> output_wires;

	for (auto &fa : full_adders) {
		auto wires = fa->GetOutputWires();
		output_wires.insert(output_wires.end(),
							wires.begin(),
							wires.end());
	}

	return output_wires;
}
