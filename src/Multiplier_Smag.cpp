#include "main.h"

/*
  Signed magnitude multiplier implementation.
*/

bool Multiplier_Smag::entityGenerated = false;

Multiplier_Smag::Multiplier_Smag(string _name,
								 size_t _num_bits_A,
								 size_t _num_bits_B,
								 MUL_TYPE _type)
	: Component(_name)
	, num_bits_A(_num_bits_A - 1)
	, num_bits_B(_num_bits_B - 1)
	, num_bits_O(num_bits_A + num_bits_B + 1)
	, num_adder_levels(num_bits_B - 1)
	, num_and_levels(num_bits_B)
	, num_adders_per_level(num_bits_A)
	, num_ands_per_level(num_bits_A)
	, type(_type)
{
	if (num_bits_A < 2) {
		cout << "[Error] Size of port A of Multiplier_Smag \"" << _name
			 << "\" must be larger or equal to 2.\n";
		exit(1);
	} else if (num_bits_B < 2) {
		cout << "[Error] Size of port B of Multiplier_Smag \"" << _name
			 << "\" must be larger or equal to 2.\n";
		exit(1);
	}

	// Generate the requested hardware.
	switch (type) {
	case MUL_TYPE::CARRY_PROPAGATE: GenerateCarryPropagateArrayHardware(); break;
	case MUL_TYPE::CARRY_SAVE: 		GenerateCarrySaveArrayHardware(); break;
	default:
		cout << "[Error] Unknown type supplied for generating Multiplier_Smag \""
			 << name << "\".\n";
		exit(1);
	}
}

void Multiplier_Smag::Update(bool propagating) {
	if (needs_update || !propagating) {
		switch (type) {
		case MUL_TYPE::CARRY_PROPAGATE:
			for (size_t i = 0; i < longest_path; ++i) {
				for (const auto &and_row : ands) {
					for (const auto &a : and_row) {
						a->Update(propagating);
					}
				}
				for (const auto &adder_row : rc_adders) {
					adder_row->Update(propagating);
				}
			}

			sign->Update(propagating);
			break;
		case MUL_TYPE::CARRY_SAVE:
			for (size_t i = 0; i < longest_path; ++i) {
				for (const auto &and_row : ands) {
					for (const auto &a : and_row) {
						a->Update(propagating);
					}
				}
				for (const auto &adder_row : cs_adders) {
					adder_row->Update(propagating);
				}
			}

			sign->Update(propagating);
			break;
		}

		needs_update = false;
	}
}

void Multiplier_Smag::Connect(PORTS port, const wire_t &wire, size_t index) {
	CheckIfIndexIsInRange(port, index);

	auto error_undefined_port = [&](const auto &wire) {
		cout << "[Error] Trying to connect wire \"" << wire->GetName()
			 << "\" to undefined port of Multiplier_2C "
			 << "\"" << name << "\".\n";
		exit(1);
	};

	switch (port) {
	case PORTS::A:
		if (index == num_adders_per_level) {
			// MSB is the sign bit.
			sign->Connect(PORTS::A, wire);
		} else {
			// Connect to the AND gates at the given index for
			// each level of AND gates.
			for (size_t level = 0; level < num_and_levels; ++level) {
				ands[level][index]->Connect(PORTS::A, wire);
			}
		}
		input_wires.emplace_back(wire);
		break;
	case PORTS::B:
		if (index == num_adders_per_level) {
			// MSB is the sign bit.
			sign->Connect(PORTS::B, wire);
		} else {
			// Connect to all AND gates at level "index".
			for (size_t i = 0; i < num_ands_per_level; ++i) {
				ands[index][i]->Connect(PORTS::B, wire);
			}
		}
		input_wires.emplace_back(wire);
		break;
	case PORTS::O:
		switch (type) {
		case MUL_TYPE::CARRY_PROPAGATE:
			if (index == (num_bits_O - 1)) {
				// MSB is the sign bit.
				sign->Connect(PORTS::O, wire);
			} else {
				if (index == (num_bits_O - 2)) {
					// One bit less than the sign bit is the carry out of the last full adder.
					rc_adders.back()->Connect(PORTS::Cout, wire, num_adders_per_level - 1);
				} else if (index > (num_bits_O - num_adders_per_level - 2)) {
					rc_adders.back()->Connect(PORTS::O, wire, index - num_adders_per_level + 1);
				} else if (index == 0) {
					ands[0][0]->Connect(PORTS::O, wire);
				} else {
					rc_adders[index - 1]->Connect(PORTS::O, wire, 0);
				}
			}
			break;
		case MUL_TYPE::CARRY_SAVE:
			if (index == (num_bits_O - 1)) {
				// MSB is the sign bit.
				sign->Connect(PORTS::O, wire);
			} else {
				if (index == (num_bits_O - 2)) {
					// One bit less than the sign bit is the carry out of the last full adder.
					cs_adders.back()->Connect(PORTS::Cout, wire, num_adders_per_level - 1);
				} else if (index > (num_bits_O - num_adders_per_level - 2)) {
					cs_adders.back()->Connect(PORTS::O, wire, index - num_adders_per_level + 1);
				} else if (index == 0) {
					ands[0][0]->Connect(PORTS::O, wire);
				} else {
					cs_adders[index - 1]->Connect(PORTS::O, wire, 0);
				}
			}
			break;
		}
		output_wires.emplace_back(wire);
		break;
	default:
		error_undefined_port(wire);
	}
}

