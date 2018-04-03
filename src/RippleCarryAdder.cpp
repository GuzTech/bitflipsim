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
  B0 -----|B      |
          |  Cout |
          ---------
              |
              |
              |
          ---------
          |  Cin  |
  A1 -----|A     S|----- S1
          |   FA  |
  B1 -----|B      |
          |  Cout |
          ---------
              |
             ...
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

		const auto fa = make_shared<FullAdder>(fa_name);
		full_adders.emplace_back(fa);

		for (const auto &w : fa->GetInternalWires()) {
			internal_wires.emplace_back(w);
		}
	}

	// Create the wires between the carry ports of the full-adders.
	for (size_t i = 1; i < num_bits; ++i) {
		string wire_name(_name);
		wire_name += "_fa_cout_";
		wire_name += to_string(i - 1);
		wire_name += "_to_";
		wire_name += to_string(i);
		
		auto wire = make_shared<Wire>(wire_name);
		const auto &fa_prev = full_adders[i - 1];
		const auto &fa_curr = full_adders[i];

		fa_prev->Connect(PORTS::Cout, wire);
		fa_curr->Connect(PORTS::Cin, wire);

		internal_wires.emplace_back(wire);
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
		Error("Index " + to_string(index) + " out of bounds for RippleCarryAdder \"" + name + "\".\n");
	}

	switch (port) {
	case PORTS::A:
	case PORTS::B:
	case PORTS::Cin:
		full_adders[index]->Connect(port, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::O:
	case PORTS::Cout:
		full_adders[index]->Connect(port, wire);
		output_wires.emplace_back(wire);
		break;
	default:
		Error("Trying to connect to undefined port of RippleCarryAdder \"" + name + "\".\n");
	}
}

void RippleCarryAdder::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		Error("Wire bundle \"" + wires->GetName() + " accessed with index " + to_string(wire_idx)
			  + " but has size " + to_string(wires->GetSize()) + ".\n");
	}

	const wire_t &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const wire_t RippleCarryAdder::GetWire(PORTS port, size_t index) const {
	if (index >= num_bits) {
		Error("Index " + to_string(index) + " out of bound for RippleCarryAdder \"" + name + "\".\n");
	}

	switch (port) {
	case PORTS::A:
	case PORTS::B:
	case PORTS::O:
		return full_adders[index]->GetWire(port);
	case PORTS::Cin:
		return full_adders.front()->GetWire(port);
	case PORTS::Cout:
		return full_adders.back()->GetWire(port);
	default:
		Error("Trying to retrieve undefined port of RippleCarryAdder \"" + name + "\".\n");
	}
}

const PORT_DIR RippleCarryAdder::GetPortDirection(PORTS port) const {
	switch (port) {
	case PORTS::A:
	case PORTS::B:
	case PORTS::Cin:
		return PORT_DIR::INPUT;
	case PORTS::Cout:
	case PORTS::O:
		return PORT_DIR::OUTPUT;
	default:
		Error("Trying to get port direction of undefined port in RippleCarryAdder \"" + name + "\".\n");
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

void RippleCarryAdder::GenerateVHDLEntity(const string &path) const {
	// We only need to do it once, since all instances of the RippleCarryAdder are identical.
	if (!entityGenerated) {
		string output;
		TemplateDictionary entity("RippleCarryAdder");
		ExpandTemplate("src/templates/VHDL/RippleCarryAdder_entity.tpl", DO_NOT_STRIP, &entity, &output);

		auto outfile = ofstream(path + "/RippleCarryAdder.vhd");
		outfile << output;
		outfile.close();

		full_adders.front()->GenerateVHDLEntity(path);

		entityGenerated = true;
	}
}

const string RippleCarryAdder::GenerateVHDLInstance() const {
	string output;
	TemplateDictionary inst("RippleCarryAdder");

	GenerateAssignments(PORTS::A, num_bits, "A", inst);
	GenerateAssignments(PORTS::B, num_bits, "B", inst);
	GenerateAssignments(PORTS::Cin, 1, "Cin", inst);
	GenerateAssignments(PORTS::O, num_bits, "O", inst);
	GenerateAssignments(PORTS::Cout, 1, "Cout", inst, true);

	inst.SetValue("NAME", name);
	inst.SetValue("SIZE", to_string(num_bits));
	ExpandTemplate("src/templates/VHDL/RippleCarryAdder_inst.tpl", DO_NOT_STRIP, &inst, &output);

	return output;
}
