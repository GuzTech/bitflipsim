#include "main.h"

/*
  Signed magnitude multiplier implementation.
*/

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
		case MUL_TYPE::CARRY_SAVE:
			for (size_t i = 0; i < longest_path; ++i) {
				for (const auto &and_row : ands) {
					for (const auto &a : and_row) {
						a->Update(propagating);
					}
				}
				for (const auto &adder_row : adders) {
					for (const auto &a : adder_row) {
						a->Update(propagating);
					}
				}
			}

			sign->Update(propagating);
			break;
		}

		needs_update = false;
	}
}

void Multiplier_Smag::Connect(PORTS port, const wire_t &wire, size_t index) {
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

	auto error_undefined_port = [&](const auto &wire) {
		cout << "[Error] Trying to connect wire \"" << wire->GetName()
			 << "\" to undefined port of Multiplier_2C "
			 << "\"" << name << "\".\n";
		exit(1);
	};

	switch (type) {
	case MUL_TYPE::CARRY_PROPAGATE:
	case MUL_TYPE::CARRY_SAVE:
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
			if (index == (num_bits_O - 1)) {
				// MSB is the sign bit.
				sign->Connect(PORTS::O, wire);
			} else {
				if (index == (num_bits_O - 2)) {
					// One bit less than the sign bit is the carry out of the last full adder.
					adders.back().back()->Connect(PORTS::Cout, wire);
				} else if (index > (num_bits_O - num_adders_per_level - 2)) {
					adders.back()[index - num_adders_per_level + 1]->Connect(PORTS::O, wire);
				} else if (index == 0) {
					ands[0][0]->Connect(PORTS::O, wire);
				} else {
					adders[index - 1][0]->Connect(PORTS::O, wire);
				}
			}
			output_wires.emplace_back(wire);
			break;
		default:
			error_undefined_port(wire);
		}
		break;
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
	return nullptr;
}

