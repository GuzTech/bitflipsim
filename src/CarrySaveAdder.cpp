#include "main.h"

/*
  Carry-save adder implementation.
  Instantiates num_bits amount of Full-adders.

  Schematic:

  Cin0 --------
              |
          ---------
          |  Cin  |
  A0 -----|A     S|----- S0
          |   FA  |
  B0 -----|B  Cout|----- Cout0
          |       |
          ---------

  Cin1 --------
              |
          ---------
          |  Cin  |
  A1 -----|A     S|----- S1
          |   FA  |
  B1 -----|B  Cout|----- Cout1
          |       |
          ---------

		     ...

  Cinn --------
              |
          ---------
          |  Cin  |
  An -----|A     S|----- Sn
          |   FA  |
  Bn -----|B  Cout|----- Coutn
          |       |
          ---------
*/

bool CarrySaveAdder::entityGenerated = false;

CarrySaveAdder::CarrySaveAdder(string _name,
							   size_t _num_bits)
	: Component(_name)
	, num_bits(_num_bits)
{
	if (num_bits == 0) {
		Error("Number of bits in CarrySaveAdder \"" + _name + "\" must be at least 1.\n");
	}

	for (size_t i = 0; i < num_bits; ++i) {
		const auto fa = make_shared<FullAdder>(name + "_fa_" + to_string(i));
		full_adders.emplace_back(fa);

		for (const auto &w : fa->GetInternalWires()) {
			internal_wires.emplace_back(w);
		}
	}
}

void CarrySaveAdder::Update(bool propagating) {
	if (needs_update || !propagating) {
		for (const auto &fa : full_adders) {
			fa->Update(propagating);
		}

		needs_update = false;
	}

	if (!propagating && print_debug) {
		PrintDebug();
	}
}

void CarrySaveAdder::Connect(PORTS port, const wire_t &wire, size_t index) {
	CheckIfIndexIsInRange(port, index);

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
		full_adders[index]->Connect(PORTS::Cin, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::O:
		full_adders[index]->Connect(PORTS::O, wire);
		output_wires.emplace_back(wire);
		break;
	case PORTS::Cout:
		full_adders[index]->Connect(PORTS::Cout, wire);
		output_wires.emplace_back(wire);
		break;
	default:
	    Error("Trying to connect wire \"" + wire->GetName() + "\" to undefined port of CarrySaveAdder \"" + name + "\".\n");
	}
}

void CarrySaveAdder::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		Error("Wire bundle \"" + wires->GetName() + " accessed with index " + to_string(wire_idx)
			  + " but has size " + to_string(wires->GetSize()) + ".\n");
	}

	const auto &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const wire_t CarrySaveAdder::GetWire(PORTS port, size_t index) const {
	CheckIfIndexIsInRange(port, index);

	switch (port) {
	case PORTS::A:
	case PORTS::B:
	case PORTS::Cin:
	case PORTS::O:
	case PORTS::Cout:
		return full_adders[index]->GetWire(port);
	default:
		Error("Trying to get wire of undefined port of CarrySaveAdder \"" + name + "\".\n");
	}
}

const PORT_DIR CarrySaveAdder::GetPortDirection(PORTS port) const {
	switch (port) {
	case PORTS::A:
	case PORTS::B:
	case PORTS::Cin:
		return PORT_DIR::INPUT;
	case PORTS::Cout:
	case PORTS::O:
		return PORT_DIR::OUTPUT;
	default:
		Error("Trying to get port direction of undefined port in CarrySaveAdder \"" + name + "\".\n");
	}
}

void CarrySaveAdder::CheckIfIndexIsInRange(PORTS port, size_t index) const {
	switch (port) {
	case PORTS::A:
	case PORTS::B:
	case PORTS::Cin:
	case PORTS::O:
	case PORTS::Cout:
		if (index >= num_bits) {
			Error("Index " + to_string(index) + " of port " + PortToPortNameMap[port] + " is out of bounds for CarrySaveAdder \"" + name + "\".\n");
		}
		break;
	default:
		Error("Undefined port \"" + PortToPortNameMap[port] + "\" of CarrySaveAdder \"" + name + "\".\n");
	}
}

void CarrySaveAdder::PrintDebug() const {
	cout << "\n========================================\n";
	cout << name << ':';

	for (const auto &fa : full_adders) {
		const auto &A    = fa->GetWire(PORTS::A);
		const auto &B    = fa->GetWire(PORTS::B);
		const auto &Cin  = fa->GetWire(PORTS::Cin);
		const auto &Cout = fa->GetWire(PORTS::Cout);
		const auto &O    = fa->GetWire(PORTS::O);

		cout << '\n' << fa->GetName() << '\n';
		if (A)    cout << "A ("    << A->GetName()    << "): " << (*A)()    << '\n';
		if (B)    cout << "B ("    << B->GetName()    << "): " << (*B)()    << '\n';
		if (Cin)  cout << "Cin ("  << Cin->GetName()  << "): " << (*Cin)()  << '\n';
		if (O)    cout << "O ("    << O->GetName()    << "): " << (*O)()    << '\n';
		if (Cout) cout << "Cout (" << Cout->GetName() << "): " << (*Cout)() << '\n';
	}

	cout << "========================================\n";
}

void CarrySaveAdder::GenerateVHDLEntity(const string &path) const {
	// We only need to do it once, since all instances of the CarrySaveAdder are identical.
	if (!entityGenerated) {
		string output;
		TemplateDictionary entity("CarrySaveAdder");
		ExpandTemplate("src/templates/VHDL/CarrySaveAdder_entity.tpl", DO_NOT_STRIP, &entity, &output);

		auto outfile = ofstream(path + "/CarrySaveAdder.vhd");
		outfile << output;
		outfile.close();

		full_adders.front()->GenerateVHDLEntity(path);

		entityGenerated = true;
	}
}

const string CarrySaveAdder::GenerateVHDLInstance() const {
	string output;
	TemplateDictionary inst("CarrySaveAdder");
	inst.SetValue("NAME", name);
	inst.SetValue("SIZE", to_string(num_bits));
	ExpandTemplate("src/templates/VHDL/CarrySaveAdder_inst.tpl", DO_NOT_STRIP, &inst, &output);

	return output;
}