void Multiplier_Smag::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		cout << "[Error] Wire bundle \"" << wires->GetName()
			 << " accessed with index " << wire_idx
			 << " but has size " << wires->GetSize() << ".\n";
		exit(1);
	}

	const wire_t &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const wire_t Multiplier_Smag::GetWire(PORTS port, size_t index) const {
	CheckIfIndexIsInRange(port, index);

	switch (port) {
		/* Inputs */
	case PORTS::A:
		if (index == num_adders_per_level) {
			return sign->GetWire(PORTS::A);
		} else {
			return ands[0][index]->GetWire(PORTS::A);
		}
		break;
	case PORTS::B:
		if (index == num_adders_per_level) {
			return sign->GetWire(PORTS::B);
		} else {
			return ands[index][0]->GetWire(PORTS::B);
		}
		break;
	case PORTS::O:
		switch (type) {
		case MUL_TYPE::CARRY_PROPAGATE:
			if (index == (num_bits_O - 1)) {
				return sign->GetWire(PORTS::O);
			} else if (index == (num_bits_O - 2)) {
				return rc_adders.back()->GetWire(PORTS::Cout, num_adders_per_level - 1);
			} else if (index > (num_bits_O - num_adders_per_level - 2)) {
				return rc_adders.back()->GetWire(PORTS::O, index - num_adders_per_level + 1);
			} else if (index == 0) {
				return ands[0][0]->GetWire(PORTS::O);
			} else {
				return rc_adders[index - 1]->GetWire(PORTS::O, 0);
			}
			break;
		case MUL_TYPE::CARRY_SAVE:
			if (index == (num_bits_O - 1)) {
				return sign->GetWire(PORTS::O);
			} else if (index == (num_bits_O - 2)) {
				return cs_adders.back()->GetWire(PORTS::Cout, num_adders_per_level - 1);
			} else if (index > (num_bits_O - num_adders_per_level - 2)) {
				return cs_adders.back()->GetWire(PORTS::O, index - num_adders_per_level + 1);
			} else if (index == 0) {
				return ands[0][0]->GetWire(PORTS::O);
			} else {
				return cs_adders[index - 1]->GetWire(PORTS::O, 0);
			}
			break;
		}
	default:
		cout << "[Error] Trying to get wire of undefined port of Multiplier_Smag "
			 << "\"" << name << "\".\n";
		exit(1);
	}

	return nullptr;
}