void Multiplier_Smag::GenerateCarryPropagateArrayHardware() {
	sign = make_shared<Xor>(name + "_sign");

	// Names for the adders.
	string row_name_prefix = string(name) + "_S_0_";
	string row_name = row_name_prefix + "0";

	// First row consists of #A - 2 FullAdders and 2 HalfAdders.
	vector<comp_t> adders_row;

	adders_row.emplace_back(make_shared<HalfAdder>(row_name));
	for (size_t a = 1; a < (num_adders_per_level - 1); ++a) {
		row_name = row_name_prefix + to_string(a);

		adders_row.emplace_back(make_shared<FullAdder>(row_name));
	}
	row_name = row_name_prefix + to_string(num_adders_per_level - 1);
	adders_row.emplace_back(make_shared<HalfAdder>(row_name));

	adders.emplace_back(adders_row);
	adders_row.clear();

	// The rest of the rows consist of #A FullAdders and 1 HalfAdder.
	for (size_t b = 1; b < num_adder_levels; ++b) {
		row_name_prefix = name + "_S_" + to_string(b) + "_";
		row_name = row_name_prefix + "0";
		adders_row.emplace_back(make_shared<HalfAdder>(row_name));

		for (size_t a = 1; a < num_adders_per_level; ++a) {
			row_name = row_name_prefix + to_string(a);
			adders_row.emplace_back(make_shared<FullAdder>(row_name));
		}

		adders.emplace_back(adders_row);
		adders_row.clear();
	}

	// Connect the O outputs of the full adders.
	for (size_t y = 0; y < (num_adder_levels - 1); ++y) {
		row_name_prefix = name + "_S_" + to_string(y);

		for (size_t x = 1; x < num_adders_per_level; ++x) {
			row_name = row_name_prefix + "_" + to_string(x) + "_O";

			const auto wire = make_shared<Wire>(row_name);
			adders[y][x]->Connect(PORTS::O, wire);
			adders[y+1][x-1]->Connect(PORTS::A, wire);
			internal_wires.emplace_back(wire);
		}
	}

	// Create the AND gates that connect to the inputs of the adders.
	vector<and_t> ands_row;

	// First level of AND gates is slightly different, so handle it
	// separately. The first AND gate is directly connected to bit 0
	// of the result.
	row_name_prefix = name + "_AND_0_";
	ands_row.emplace_back(make_shared<And>(row_name_prefix + "0"));

	for (size_t a = 1; a < num_ands_per_level; ++a) {
		row_name = row_name_prefix + to_string(a);

		const auto and_gate = make_shared<And>(row_name);
		const auto and_wire = make_shared<Wire>(row_name + "_O");
		and_gate->Connect(PORTS::O, and_wire);
		adders[0][a-1]->Connect(PORTS::A, and_wire);
		ands_row.emplace_back(and_gate);

		internal_wires.emplace_back(and_wire);
	}
	ands.emplace_back(ands_row);
	ands_row.clear();

	// Now handle the rest of the AND gate levels.
	for (size_t b = 1; b < num_and_levels; ++b) {
		row_name_prefix = name + "_AND_" + to_string(b) + "_";

		for (size_t a = 0; a < num_ands_per_level; ++a) {
			row_name = row_name_prefix + to_string(a);

			const auto and_gate = make_shared<And>(row_name);
			const auto and_wire = make_shared<Wire>(row_name + "_O");
			and_gate->Connect(PORTS::O, and_wire);
			adders[b-1][a]->Connect(PORTS::B, and_wire);
			ands_row.emplace_back(and_gate);

			internal_wires.emplace_back(and_wire);
		}

		ands.emplace_back(ands_row);
		ands_row.clear();
	}

	// Create the connections between Cout and Cin of the adders.
	for (size_t b = 0; b < num_adder_levels; ++b) {
		row_name_prefix = name + "_Cout_" + to_string(b) + "_";
		const bool last_b = b == (num_adder_levels - 1);

		if (b == 0) {
			// If we do not have 1 level of adders...
			if (num_adder_levels != 1) {
				// First row has 2 HalfAdders so the connections are slightly different.
				for (size_t a = 0; a < num_adders_per_level; ++a) {
					row_name = row_name_prefix + to_string(a);

					const bool second_to_last_a = a == (num_adders_per_level - 2);
					const bool last_a = a == (num_adders_per_level - 1);

					const auto wire = make_shared<Wire>(row_name);
					adders[b][a]->Connect(PORTS::Cout, wire);
					internal_wires.emplace_back(wire);

					if (last_a) {
						adders[b+1][a]->Connect(PORTS::A, wire);
					} else {
						if (second_to_last_a) {
							adders[b][a+1]->Connect(PORTS::A, wire);
						} else {
							adders[b][a+1]->Connect(PORTS::Cin, wire);
						}
					}
				}
			} else {
				// If we have only 1 level of adders, do not connect the last
				// full adder carry output to the next level, since there is none.
				for (size_t a = 0; a < (num_adders_per_level - 1); ++a) {
					row_name = row_name_prefix + to_string(a);

					const bool second_to_last_a = a == (num_adders_per_level - 2);

					const auto wire = make_shared<Wire>(row_name);
					adders[b][a]->Connect(PORTS::Cout, wire);
					internal_wires.emplace_back(wire);

					if (second_to_last_a) {
						adders[b][a+1]->Connect(PORTS::A, wire);
					} else {
						adders[b][a+1]->Connect(PORTS::Cin, wire);
					}
				}
			}
		} else {
			// The rest of the rows all have the same connections.
			for (size_t a = 0; a < num_bits_A; ++a) {
				row_name = row_name_prefix + to_string(a);

				const bool last_a = a == (num_adders_per_level - 1);

				// The very last one FullAdder connects to the output, so only
				// connect the a wire here if it's not the last FullAdder.
				if (!(last_a && last_b)) {
					const auto wire = make_shared<Wire>(row_name);
					adders[b][a]->Connect(PORTS::Cout, wire);
					internal_wires.emplace_back(wire);

					if (last_a) {
						// The last FullAdder connects to the A input of the
						// adder of the next level instead of the Cin port.
						adders[b+1][a]->Connect(PORTS::A, wire);
					} else {
						adders[b][a+1]->Connect(PORTS::Cin, wire);
					}
				}
			}
		}
	}
}

