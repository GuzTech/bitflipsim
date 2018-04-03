#include "main.h"

/*
  Ripple-Carry adder/subtracter implementation. Uses the
  Ripple-Carry adder.

  Schematic:

                    #ADD/SUBTRACT
                         |
       ------------------+
       |                 |
       |             ---------
       |             |  Cin  |
       |       A0 ---|A     S|----- S0
       +--|X|        |   FA  |
       |  |O|--------|B      |
  B0 -----|R|        |  Cout |
       |             ---------
       |                 |
       |                 |
       |                 |
       |             ---------
       |             |  Cin  |
       |       A1 ---|A     S|----- S1
       +--|X|        |   FA  |
       |  |O|--------|B      |
  B1 -----|R|        |  Cout |
       |             ---------
       |                 |
      ...               ...
       |                 |
       |             ---------
       |             |  Cin  |
       |       An ---|A     S|----- Sn
       ---|X|        |   FA  |
          |O|--------|B      |
  Bn -----|R|        |  Cout |
                     ---------
*/

bool RippleCarryAdderSubtracter::entityGenerated = false;

RippleCarryAdderSubtracter::RippleCarryAdderSubtracter(string _name, size_t _num_bits)
	: Component(_name, _num_bits)
	, num_bits(_num_bits)
{
	// Reserve space.
	xors.reserve(num_bits);

	// Create the ripple-carry adder.
	adder = make_shared<RippleCarryAdder>(name, num_bits);

	// Create the XOR gates and wires to the ripple-carry adder
	for (size_t i = 0; i < num_bits; ++i) {
		string xor_name = name + "_xor_" + to_string(i);

		const auto x = make_shared<Xor>(xor_name);
		xors.emplace_back(x);

		string wire_name = xor_name + "_to_rca";
		auto wire = make_shared<Wire>(wire_name);
		x->Connect(PORTS::B, wire);
		internal_wires.emplace_back(wire);
	}
}

void RippleCarryAdderSubtracter::Update(bool propagating) {
	if (needs_update || !propagating) {
		for (size_t i = 0; i < longest_path; ++i) {
			for (auto &x : xors) {
				x->Update(propagating);
			}

			adder->Update(propagating);
		}

		if (!propagating && print_debug) {
			PrintDebug();
		}

		needs_update = false;
	}
}

void RippleCarryAdderSubtracter::Connect(PORTS port, const wire_t &wire, size_t index) {
	if (index >= num_bits) {
		Error("Index " + to_string(index) + " out of bounds for RippleCarryAdderSubtracter \"" + name + "\".\n");
	}

	switch (port) {
	case PORTS::A:
		adder->Connect(port, wire, index);
		input_wires.emplace_back(wire);
		break;
	case PORTS::B:
		xors[index]->Connect(PORTS::B, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::O:
		adder->Connect(port, wire, index);
		output_wires.emplace_back(wire);
		break;
	case PORTS::Cout:
		adder->Connect(port, wire, index);
		output_wires.emplace_back(wire);
		break;
	default:
		Error("Trying to connect to undefined port of RippleCarryAdderSubtracter \"" + name + "\".\n");
	}
}

void RippleCarryAdderSubtracter::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		Error("Wire bundle \"" + wires->GetName() + " accessed with index " + to_string(wire_idx)
			  + " but has size " + to_string(wires->GetSize()) + ".\n");
	}

	const wire_t &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const wire_t RippleCarryAdderSubtracter::GetWire(PORTS port, size_t index) const {
	if (index >= num_bits) {
		Error("Index " + to_string(index) + " out of bound for RippleCarryAdderSubtracter \"" + name + "\".\n");
	}

	switch (port) {
	case PORTS::A:
	case PORTS::B:
	case PORTS::O:
	case PORTS::Cin:
	case PORTS::Cout:
		return adder->GetWire(port, index);
	default:
		Error("Trying to retrieve undefined port of RippleCarryAdderSubtracter \"" + name + "\".\n");
	}
}

const PORT_DIR RippleCarryAdderSubtracter::GetPortDirection(PORTS port) const {
	switch (port) {
	case PORTS::A:
	case PORTS::B:
	case PORTS::Cin:
		return PORT_DIR::INPUT;
	case PORTS::Cout:
	case PORTS::O:
		return PORT_DIR::OUTPUT;
	default:
		Error("Trying to get port direction of undefined port in RippleCarryAdderSubtracter \"" + name + "\".\n");
	}
}

void RippleCarryAdderSubtracter::PrintDebug() const {
	cout << "\n========================================\n";
	cout << name << ':';

	adder->PrintDebug();

	cout << "========================================\n";
}

//void RippleCarryAdderSubtracter::GenerateVHDLEntity(const string &path) const {
//	// We only need to do it once, since all instances of the RippleCarryAdderSubtracter are identical.
//	if (!entityGenerated) {
//		string output;
//		TemplateDictionary entity("RippleCarryAdderSubtracter");
//		ExpandTemplate("src/templates/VHDL/RippleCarryAdderSubtracter_entity.tpl", DO_NOT_STRIP, &entity, &output);
//
//		auto outfile = ofstream(path + "/RippleCarryAdderSubtracter.vhd");
//		outfile << output;
//		outfile.close();
//
//		full_adders.front()->GenerateVHDLEntity(path);
//
//		entityGenerated = true;
//	}
//}

const string RippleCarryAdderSubtracter::GenerateVHDLInstance() const {
	string output;
	TemplateDictionary inst("RippleCarryAdderSubtracter");
	inst.SetValue("NAME", name);
	inst.SetValue("SIZE", to_string(num_bits));
	ExpandTemplate("src/templates/VHDL/RippleCarryAdderSubtracter_inst.tpl", DO_NOT_STRIP, &inst, &output);

	return output;
}