void Multiplier_Smag::GenerateCarryPropagateArrayHardware() {
	const auto create_rca = [&](const auto &name, const size_t size) {
		const auto rca = make_shared<RippleCarryAdder>(name, size);
		rc_adders.emplace_back(rca);

		for (const auto &w : rca->GetInternalWires()) {
			internal_wires.emplace_back(w);
		}
	};

	sign = make_shared<Xor>(name + "_sign");

	string name_prefix = name + "_rca_";

	for (size_t i = 0; i < num_adder_levels; ++i) {
		create_rca(name_prefix + to_string(i), num_adders_per_level);
	}

	// Connect the O outputs of the full adders.
	for (size_t y = 0; y < (num_adder_levels - 1); ++y) {
		name_prefix = name + "_rca_" + to_string(y) + "_fa_";

		for (size_t x = 1; x < num_adders_per_level; ++x) {
			const string row_name = name_prefix + to_string(x) + "_O";

			const auto wire = make_shared<Wire>(row_name);
			rc_adders[y]->Connect(PORTS::O, wire, x);
			rc_adders[y + 1]->Connect(PORTS::A, wire, x - 1);
			internal_wires.emplace_back(wire);
		}
	}

	// Create the AND gates that connect to the inputs of the adders.
	vector<and_t> ands_row;

	// First level of AND gates is slightly different, so handle it
	// separately. The first AND gate is directly connected to bit 0
	// of the result.
	name_prefix = name + "_AND_0_";
	ands_row.emplace_back(make_shared<And>(name_prefix + "0"));

	for (size_t a = 1; a < num_ands_per_level; ++a) {
		const string row_name = name_prefix + to_string(a);

		const auto and_gate = make_shared<And>(row_name);
		const auto and_wire = make_shared<Wire>(row_name + "_O");
		and_gate->Connect(PORTS::O, and_wire);
		rc_adders[0]->Connect(PORTS::A, and_wire, a - 1);
		ands_row.emplace_back(and_gate);

		internal_wires.emplace_back(and_wire);
	}
	ands.emplace_back(ands_row);
	ands_row.clear();

	// Now handle the rest of the AND gate levels.
	for (size_t b = 1; b < num_and_levels; ++b) {
		name_prefix = name + "_AND_" + to_string(b) + "_";

		for (size_t a = 0; a < num_ands_per_level; ++a) {
			const string row_name = name_prefix + to_string(a);

			const auto and_gate = make_shared<And>(row_name);
			const auto and_wire = make_shared<Wire>(row_name + "_O");
			and_gate->Connect(PORTS::O, and_wire);
			rc_adders[b - 1]->Connect(PORTS::B, and_wire, a);
			ands_row.emplace_back(and_gate);

			internal_wires.emplace_back(and_wire);
		}

		ands.emplace_back(ands_row);
		ands_row.clear();
	}

	// Create the Cout connections.
	for (size_t b = 0; b < num_adder_levels; ++b) {
		name_prefix = name + "_Cout_" + to_string(b) + "_";
		const bool last_b = b == (num_adder_levels - 1);

		if (b == 0) {
			// If we do not have 1 level of adders...
			if (num_adder_levels != 1) {
				// First row has 2 HalfAdders so the connections are slightly different.
				const size_t a = num_adders_per_level - 1;
				const string row_name = name_prefix + to_string(a);
				const auto wire = make_shared<Wire>(row_name);

				rc_adders[b]->Connect(PORTS::Cout, wire, a);
				rc_adders[b + 1]->Connect(PORTS::A, wire, a);

				internal_wires.emplace_back(wire);
			} else {
				// If we have only 1 level of adders, do not connect the last
				// full adder carry output to the next level, since there is none.
			}
		} else {
			// The rest of the rows all have the same connections.
			const size_t a = num_adders_per_level - 1;
			const string row_name = name_prefix + to_string(a);

			// The very last one FullAdder connects to the output, so only
			// connect the a wire here if it's not the last FullAdder.
			if (!last_b) {
				const auto wire = make_shared<Wire>(row_name);
				rc_adders[b]->Connect(PORTS::Cout, wire, a);
				internal_wires.emplace_back(wire);

				// The last FullAdder connects to the A input of the
				// adder of the next level instead of the Cin port.
				rc_adders[b + 1]->Connect(PORTS::A, wire, a);
			}
		}
	}
}