void Multiplier_Smag::GenerateCarrySaveArrayHardware() {
	sign = make_shared<Xor>(name + "_sign");

	// Names for the adders.
	string row_name_prefix = string(name) + "_S_0_";
	string row_name = row_name_prefix + "0";

	// First row consists of (num_bits_A - 2) FullAdders and 2 HalfAdders.
	vector<comp_t> adders_row;

	adders_row.emplace_back(make_shared<HalfAdder>(row_name));
	for (size_t a = 1; a < (num_adders_per_level - 1); ++a) {
		row_name = row_name_prefix + to_string(a);

		adders_row.emplace_back(make_shared<FullAdder>(row_name));
	}
	row_name = row_name_prefix + to_string(num_adders_per_level - 1);
	adders_row.emplace_back(make_shared<HalfAdder>(row_name));

	adders.emplace_back(adders_row);
	adders_row.clear();

	// The rest of the rows consist of (num_bits_A - 2) FullAdders and 1 HalfAdder.
	for (size_t b = 1; b < num_adder_levels; ++b) {
		row_name_prefix = name + "_S_" + to_string(b) + "_";
		row_name = row_name_prefix + "0";
		adders_row.emplace_back(make_shared<HalfAdder>(row_name));

		for (size_t a = 1; a < num_adders_per_level; ++a) {
			row_name = row_name_prefix + to_string(a);
			adders_row.emplace_back(make_shared<FullAdder>(row_name));
		}

		adders.emplace_back(adders_row);
		adders_row.clear();
	}

	// Connect the O outputs of the full adders.
	for (size_t y = 0; y < (num_adder_levels - 1); ++y) {
		row_name_prefix = name + "_S_" + to_string(y);

		for (size_t x = 1; x < num_adders_per_level; ++x) {
			row_name = row_name_prefix + '_' + to_string(x) + "_O";

			const auto wire = make_shared<Wire>(row_name);
			adders[y][x]->Connect(PORTS::O, wire);
			adders[y+1][x-1]->Connect(PORTS::A, wire);
			internal_wires.emplace_back(wire);
		}
	}

	// Create the AND gates that connect to the inputs of the adders.
	vector<and_t> ands_row;

	// First three levels of AND gates are slightly different, so handle
	// it separately. The first AND gate is directly connected to bit 0
	// of the result.
	row_name_prefix = name + "_AND_0_";
	ands_row.emplace_back(make_shared<And>(row_name_prefix + '0'));

	// Level 0
	for (size_t a = 1; a < num_ands_per_level; ++a) {
		row_name = row_name_prefix + to_string(a);

		const auto and_gate = make_shared<And>(row_name);
		const auto and_wire = make_shared<Wire>(row_name + "_O");
		and_gate->Connect(PORTS::O, and_wire);
		adders[0][a - 1]->Connect(PORTS::A, and_wire);
		ands_row.emplace_back(and_gate);

		internal_wires.emplace_back(and_wire);
	}
	ands.emplace_back(ands_row);
	ands_row.clear();

	// Level 1
	row_name_prefix = name + "_AND_1_";
	for (size_t a = 0; a < num_ands_per_level; ++a) {
		row_name = row_name_prefix + to_string(a);

		const auto and_gate = make_shared<And>(row_name);
		const auto and_wire = make_shared<Wire>(row_name + "_O");
		and_gate->Connect(PORTS::O, and_wire);
		adders[0][a]->Connect(PORTS::B, and_wire);
		ands_row.emplace_back(and_gate);

		internal_wires.emplace_back(and_wire);
	}
	ands.emplace_back(ands_row);
	ands_row.clear();

	// Level 2
	row_name_prefix = name + "_AND_2_";
	{
		size_t x = 1;
		size_t y = 0;

		for (size_t a = 0; a < num_ands_per_level; ++a) {
			row_name = row_name_prefix + to_string(a);

			const auto and_gate = make_shared<And>(row_name);
			const auto and_wire = make_shared<Wire>(row_name + "_O");
			and_gate->Connect(PORTS::O, and_wire);

			if (x == num_adders_per_level) {
				x = (num_adders_per_level - 1);
				y++;
			}

			if (x == (num_adders_per_level - 1)) {
				adders[y][x++]->Connect(PORTS::A, and_wire);
			} else {
				adders[y][x++]->Connect(PORTS::Cin, and_wire);
			}
			ands_row.emplace_back(and_gate);

			internal_wires.emplace_back(and_wire);
		}
	}
	ands.emplace_back(ands_row);
	ands_row.clear();

	// Now handle the rest of the AND gate levels.
	for (size_t b = 3; b < num_and_levels; ++b) {
		row_name_prefix = name + "_AND_" + to_string(b) + '_';

		size_t x = 1;		// Starting x position in the row.
		size_t y = b - 2;	// Starting y position is level - 2.

		for (size_t a = 0; a < num_ands_per_level; ++a) {
			row_name = row_name_prefix + to_string(a);

			const auto and_gate = make_shared<And>(row_name);
			const auto and_wire = make_shared<Wire>(row_name + "_O");
			and_gate->Connect(PORTS::O, and_wire);

			if (x == num_adders_per_level) {
				x = (num_adders_per_level - 1);
				y++;
			}

			if (a >= (num_adders_per_level - 1)) {
				adders[y][x++]->Connect(PORTS::A, and_wire);
			} else {
				adders[y][x++]->Connect(PORTS::Cin, and_wire);
			}
			ands_row.emplace_back(and_gate);

			internal_wires.emplace_back(and_wire);
		}

		ands.emplace_back(ands_row);
		ands_row.clear();
	}

	// Create the Cout connections for each adder level except the last.
	for (size_t b = 0; b < (num_adder_levels - 1); ++b) {
		row_name_prefix = name + "_Cout_" + to_string(b) + '_';

		for (size_t a = 0; a < num_adders_per_level; ++a) {
			row_name = row_name_prefix + to_string(a);

			const auto wire = make_shared<Wire>(row_name);
			adders[b][a]->Connect(PORTS::Cout, wire);
			adders[b + 1][a]->Connect(PORTS::B, wire);

			internal_wires.emplace_back(wire);
		}
	}

	// Create the Cout connections for the last level.
	row_name_prefix = name + "_Cout_" + to_string(num_adders_per_level - 1) + '_';
	for (size_t a = 0; a < (num_adders_per_level - 1); ++a) {
		row_name = row_name_prefix + to_string(a);

		const auto wire = make_shared<Wire>(row_name);
		adders.back()[a]->Connect(PORTS::Cout, wire);
		adders.back()[a + 1]->Connect(PORTS::Cin, wire);

		internal_wires.emplace_back(wire);
	}
}
