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
	: Component(_name)
	, num_bits(_num_bits)
{
	// Reserve space.
	full_adders.reserve(num_bits);
	An.reserve(num_bits);
	Bn.reserve(num_bits);
	Sn.reserve(num_bits);

	// Create the full-adders.
	for (auto i = 0; i < num_bits; ++i) {
		std::string fa_name(_name);
		fa_name += "_fa_";
		fa_name += i;
		
		full_adders.push_back(std::make_shared<FullAdder>(fa_name));
	}

	// Create the wires between the carry ports of the full-adders.
	for (auto i = 1; i < num_bits; ++i) {
		std::string wire_name(_name);
		wire_name += "_fa_wire_";
		wire_name += i - 1;
		wire_name += "_to_";
		wire_name += i;
		
		auto wire = std::make_shared<Wire>(wire_name);
		auto fa_prev = full_adders[i - 1];
		auto fa_curr = full_adders[i];

		fa_prev->Connect(FullAdder::PORTS::Cout, wire);
		fa_curr->Connect(FullAdder::PORTS::Cin, wire);
	}
}

void RippleCarryAdder::Update(bool propagating) {
	if (needs_update || !propagating) {
		for (auto &fa : full_adders) {
			fa->Update(propagating);
		}

		needs_update = false;
	}
}

void RippleCarryAdder::Connect(PORTS port, wire_t wire, std::size_t index) {
	if (index >= num_bits) {
		std::cout << "[Error] Index " << index << " out of bound for "
				  << "ripple-carry adder \"" << name << "\"\n";
		exit(1);
	}

	switch (port) {
	case PORTS::An:	  An[index] = wire; wire->AddOutput(this->shared_from_base<RippleCarryAdder>()); break;
	case PORTS::Bn:   Bn[index] = wire; wire->AddOutput(this->shared_from_base<RippleCarryAdder>()); break;
	case PORTS::Sn:   Sn[index] = wire; wire->SetInput(this->shared_from_base<RippleCarryAdder>()); break;
    case PORTS::Cin:  Cin       = wire; wire->AddOutput(this->shared_from_base<RippleCarryAdder>()); break;
	case PORTS::Cout: Cout      = wire; wire->SetInput(this->shared_from_base<RippleCarryAdder>()); break;
	}
}

std::vector<wire_t> RippleCarryAdder::GetWires() {
	std::vector<wire_t> all_wires;

	all_wires.insert(all_wires.end(), An.begin(), An.end());
	all_wires.insert(all_wires.end(), Bn.begin(), Bn.end());
	all_wires.insert(all_wires.end(), Sn.begin(), Sn.end());
	all_wires.push_back(Cin);
	all_wires.push_back(Cout);

	return all_wires;
}

std::vector<wire_t> RippleCarryAdder::GetInputWires() {
	std::vector<wire_t> input_wires;

	input_wires.insert(input_wires.end(), An.begin(), An.end());
	input_wires.insert(input_wires.end(), Bn.begin(), Bn.end());
	input_wires.push_back(Cin);

	return input_wires;
}

std::vector<wire_t> RippleCarryAdder::GetOutputWires() {
	std::vector<wire_t> output_wires;

	output_wires.insert(output_wires.end(), Sn.begin(), Sn.end());
	output_wires.push_back(Cout);

	return output_wires;
}