void Multiplier_Smag::GenerateCarrySaveArrayHardware() {
	const auto create_csa = [&](const auto &name, const size_t size) {
		const auto csa = make_shared<CarrySaveAdder>(name, size);
		cs_adders.emplace_back(csa);

		for (const auto &w : csa->GetInternalWires()) {
			internal_wires.emplace_back(w);
		}
	};

	sign = make_shared<Xor>(name + "_sign");

	string name_prefix = name + "_csa_";

	for (size_t i = 0; i < num_adder_levels; ++i) {
		create_csa(name_prefix + to_string(i), num_adders_per_level);
	}

	// Connect the O outputs of the full adders.
	for (size_t y = 0; y < (num_adder_levels - 1); ++y) {
		name_prefix = name + "_csa_" + to_string(y);

		for (size_t x = 1; x < num_adders_per_level; ++x) {
			const string row_name = name_prefix + '_' + to_string(x) + "_O";

			const auto wire = make_shared<Wire>(row_name);
			cs_adders[y]->Connect(PORTS::O, wire, x);
			cs_adders[y + 1]->Connect(PORTS::A, wire, x - 1);
			internal_wires.emplace_back(wire);
		}
	}

	// Create the AND gates that connect to the inputs of the adders.
	vector<and_t> ands_row;

	// First three levels of AND gates are slightly different, so handle
	// it separately. The first AND gate is directly connected to bit 0
	// of the result.
	name_prefix = name + "_AND_0_";
	ands_row.emplace_back(make_shared<And>(name_prefix + '0'));

	// Level 0
	for (size_t a = 1; a < num_ands_per_level; ++a) {
		const string row_name = name_prefix + to_string(a);

		const auto and_gate = make_shared<And>(row_name);
		const auto and_wire = make_shared<Wire>(row_name + "_O");
		and_gate->Connect(PORTS::O, and_wire);
		cs_adders[0]->Connect(PORTS::A, and_wire, a - 1);
		ands_row.emplace_back(and_gate);

		internal_wires.emplace_back(and_wire);
	}
	ands.emplace_back(ands_row);
	ands_row.clear();

	// Level 1
	name_prefix = name + "_AND_1_";
	for (size_t a = 0; a < num_ands_per_level; ++a) {
		const string row_name = name_prefix + to_string(a);

		const auto and_gate = make_shared<And>(row_name);
		const auto and_wire = make_shared<Wire>(row_name + "_O");
		and_gate->Connect(PORTS::O, and_wire);
		cs_adders[0]->Connect(PORTS::B, and_wire, a);
		ands_row.emplace_back(and_gate);

		internal_wires.emplace_back(and_wire);
	}
	ands.emplace_back(ands_row);
	ands_row.clear();

	// Level 2
	name_prefix = name + "_AND_2_";
	{
		size_t x = 1;
		size_t y = 0;

		for (size_t a = 0; a < num_ands_per_level; ++a) {
			const string row_name = name_prefix + to_string(a);

			const auto and_gate = make_shared<And>(row_name);
			const auto and_wire = make_shared<Wire>(row_name + "_O");
			and_gate->Connect(PORTS::O, and_wire);

			if (x == num_adders_per_level) {
				x = (num_adders_per_level - 1);
				y++;
			}

			if (x == (num_adders_per_level - 1)) {
				cs_adders[y]->Connect(PORTS::A, and_wire, x++);
			} else {
				cs_adders[y]->Connect(PORTS::Cin, and_wire, x++);
			}
			ands_row.emplace_back(and_gate);

			internal_wires.emplace_back(and_wire);
		}
	}
	ands.emplace_back(ands_row);
	ands_row.clear();

	// Now handle the rest of the AND gate levels.
	for (size_t b = 3; b < num_and_levels; ++b) {
		name_prefix = name + "_AND_" + to_string(b) + '_';

		size_t x = 1;		// Starting x position in the row.
		size_t y = b - 2;	// Starting y position is level - 2.

		for (size_t a = 0; a < num_ands_per_level; ++a) {
			const string row_name = name_prefix + to_string(a);

			const auto and_gate = make_shared<And>(row_name);
			const auto and_wire = make_shared<Wire>(row_name + "_O");
			and_gate->Connect(PORTS::O, and_wire);

			if (x == num_adders_per_level) {
				x = (num_adders_per_level - 1);
				y++;
			}

			if (a >= (num_adders_per_level - 1)) {
				cs_adders[y]->Connect(PORTS::A, and_wire, x);
				x++;
			} else {
				cs_adders[y]->Connect(PORTS::Cin, and_wire, x);
				x++;
			}
			ands_row.emplace_back(and_gate);

			internal_wires.emplace_back(and_wire);
		}

		ands.emplace_back(ands_row);
		ands_row.clear();
	}

	// Create the Cout connections for each adder level except the last.
	for (size_t b = 0; b < (num_adder_levels - 1); ++b) {
		name_prefix = name + "_Cout_" + to_string(b) + '_';

		for (size_t a = 0; a < num_adders_per_level; ++a) {
			const string row_name = name_prefix + to_string(a);
			const auto wire = make_shared<Wire>(row_name);
			cs_adders[b]->Connect(PORTS::Cout, wire, a);
			cs_adders[b + 1]->Connect(PORTS::B, wire, a);

			internal_wires.emplace_back(wire);
		}
	}

	// Create the Cout connections for the last level.
	name_prefix = name + "_Cout_" + to_string(num_adders_per_level - 1) + '_';
	for (size_t a = 0; a < (num_adders_per_level - 1); ++a) {
		const string row_name = name_prefix + to_string(a);
		const auto wire = make_shared<Wire>(row_name);
		cs_adders.back()->Connect(PORTS::Cout, wire, a);
		cs_adders.back()->Connect(PORTS::Cin, wire, a + 1);

		internal_wires.emplace_back(wire);
	}
}

