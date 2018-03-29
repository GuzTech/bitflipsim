#include "main.h"

/*
  Ripple-Carry subtracter implementation. Uses the
  Ripple-Carry adder component.

  Schematic:

                        '1'
                         |
                         |
                     ---------
                     |  Cin  |
               A0 ---|A     S|----- S0
          |N|        |   FA  |
  B0 -----|O|--------|B      |
		  |T|        |  Cout |
                     ---------
                         |
                         |
                         |
                     ---------
                     |  Cin  |
               A1 ---|A     S|----- S1
          |N|        |   FA  |
  B1 -----|O|--------|B      |
          |T|        |  Cout |
                     ---------
                         |
                        ...
                         |
                     ---------
                     |  Cin  |
               An ---|A     S|----- Sn
          |N|        |   FA  |
  Bn -----|O|--------|B      |
		  |T|        |  Cout |
                     ---------
*/

bool RippleCarrySubtracter::entityGenerated = false;

RippleCarrySubtracter::RippleCarrySubtracter(string _name, size_t _num_bits)
	: Component(_name, _num_bits)
	, num_bits(_num_bits)
{
	// Reserve space.
	nots.reserve(num_bits);

	// Create the ripple-carry adder.
	adder = make_shared<RippleCarryAdder>(name, num_bits);
	for (const auto &w : adder->GetInternalWires()) {
		internal_wires.emplace_back(w);
	}

	// Create the wire that is fixed to logic high.
	fixed_one = make_shared<Wire>(name + "_fixed_one");
	fixed_one->SetValue(true, true);
	internal_wires.emplace_back(fixed_one);
	adder->Connect(PORTS::Cin, fixed_one, 0);

	// Create the NOT gates and wires to the ripple-carry adder.
	for (size_t i = 0; i < num_bits; ++i) {
		string not_name = name + "_not_" + to_string(i);

		const auto n = make_shared<Not>(not_name);
		nots.emplace_back(n);

		string wire_name = not_name + "_to_rca";
		auto wire = make_shared<Wire>(wire_name);
		n->Connect(PORTS::O, wire);
		adder->Connect(PORTS::B, wire, i);
		internal_wires.emplace_back(wire);
	}
}

void RippleCarrySubtracter::Update(bool propagating) {
	if (needs_update || !propagating) {
		for (size_t i = 0; i < longest_path; ++i) {
			for (auto &n : nots) {
				n->Update(propagating);
			}

			adder->Update(propagating);
		}

		if (!propagating && print_debug) {
			PrintDebug();
		}

		needs_update = false;
	}
}

void RippleCarrySubtracter::Connect(PORTS port, const wire_t &wire, size_t index) {
	if (index >= num_bits) {
		cout << "[Error] Index " << index << " out of bounds for "
			 << "RippleCarrySubtracter \"" << name << "\".\n";
		exit(1);
	}

	switch (port) {
	case PORTS::A:
		adder->Connect(port, wire, index);
		input_wires.emplace_back(wire);
		break;
	case PORTS::B:
		nots[index]->Connect(PORTS::I, wire);
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
		cout << "[Error] Trying to connect to undefined port of RippleCarrySubtracter "
			 << "\"" << name << "\".\n";
		exit(1);
	}
}

void RippleCarrySubtracter::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		cout << "[Error] Wire bundle \"" << wires->GetName()
			 << " accessed with index " << wire_idx
			 << " but has size " << wires->GetSize() << ".\n";
		exit(1);
	}

	const wire_t &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const wire_t RippleCarrySubtracter::GetWire(PORTS port, size_t index) const {
	if (index >= num_bits) {
		cout << "[Error] Index " << index << " out of bound for "
			 << "RippleCarrySubtracter \"" << name << "\".\n";
		exit(1);
	}

	switch (port) {
	case PORTS::A:
	case PORTS::O:
	case PORTS::Cout:
		return adder->GetWire(port, index);
	case PORTS::B:
		return nots[index]->GetWire(PORTS::I);
	default:
		cout << "[Error] Trying to retrieve undefined port of RippleCarrySubtracter "
			 << "\"" << name << "\".\n";
		exit(1);
	}
}

const PORT_DIR RippleCarrySubtracter::GetPortDirection(PORTS port) const {
	switch (port) {
	case PORTS::A:
	case PORTS::B:
		return PORT_DIR::INPUT;
	case PORTS::Cout:
	case PORTS::O:
		return PORT_DIR::OUTPUT;
	default:
		cout << "[Error] Trying to get port direction of undefined port in RippleCarrySubtracter "
			 << "\"" << name << "\".\n";
		exit(1);
	}
}

void RippleCarrySubtracter::PrintDebug() const {
	cout << "\n========================================\n";
	cout << name << ':';

	adder->PrintDebug();
	// TODO: Print the values of the NOT gate outputs.

	cout << "========================================\n";
}

void RippleCarrySubtracter::GenerateVHDLEntity(const string &path) const {
	// We only need to do it once, since all instances of the RippleCarrySubtracter are identical.
	if (!entityGenerated) {
		string output;
		TemplateDictionary entity("RippleCarrySubtracter");
		ExpandTemplate("src/templates/VHDL/RippleCarrySubtracter_entity.tpl", DO_NOT_STRIP, &entity, &output);

		auto outfile = ofstream(path + "/RippleCarrySubtracter.vhd");
		outfile << output;
		outfile.close();

		nots.front()->GenerateVHDLEntity(path);
		adder->GenerateVHDLEntity(path);

		entityGenerated = true;
	}
}

const string RippleCarrySubtracter::GenerateVHDLInstance() const {
	string output;
	TemplateDictionary inst("RippleCarrySubtracter");

	GenerateAssignments(PORTS::A, num_bits, "A", inst);
	GenerateAssignments(PORTS::B, num_bits, "B", inst);
	GenerateAssignments(PORTS::O, num_bits, "O", inst);
	GenerateAssignments(PORTS::Cout, 1, "Cout", inst, true);

	inst.SetValue("NAME", name);
	inst.SetValue("SIZE", to_string(num_bits));
	ExpandTemplate("src/templates/VHDL/RippleCarrySubtracter_inst.tpl", DO_NOT_STRIP, &inst, &output);

	return output;
}
