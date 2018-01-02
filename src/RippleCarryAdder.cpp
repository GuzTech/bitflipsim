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

bool RippleCarryAdder::entityGenerated = false;

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

		if (!propagating && print_debug) {
			PrintDebug();
		}

		needs_update = false;
	}
}

void RippleCarryAdder::Connect(PORTS port, const wire_t &wire, size_t index) {
	if (index >= num_bits) {
		cout << "[Error] Index " << index << " out of bounds for "
			 << "RippleCarryAdder \"" << name << "\".\n";
		exit(1);
	}

	switch (port) {
	case PORTS::A:
		full_adders[index]->Connect(PORTS::A, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::B:
		full_adders[index]->Connect(PORTS::B, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::Cin:
		full_adders.front()->Connect(PORTS::Cin, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::O:
		full_adders[index]->Connect(PORTS::O, wire);
		output_wires.emplace_back(wire);
		break;
	case PORTS::Cout:
		full_adders.back()->Connect(PORTS::Cout, wire);
		output_wires.emplace_back(wire);
		break;
	default:
		cout << "[Error] Trying to connect to undefined port of RippleCarryAdder "
			 << "\"" << name << "\".\n";
		exit(1);
	}
}

void RippleCarryAdder::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		cout << "[Error] Wire bundle \"" << wires->GetName()
			 << " accessed with index " << wire_idx
			 << " but has size " << wires->GetSize() << ".\n";
		exit(1);
	}

	const wire_t &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const wire_t RippleCarryAdder::GetWire(PORTS port, size_t index) const {
	if (index >= num_bits) {
		cout << "[Error] Index " << index << " out of bound for "
			 << "RippleCarryAdder \"" << name << "\".\n";
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
			 << "\"" << name << "\".\n";
		exit(1);
	}
}

void RippleCarryAdder::PrintDebug() const {
	cout << "\n========================================\n";
	cout << name << ':';

	for (const auto &fa : full_adders) {
		const auto &A = fa->GetWire(PORTS::A);
		const auto &B = fa->GetWire(PORTS::B);
		const auto &Cin = fa->GetWire(PORTS::Cin);
		const auto &Cout = fa->GetWire(PORTS::Cout);
		const auto &O = fa->GetWire(PORTS::O);

		cout << '\n' << fa->GetName() << '\n';
		if (A)    cout << "A (" << A->GetName() << "): " << (*A)() << '\n';
		if (B)    cout << "B (" << B->GetName() << "): " << (*B)() << '\n';
		if (Cin)  cout << "Cin (" << Cin->GetName() << "): " << (*Cin)() << '\n';
		if (O)    cout << "O (" << O->GetName() << "): " << (*O)() << '\n';
		if (Cout) cout << "Cout (" << Cout->GetName() << "): " << (*Cout)() << '\n';
	}

	cout << "========================================\n";
}

void RippleCarryAdder::GenerateVHDLEntity() const {
	// We only need to do it once, since all instances of the RippleCarryAdder are identical.
	if (!entityGenerated) {
		string output;
		TemplateDictionary entity("RippleCarryAdder");
		ExpandTemplate("src/templates/VHDL/RippleCarryAdder_entity.tpl", DO_NOT_STRIP, &entity, &output);
		cout << output;

		entityGenerated = true;
	}
}

void RippleCarryAdder::GenerateVHDLInstance() {
	string output;
	TemplateDictionary inst("RippleCarryAdder");
	inst.SetValue("NAME", name);
	inst.SetValue("SIZE", to_string(num_bits));
	ExpandTemplate("src/templates/VHDL/RippleCarryAdder_inst.tpl", DO_NOT_STRIP, &inst, &output);
	cout << output;
}