void Multiplier_Smag::CheckIfIndexIsInRange(PORTS port, size_t index) const {
	if (port == PORTS::A && index > num_bits_A) {
		cout << "[Error] Index " << index << " of port A is out of "
			 << "bounds for Multiplier_Smag \"" << name << "\".\n";
		exit(1);
	} else if (port == PORTS::B && index > num_bits_B) {
		cout << "[Error] Index " << index << " of port B is out of "
			 << "bounds for Multiplier_Smag \"" << name << "\".\n";
		exit(1);
	} else if (port == PORTS::O && index >= num_bits_O) {
		cout << "[Error] Index " << index << " of port O is out of "
			 << "bounds for Multiplier_Smag \"" << name << "\".\n";
		exit(1);
	}
}

void Multiplier_Smag::GenerateVHDLEntity(const string &path) const {
	// We only need to generate it once, since all instances of Multiplier_Smag are identical.
	if (!entityGenerated) {
		string output;
		TemplateDictionary entity("Multiplier_Smag");
		ExpandTemplate("src/templates/VHDL/Multiplier_Smag_entity.tpl", DO_NOT_STRIP, &entity, &output);

		auto outfile = ofstream(path + "/Multiplier_Smag.vhd");
		outfile << output;
		outfile.close();

		// Generate entities of other components used by this component.
		if (cs_adders.size() > 0) {
			cs_adders.back()->GenerateVHDLEntity(path);
		}
		//if (rc_adders.size() > 0) {
		rc_adders.back()->GenerateVHDLEntity(path);
		//}

		entityGenerated = true;
	}
}

const string Multiplier_Smag::GenerateVHDLInstance() const {
	string output;
	TemplateDictionary inst("Multiplier_Smag");

	// Set up the I/O wires.
	inst.SetValue("NAME", name);
	inst.SetValue("NUM_BITS_A", to_string(num_bits_A + 1));
	inst.SetValue("NUM_BITS_B", to_string(num_bits_B + 1));
	string arch_str;
	switch (type) {
	case MUL_TYPE::CARRY_PROPAGATE:
		arch_str = "carry_propagate";
		break;
	case MUL_TYPE::CARRY_SAVE:
		arch_str = "carry_save";
		break;
	}

	inst.SetValue("ARCH", arch_str);

	// A
	{
		const auto &wire = GetWire(PORTS::A);
		if (wire) {
			const auto &wb = wire->GetWireBundle();
			if (wb) {
				inst.SetValue("A_WIRE", "int_" + wb->GetName());
			} else {
				inst.SetValue("A_WIRE", "int_" + wire->GetName());
			}
		} else {
			// No wire, so assign a '0';
			inst.SetValue("A_WIRE", "(OTHERS => '0')");
		}
	}

	// B
	{
		const auto &wire = GetWire(PORTS::B);
		if (wire) {
			const auto &wb = wire->GetWireBundle();
			if (wb) {
				inst.SetValue("B_WIRE", "int_" + wb->GetName());
			} else {
				inst.SetValue("B_WIRE", "int_" + wire->GetName());
			}
		} else {
			// No wire, so assign a '0';
			inst.SetValue("B_WIRE", "(OTHERS => '0')");
		}
	}

	// O
	{
		const auto &wire = GetWire(PORTS::O);
		if (wire) {
			const auto &wb = wire->GetWireBundle();
			if (wb) {
				inst.SetValue("O_WIRE", "int_" + wb->GetName());
			} else {
				inst.SetValue("O_WIRE", "int_" + wire->GetName());
			}
		} else {
			// No wire, so assign a '0';
			inst.SetValue("O_WIRE", "(OTHERS => '0')");
		}
	}

	ExpandTemplate("src/templates/VHDL/Multiplier_Smag_inst.tpl", DO_NOT_STRIP, &inst, &output);

	return output;
}
