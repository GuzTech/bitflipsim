#include "main.h"

/*
  Twos-complement multiplier implementation.
*/

Multiplier_2C::Multiplier_2C(string _name,
							 size_t _num_bits_A,
							 size_t _num_bits_B,
							 MUL_TYPE _type)
	: Component(_name)
	, num_bits_A(_num_bits_A)
	, num_bits_B(_num_bits_B)
	, num_bits_O(num_bits_A + num_bits_B)
	, num_adder_levels(num_bits_B - 1)
	, num_and_levels(num_bits_B)
	, num_adders_per_level(num_bits_A)
	, num_ands_per_level(num_bits_A)
	, type(_type)
{
	if (num_bits_A == 0) {
		Error("Size of port A of Multiplier_2C \"" + _name + "\" must be larger than zero.\n");
	} else if (num_bits_B == 0) {
		Error("Size of port B of Multiplier_2C \"" + _name + "\" must be larger than zero.\n");
	}

	// Generate the requested hardware.
	switch (type) {
	case MUL_TYPE::CARRY_PROPAGATE_SIGN_EXTEND:		GenerateCarryPropagateSignExtendHardware(); break;
	case MUL_TYPE::CARRY_PROPAGATE_INVERSION: 		GenerateCarryPropagateInversionHardware(); break;
		//case MUL_TYPE::CARRY_PROPAGATE_BAUGH_WOOLEY:	GenerateCarryPropagateBaughWooleyHardware(); break;
	case MUL_TYPE::CARRY_SAVE_SIGN_EXTEND: 			GenerateCarrySaveSignExtendHardware(); break;
	case MUL_TYPE::CARRY_SAVE_INVERSION:   			GenerateCarrySaveInversionHardware(); break;
	case MUL_TYPE::CARRY_SAVE_BAUGH_WOOLEY:
		num_adder_levels++;
		num_adders_per_level--;
		GenerateCarrySaveBaughWooleyHardware();
		break;
	default:
		Error("Unknown type supplied for generating Multiplier_2C \"" + name + "\".\n");
	}
}

void Multiplier_2C::Update(bool propagating) {
	if (needs_update || !propagating) {
		switch (type) {
		case MUL_TYPE::CARRY_PROPAGATE_SIGN_EXTEND:
		case MUL_TYPE::CARRY_SAVE_SIGN_EXTEND:
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
			break;
		case MUL_TYPE::CARRY_SAVE_BAUGH_WOOLEY:
			for (size_t i = 0; i < longest_path; ++i) {
				for (const auto &notA : input_nots_A) {
					notA->Update(propagating);
				}
				for (const auto &notB : input_nots_B) {
					notB->Update(propagating);
				}
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
			break;
		case MUL_TYPE::CARRY_PROPAGATE_INVERSION:
		case MUL_TYPE::CARRY_SAVE_INVERSION:
			for (size_t i = 0; i < longest_path; ++i) {
				different_sign->Update(propagating);
				
				for (const auto &xor_A : input_2C_xors_A) {
					xor_A->Update(propagating);
				}
				for (const auto &xor_B : input_2C_xors_B) {
					xor_B->Update(propagating);
				}
				for (const auto &add_A : input_2C_adders_A) {
					add_A->Update(propagating);
				}
				for (const auto &add_B : input_2C_adders_B) {
					add_B->Update(propagating);
				}
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
				for (const auto &xor_o : output_2C_xors) {
					xor_o->Update(propagating);
				}
				for (const auto &add_o : output_2C_adders) {
					add_o->Update(propagating);
				}

				output_2C_adder_xor->Update(propagating);
			}
			break;
		default:
			Error("Multiplier not implemented yet!\n");
			break;
		}

		needs_update = false;
	}
}

void Multiplier_2C::Connect(PORTS port, const wire_t &wire, size_t index) {
	if (port == PORTS::A && index >= num_bits_A) {
		Error("Index " + to_string(index) + " of port A is out of bounds for Multiplier_2C \"" + name + "\".\n");
	} else if (port == PORTS::B && index >= num_bits_B) {
		Error("Index " + to_string(index) + " of port B is out of bounds for Multiplier_2C \"" + name + "\".\n");
	} else if (port == PORTS::O && index >= num_bits_O) {
		Error("Index " + to_string(index) + " of port O is out of bounds for Multiplier_2C \"" + name + "\".\n");
	}

	auto error_undefined_port = [&](const auto &wire) {
		Error("Trying to connect wire \"" + wire->GetName() + "\" to undefined port of Multiplier_2C \"" + name + "\".\n");
	};

	switch (type) {
	case MUL_TYPE::CARRY_PROPAGATE_SIGN_EXTEND:
	case MUL_TYPE::CARRY_SAVE_SIGN_EXTEND:
		switch (port) {
		case PORTS::A:
			// Connect to the AND gates at the given index for
			// each level of AND gates.
			for (size_t level = 0; level < num_bits_B; ++level) {
				ands[level][index]->Connect(PORTS::A, wire);
			}
			input_wires.emplace_back(wire);
			break;
		case PORTS::B:
			for (size_t i = 0; i < num_ands_per_level; ++i) {
				ands[index][i]->Connect(PORTS::B, wire);
			}
			input_wires.emplace_back(wire);
			break;
		case PORTS::O:
			if (index == 0) {
				ands[0][0]->Connect(PORTS::O, wire);
			} else {
				adders[index - 1][0]->Connect(PORTS::O, wire);
			}
			output_wires.emplace_back(wire);
			break;
		default:
			error_undefined_port(wire);
		}
		break;
	case MUL_TYPE::CARRY_SAVE_BAUGH_WOOLEY:
		switch (port) {
		case PORTS::A:
			input_nots_A[index]->Connect(PORTS::I, wire);

			for (size_t level = 0; level < (num_adder_levels - 1); ++level) {
				ands[level][index]->Connect(PORTS::A, wire);
			}
			if (index == (num_bits_A - 1)) {
				ands.back().back()->Connect(PORTS::A, wire);
				adders.back()[0]->Connect(PORTS::B, wire);
			}
			input_wires.emplace_back(wire);
			break;
		case PORTS::B:
			input_nots_B[index]->Connect(PORTS::I, wire);

			if (index < (num_bits_B - 1)) {
				for (size_t i = 0; i < (num_bits_A - 1); ++i) {
					ands[index][i]->Connect(PORTS::B, wire);
				}
			} else {
				for (size_t i = 0; i < num_bits_A; ++i) {
					ands[index][i]->Connect(PORTS::B, wire);
				}
				adders.back()[0]->Connect(PORTS::Cin, wire);
			}
			input_wires.emplace_back(wire);
			break;
		case PORTS::O:
			if (index == 0) {
				ands[0][0]->Connect(PORTS::O, wire);
			} else if (index < (num_bits_A - 1)) {
				adders[index - 1][0]->Connect(PORTS::O, wire);
			} else {
				adders.back()[index - (num_bits_A - 1)]->Connect(PORTS::O, wire);
			}
			output_wires.emplace_back(wire);
			break;
		default:
			error_undefined_port(wire);
		}
		break;
	case MUL_TYPE::CARRY_PROPAGATE_INVERSION:
	case MUL_TYPE::CARRY_SAVE_INVERSION:
		switch (port) {
		case PORTS::A:
			if (index < (num_bits_A - 1)) {
				input_2C_xors_A[index]->Connect(PORTS::A, wire);
			} else {
				for (size_t i = 0; i < (num_bits_A - 1); ++i) {
					input_2C_xors_A[i]->Connect(PORTS::B, wire);
				}

				input_2C_adders_A[0]->Connect(PORTS::B, wire);
				different_sign->Connect(PORTS::A, wire);
			}
			input_wires.emplace_back(wire);
			break;
		case PORTS::B:
			if (index < (num_bits_B - 1)) {
				input_2C_xors_B[index]->Connect(PORTS::A, wire);
			} else {
				for (size_t i = 0; i < (num_bits_B - 1); ++i) {
					input_2C_xors_B[i]->Connect(PORTS::B, wire);
				}

				input_2C_adders_B[0]->Connect(PORTS::B, wire);
				different_sign->Connect(PORTS::B, wire);
			}
			input_wires.emplace_back(wire);
			break;
		case PORTS::O:
			if (index == (num_bits_O - 1)) {
				output_2C_adder_xor->Connect(PORTS::O, wire);
			} else {
				output_2C_adders[index]->Connect(PORTS::O, wire);
			}
			output_wires.emplace_back(wire);
			break;
		default:
			error_undefined_port(wire);
		}
		break;
	default:
		Error("Multiplier not implemented yet!\n");
		break;
	}
}

void Multiplier_2C::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		Error("Wire bundle \"" + wires->GetName() + " accessed with index " + to_string(wire_idx)
			  + " but has size " + to_string(wires->GetSize()) + ".\n");
	}

	const wire_t &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const wire_t Multiplier_2C::GetWire(PORTS port, size_t index) const {
	return nullptr;
}

const PORT_DIR Multiplier_2C::GetPortDirection(PORTS port) const {
	switch (port) {
	case PORTS::A:
	case PORTS::B:
		return PORT_DIR::INPUT;
	case PORTS::O:
		return PORT_DIR::OUTPUT;
	default:
		Error("Trying to get port direction of undefined port in Multiplier_2C \"" + name + "\".\n");
	}
}

// Generates the standard sign-extension multiplier with carry propagation.
void Multiplier_2C::GenerateCarryPropagateSignExtendHardware() {
	// Names for the adders.
	string row_name_prefix = string(name) + "_S_0_";
	string row_name = row_name_prefix + "0";

	vector<comp_t> adders_row;

	// All rows consist of #(O - level - 1) FullAdders and 1 HalfAdder.
	for (size_t b = 0; b < (num_bits_O - 1); ++b) {
		row_name_prefix = name + "_S_" + to_string(b) + "_";
		row_name = row_name_prefix + "0";
		adders_row.emplace_back(make_shared<HalfAdder>(row_name));

		for (size_t a = 1; a < (num_bits_O - b - 1); ++a) {
			row_name = row_name_prefix + to_string(a);
			adders_row.emplace_back(make_shared<FullAdder>(row_name));
		}

		adders.emplace_back(adders_row);
		adders_row.clear();
	}

	// Connect the O outputs of the full adders.
	for (size_t y = 0; y < (num_bits_O - 2); ++y) {
		row_name_prefix = name + "_S_" + to_string(y);

		for (size_t x = 1; x < (num_bits_O - y - 1); ++x) {
			row_name = row_name_prefix + "_" + to_string(x) + "_O";

			const auto wire = make_shared<Wire>(row_name);
			adders[y][x]->Connect(PORTS::O, wire);
			adders[y + 1][x - 1]->Connect(PORTS::A, wire);
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

	for (size_t a = 1; a < num_bits_A; ++a) {
		row_name = row_name_prefix + to_string(a);

		const auto and_gate = make_shared<And>(row_name);
		const auto and_wire = make_shared<Wire>(row_name + "_O");
		and_gate->Connect(PORTS::O, and_wire);
		adders[0][a - 1]->Connect(PORTS::A, and_wire);

		if (a == (num_bits_A - 1)) {
			for (size_t i = (num_bits_A - 1); i < (num_bits_O - 1); ++i) {
				adders[0][i]->Connect(PORTS::A, and_wire);
			}
		}

		ands_row.emplace_back(and_gate);

		internal_wires.emplace_back(and_wire);
	}
	ands.emplace_back(ands_row);
	ands_row.clear();

	// Now handle the rest of the AND gate levels.
	for (size_t b = 1; b < num_bits_B; ++b) {
		row_name_prefix = name + "_AND_" + to_string(b) + "_";

		for (size_t a = 0; a < num_ands_per_level; ++a) {
			row_name = row_name_prefix + to_string(a);

			const auto and_gate = make_shared<And>(row_name);
			const auto and_wire = make_shared<Wire>(row_name + "_O");
			and_gate->Connect(PORTS::O, and_wire);
			adders[b-1][a]->Connect(PORTS::B, and_wire);

			if (a == (num_bits_A - 1)) {
				for (size_t i = num_bits_A; i < (num_bits_O - b); ++i) {
					adders[b - 1][i]->Connect(PORTS::B, and_wire);
				}
			}

			if (b == (num_bits_B - 1)) {
				for (size_t i = num_bits_B; i < (num_bits_O - a); ++i) {
					adders[i - 1][a]->Connect(PORTS::B, and_wire);
				}
			}

			ands_row.emplace_back(and_gate);

			internal_wires.emplace_back(and_wire);
		}

		ands.emplace_back(ands_row);
		ands_row.clear();
	}

	// Create the connections between Cout and Cin of the adders.
	for (size_t b = 0; b < (num_bits_O - 2); ++b) {
		row_name_prefix = name + "_Cout_" + to_string(b) + "_";

		for (size_t a = 0; a < (num_bits_O - b - 2); ++a) {
			row_name = row_name_prefix + to_string(a);

			const auto wire = make_shared<Wire>(row_name);
			adders[b][a]->Connect(PORTS::Cout, wire);
			adders[b][a+1]->Connect(PORTS::Cin, wire);
			internal_wires.emplace_back(wire);
		}
	}
}

void Multiplier_2C::GenerateCarrySaveSignExtendHardware() {
	// Names for the adders.
	string row_name_prefix = string(name) + "_S_0_";
	string row_name = row_name_prefix + "0";

	vector<comp_t> adders_row;

	// All rows consist of (num_bits_O - level - 1) FullAdders and 1 HalfAdder.
	for (size_t b = 0; b < (num_bits_O - 1); ++b) {
		row_name_prefix = name + "_S_" + to_string(b) + "_";
		row_name = row_name_prefix + "0";
		adders_row.emplace_back(make_shared<HalfAdder>(row_name));

		for (size_t a = 1; a < (num_bits_O - b - 1); ++a) {
			row_name = row_name_prefix + to_string(a);
			adders_row.emplace_back(make_shared<FullAdder>(row_name));
		}

		adders.emplace_back(adders_row);
		adders_row.clear();
	}

	// Connect the O outputs of the full adders.
	for (size_t y = 0; y < (num_bits_O - 2); ++y) {
		row_name_prefix = name + "_S_" + to_string(y);

		for (size_t x = 1; x < (num_bits_O - y - 1); ++x) {
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

	// Level 0
	for (size_t a = 1; a < num_bits_A; ++a) {
		row_name = row_name_prefix + to_string(a);

		const auto and_gate = make_shared<And>(row_name);
		const auto and_wire = make_shared<Wire>(row_name + "_O");
		and_gate->Connect(PORTS::O, and_wire);
		adders[0][a - 1]->Connect(PORTS::A, and_wire);

		if (a == (num_bits_A - 1)) {
			for (size_t i = (num_bits_A - 1); i < (num_bits_O - 1); ++i) {
				adders[0][i]->Connect(PORTS::A, and_wire);
			}
		}

		ands_row.emplace_back(and_gate);

		internal_wires.emplace_back(and_wire);
	}
	ands.emplace_back(ands_row);
	ands_row.clear();

	// Level 1
	row_name_prefix = name + "_AND_1_";
	for (size_t a = 0; a < num_bits_A; ++a) {
		row_name = row_name_prefix + to_string(a);

		const auto and_gate = make_shared<And>(row_name);
		const auto and_wire = make_shared<Wire>(row_name + "_O");
		and_gate->Connect(PORTS::O, and_wire);
		adders[0][a]->Connect(PORTS::B, and_wire);

		if (a == (num_bits_A - 1)) {
			for (size_t i = num_bits_A; i < (num_bits_O - 1); ++i) {
				adders[0][i]->Connect(PORTS::B, and_wire);
			}
		}

		ands_row.emplace_back(and_gate);

		internal_wires.emplace_back(and_wire);
	}
	ands.emplace_back(ands_row);
	ands_row.clear();

	// Level 2
	row_name_prefix = name + "_AND_2_";
	for (size_t a = 0; a < num_bits_A; ++a) {
		row_name = row_name_prefix + to_string(a);

		const auto and_gate = make_shared<And>(row_name);
		const auto and_wire = make_shared<Wire>(row_name + "_O");
		and_gate->Connect(PORTS::O, and_wire);
		adders[0][a + 1]->Connect(PORTS::Cin, and_wire);

		if (a == (num_bits_A - 1)) {
			for (size_t i = (num_bits_A + 1); i < (num_bits_O - 1); ++i) {
				adders[0][i]->Connect(PORTS::Cin, and_wire);
			}
		}

		ands_row.emplace_back(and_gate);

		internal_wires.emplace_back(and_wire);
	}
	ands.emplace_back(ands_row);
	ands_row.clear();

	// Now handle the rest of the AND gate levels.
	for (size_t b = 3; b < num_bits_B; ++b) {
		row_name_prefix = name + "_AND_" + to_string(b) + "_";

		for (size_t a = 0; a < num_ands_per_level; ++a) {
			row_name = row_name_prefix + to_string(a);

			const auto and_gate = make_shared<And>(row_name);
			const auto and_wire = make_shared<Wire>(row_name + "_O");
			and_gate->Connect(PORTS::O, and_wire);
			adders[b - 2][a + 1]->Connect(PORTS::Cin, and_wire);

			if (a == (num_bits_A - 1)) {
				for (size_t i = num_bits_A; i < (num_bits_O - b); ++i) {
					adders[b - 2][i + 1]->Connect(PORTS::Cin, and_wire);
				}
			}

			if (b == (num_bits_B - 1)) {
				for (size_t i = num_bits_B; i < (num_bits_O - a); ++i) {
					adders[i - 2][a + 1]->Connect(PORTS::Cin, and_wire);
				}
			}

			ands_row.emplace_back(and_gate);

			internal_wires.emplace_back(and_wire);
		}

		ands.emplace_back(ands_row);
		ands_row.clear();
	}

	// Create the connections between Cout and Cin of the adders.
	for (size_t b = 0; b < (num_bits_O - 2); ++b) {
		row_name_prefix = name + "_Cout_" + to_string(b) + "_";

		for (size_t a = 0; a < (num_bits_O - b - 2); ++a) {
			row_name = row_name_prefix + to_string(a);

			const auto wire = make_shared<Wire>(row_name);
			adders[b][a]->Connect(PORTS::Cout, wire);
			adders[b + 1][a]->Connect(PORTS::B, wire);
			internal_wires.emplace_back(wire);
		}
	}
}

// This generates hardware that has a twos-complement operator for
// both A and B inputs and also the output. Effectively, this ensures
// that we always multiply two positive numbers, and adjust the output
// if the signs of A and B are different.
void Multiplier_2C::GenerateCarryPropagateInversionHardware() {
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
	for (size_t y = 0; y < num_adder_levels - 1; ++y) {
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

	// Names for the 2C converters;
	string conv_name_prefix = name + "_in_2C_XOR_A_";
	string conv_name;

	/*
	 * Twos-complement hardware for input A.
	 */
	// Create the XOR gates for the A input.
	for (size_t i = 0; i < num_bits_A - 1; ++i) {
		conv_name = conv_name_prefix + to_string(i);
		const auto inv_xor = make_shared<Xor>(conv_name);
		const auto wire = make_shared<Wire>(conv_name + "_O");
		inv_xor->Connect(PORTS::O, wire);

		input_2C_xors_A.emplace_back(inv_xor);
		internal_wires.emplace_back(wire);
	}

	// Create the half adders for the A input.
	conv_name_prefix = name + "_in_2C_ha_A_";
	for (size_t i = 0; i < num_bits_A - 1; ++i) {
		conv_name = conv_name_prefix + to_string(i);
		const auto inv_ha = make_shared<HalfAdder>(conv_name);
		const auto &wire = input_2C_xors_A[i]->GetWire(PORTS::O);
		inv_ha->Connect(PORTS::A, wire);

		// Create and connect a wire to the output port of the
		// half adders.
		const auto out_wire = make_shared<Wire>(conv_name + "_O");
		inv_ha->Connect(PORTS::O, out_wire);

		// Connect the output wire to all the A(i) input at each level.
		for (size_t level = 0; level < num_and_levels; ++level) {
			ands[level][i]->Connect(PORTS::A, out_wire);
		}

		input_2C_adders_A.emplace_back(inv_ha);
		internal_wires.emplace_back(out_wire);
	}

	// Connect the carry out of the half adders to the
	// B input of the next half adder.
	for (size_t i = 0; i < num_bits_A - 1; ++i) {
		conv_name = conv_name_prefix + to_string(i) + "_Cout";
		const auto wire = make_shared<Wire>(conv_name);
		input_2C_adders_A[i]->Connect(PORTS::Cout, wire);

		if (i != (num_bits_A - 2)) {
			// Connect the carry out to the B port of the next half adder.
			input_2C_adders_A[i+1]->Connect(PORTS::B, wire);
		} else {
			// The carry out of the last half adder connects to the
			// last AND gate of each level.
			for (size_t level = 0; level < num_and_levels; ++level) {
				ands[level][i + 1]->Connect(PORTS::A, wire);
			}
		}

		internal_wires.emplace_back(wire);
	}

	/*
	 * Twos-complement hardware for input B.
	 */
	// Create the XOR gates for the B input.
	conv_name_prefix = name + "_in_2C_XOR_B_";
	for (size_t i = 0; i < num_bits_B - 1; ++i) {
		conv_name = conv_name_prefix + to_string(i);
		const auto inv_xor = make_shared<Xor>(conv_name);
		const auto wire = make_shared<Wire>(conv_name + "_O");
		inv_xor->Connect(PORTS::O, wire);

		input_2C_xors_B.emplace_back(inv_xor);
		internal_wires.emplace_back(wire);
	}

	// Create the half adders for the B input.
	conv_name_prefix = name + "_in_2C_ha_B_";
	for (size_t i = 0; i < num_bits_B - 1; ++i) {
		conv_name = conv_name_prefix + to_string(i);
		const auto inv_ha = make_shared<HalfAdder>(conv_name);
		const auto &wire = input_2C_xors_B[i]->GetWire(PORTS::O);
		inv_ha->Connect(PORTS::A, wire);

		// Create and connect a wire to the output port of the
		// half adders.
		const auto out_wire = make_shared<Wire>(conv_name + "_O");
		inv_ha->Connect(PORTS::O, out_wire);

		// Connect the output wire to all AND gates at level i.
		for (size_t a = 0; a < num_ands_per_level; ++a) {
			ands[i][a]->Connect(PORTS::B, out_wire);
		}

		input_2C_adders_B.emplace_back(inv_ha);
		internal_wires.emplace_back(out_wire);
	}

	// Connect the carry out of the half adders to the
	// B input of the next half adder.
	for (size_t i = 0; i < num_bits_B - 1; ++i) {
		conv_name = conv_name_prefix + to_string(i) + "_Cout";
		const auto cout_wire = make_shared<Wire>(conv_name);
		input_2C_adders_B[i]->Connect(PORTS::Cout, cout_wire);

		if (i != (num_bits_B - 2)) {
			// Connect the carry out to the B port of the next half adder.
			input_2C_adders_B[i+1]->Connect(PORTS::B, cout_wire);
		} else {
			// The carry out of the last half adder connects to all
			// AND gates at the last level.
			for (size_t a = 0; a < num_ands_per_level; ++a) {
				ands[i + 1][a]->Connect(PORTS::B, cout_wire);
			}
		}

		internal_wires.emplace_back(cout_wire);
	}

	/*
	 * Twos-complement hardware for the output.
	 */
	// Create the XOR gates for the output.
	conv_name_prefix = name + "_out_2C_XOR_";
	for (size_t i = 0; i < num_bits_O; ++i) {
		conv_name = conv_name_prefix + to_string(i);
		const auto inv_xor = make_shared<Xor>(conv_name);
		const auto inv_wire = make_shared<Wire>(conv_name + "_O");
		inv_xor->Connect(PORTS::O, inv_wire);

		output_2C_xors.emplace_back(inv_xor);
		internal_wires.emplace_back(inv_wire);
	}

	// Create the half adders for the output.
	conv_name_prefix = name + "_out_2C_ha_";
	for (size_t i = 0; i < num_bits_O - 1; ++i) {
		conv_name = conv_name_prefix + to_string(i);
		const auto inv_ha = make_shared<HalfAdder>(conv_name);
		const auto &xor_wire = output_2C_xors[i]->GetWire(PORTS::O);
		inv_ha->Connect(PORTS::A, xor_wire);
		output_2C_adders.emplace_back(inv_ha);
	}

	// We create one more XOR gate than half adders, because
	// instead of an extra half adder, we only need an XOR gate.
	output_2C_adder_xor = make_shared<Xor>(conv_name_prefix + "XOR");
	const auto &xor_wire = output_2C_xors.back()->GetWire(PORTS::O);
	output_2C_adder_xor->Connect(PORTS::B, xor_wire);

	// Connect the carry out of the half adders to the
	// B input of the next half adder.
	for (size_t i = 0; i < num_bits_O - 2; ++i) {
		conv_name = conv_name_prefix + to_string(i) + "_Cout";
		const auto cout_wire = make_shared<Wire>(conv_name);
		output_2C_adders[i]->Connect(PORTS::Cout, cout_wire);
		output_2C_adders[i+1]->Connect(PORTS::B, cout_wire);
		internal_wires.emplace_back(cout_wire);
	}

	// Connect the carry out of the last half adder to
	// the A input of the output adder XOR gate.
	auto wire = make_shared<Wire>(
		conv_name_prefix + to_string(num_bits_O - 2) + "_Cout");
	output_2C_adders.back()->Connect(PORTS::Cout, wire);
	output_2C_adder_xor->Connect(PORTS::A, wire);
	internal_wires.emplace_back(wire);

	// Connect the XOR gates to the last level of adders.
	conv_name_prefix = name + "_in_2C_XOR_";
	const auto &last_row = adders.back();

	for (size_t i = 0; i < num_bits_O - 1; ++i) {
		conv_name = conv_name_prefix + to_string(i);
		const auto xor_wire = make_shared<Wire>(conv_name);

		if (i == (num_bits_O - 1)) {
			last_row.back()->Connect(PORTS::Cout, xor_wire);
		} else if (i > (num_bits_O - num_adders_per_level - 1)) {
			last_row[i - num_adders_per_level + 1]->Connect(PORTS::O, xor_wire);
		} else if (i == 0) {
			// Index zero is the output of the first AND gate.
			ands[0][0]->Connect(PORTS::O, xor_wire);
		} else {
			adders[i - 1][0]->Connect(PORTS::O, xor_wire);
		}
		output_2C_xors[i]->Connect(PORTS::A, xor_wire);
		internal_wires.emplace_back(xor_wire);
	}

	// Connect the carry out of the last adder of the last
	// level to the A input of the output 2C adder XOR gate.
	wire = make_shared<Wire>(conv_name_prefix + to_string(num_bits_O - 1));
	last_row.back()->Connect(PORTS::Cout, wire);
	output_2C_xors.back()->Connect(PORTS::A, wire);
	internal_wires.emplace_back(wire);

	// Connect the output of the different_sign XOR gate to the
	// output 2C XOR gates and the first output 2C half
	// adder B input.
	different_sign = make_shared<Xor>(name + "_different_sign");
	wire = make_shared<Wire>(name + "_different_sign_O");
	different_sign->Connect(PORTS::O, wire);

	for (const auto &x : output_2C_xors) {
		x->Connect(PORTS::B, wire);
	}
	output_2C_adders.front()->Connect(PORTS::B, wire);
	internal_wires.emplace_back(wire);
}

void Multiplier_2C::GenerateCarrySaveInversionHardware() {
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

	// The rest of the rows consist of (num_bits_A - 1) FullAdders and 1 HalfAdder.
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

	// First three levels of AND gates are slightly different, so handle
	// it separately. The first AND gate is directly connected to bit 0
	// of the result.
	row_name_prefix = name + "_AND_0_";
	ands_row.emplace_back(make_shared<And>(row_name_prefix + "0"));

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
		row_name_prefix = name + "_AND_" + to_string(b) + "_";

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
		row_name_prefix = name + "_Cout_" + to_string(b) + "_";

		for (size_t a = 0; a < num_adders_per_level; ++a) {
			row_name = row_name_prefix + to_string(a);

			const auto wire = make_shared<Wire>(row_name);
			adders[b][a]->Connect(PORTS::Cout, wire);
			adders[b + 1][a]->Connect(PORTS::B, wire);

			internal_wires.emplace_back(wire);
		}
	}

	// Create the Cout connections for the last level.
	row_name_prefix = name + "_Cout_" + to_string(num_adders_per_level - 1) + "_";
	for (size_t a = 0; a < (num_adders_per_level - 1); ++a) {
		row_name = row_name_prefix + to_string(a);

		const auto wire = make_shared<Wire>(row_name);
		adders.back()[a]->Connect(PORTS::Cout, wire);
		adders.back()[a + 1]->Connect(PORTS::Cin, wire);

		internal_wires.emplace_back(wire);
	}

	// Names for the 2C converters;
	string conv_name_prefix = name + "_in_2C_XOR_A_";
	string conv_name;

	/*
	 * Twos-complement hardware for input A.
	 */
	// Create the XOR gates for the A input.
	for (size_t i = 0; i < num_bits_A - 1; ++i) {
		conv_name = conv_name_prefix + to_string(i);
		const auto inv_xor = make_shared<Xor>(conv_name);
		const auto wire = make_shared<Wire>(conv_name + "_O");
		inv_xor->Connect(PORTS::O, wire);

		input_2C_xors_A.emplace_back(inv_xor);
		internal_wires.emplace_back(wire);
	}

	// Create the half adders for the A input.
	conv_name_prefix = name + "_in_2C_ha_A_";
	for (size_t i = 0; i < num_bits_A - 1; ++i) {
		conv_name = conv_name_prefix + to_string(i);
		const auto inv_ha = make_shared<HalfAdder>(conv_name);
		const auto &wire = input_2C_xors_A[i]->GetWire(PORTS::O);
		inv_ha->Connect(PORTS::A, wire);

		// Create and connect a wire to the output port of the
		// half adders.
		const auto out_wire = make_shared<Wire>(conv_name + "_O");
		inv_ha->Connect(PORTS::O, out_wire);

		// Connect the output wire to all the A(i) input at each level.
		for (size_t level = 0; level < num_and_levels; ++level) {
			ands[level][i]->Connect(PORTS::A, out_wire);
		}

		input_2C_adders_A.emplace_back(inv_ha);
		internal_wires.emplace_back(out_wire);
	}

	// Connect the carry out of the half adders to the
	// B input of the next half adder.
	for (size_t i = 0; i < num_bits_A - 1; ++i) {
		conv_name = conv_name_prefix + to_string(i) + "_Cout";
		const auto wire = make_shared<Wire>(conv_name);
		input_2C_adders_A[i]->Connect(PORTS::Cout, wire);

		if (i != (num_bits_A - 2)) {
			// Connect the carry out to the B port of the next half adder.
			input_2C_adders_A[i+1]->Connect(PORTS::B, wire);
		} else {
			// The carry out of the last half adder connects to the
			// last AND gate of each level.
			for (size_t level = 0; level < num_and_levels; ++level) {
				ands[level][i + 1]->Connect(PORTS::A, wire);
			}
		}

		internal_wires.emplace_back(wire);
	}

	/*
	 * Twos-complement hardware for input B.
	 */
	// Create the XOR gates for the B input.
	conv_name_prefix = name + "_in_2C_XOR_B_";
	for (size_t i = 0; i < num_bits_B - 1; ++i) {
		conv_name = conv_name_prefix + to_string(i);
		const auto inv_xor = make_shared<Xor>(conv_name);
		const auto wire = make_shared<Wire>(conv_name + "_O");
		inv_xor->Connect(PORTS::O, wire);

		input_2C_xors_B.emplace_back(inv_xor);
		internal_wires.emplace_back(wire);
	}

	// Create the half adders for the B input.
	conv_name_prefix = name + "_in_2C_ha_B_";
	for (size_t i = 0; i < num_bits_B - 1; ++i) {
		conv_name = conv_name_prefix + to_string(i);
		const auto inv_ha = make_shared<HalfAdder>(conv_name);
		const auto &wire = input_2C_xors_B[i]->GetWire(PORTS::O);
		inv_ha->Connect(PORTS::A, wire);

		// Create and connect a wire to the output port of the
		// half adders.
		const auto out_wire = make_shared<Wire>(conv_name + "_O");
		inv_ha->Connect(PORTS::O, out_wire);

		// Connect the output wire to all AND gates at level i.
		for (size_t a = 0; a < num_ands_per_level; ++a) {
			ands[i][a]->Connect(PORTS::B, out_wire);
		}

		input_2C_adders_B.emplace_back(inv_ha);
		internal_wires.emplace_back(out_wire);
	}

	// Connect the carry out of the half adders to the
	// B input of the next half adder.
	for (size_t i = 0; i < num_bits_B - 1; ++i) {
		conv_name = conv_name_prefix + to_string(i) + "_Cout";
		const auto cout_wire = make_shared<Wire>(conv_name);
		input_2C_adders_B[i]->Connect(PORTS::Cout, cout_wire);

		if (i != (num_bits_B - 2)) {
			// Connect the carry out to the B port of the next half adder.
			input_2C_adders_B[i+1]->Connect(PORTS::B, cout_wire);
		} else {
			// The carry out of the last half adder connects to all
			// AND gates at the last level.
			for (size_t a = 0; a < num_ands_per_level; ++a) {
				ands[i + 1][a]->Connect(PORTS::B, cout_wire);
			}
		}

		internal_wires.emplace_back(cout_wire);
	}

	/*
	 * Twos-complement hardware for the output.
	 */
	// Create the XOR gates for the output.
	conv_name_prefix = name + "_out_2C_XOR_";
	for (size_t i = 0; i < num_bits_O; ++i) {
		conv_name = conv_name_prefix + to_string(i);
		const auto inv_xor = make_shared<Xor>(conv_name);
		const auto inv_wire = make_shared<Wire>(conv_name + "_O");
		inv_xor->Connect(PORTS::O, inv_wire);

		output_2C_xors.emplace_back(inv_xor);
		internal_wires.emplace_back(inv_wire);
	}

	// Create the half adders for the output.
	conv_name_prefix = name + "_out_2C_ha_";
	for (size_t i = 0; i < num_bits_O - 1; ++i) {
		conv_name = conv_name_prefix + to_string(i);
		const auto inv_ha = make_shared<HalfAdder>(conv_name);
		const auto &xor_wire = output_2C_xors[i]->GetWire(PORTS::O);
		inv_ha->Connect(PORTS::A, xor_wire);
		output_2C_adders.emplace_back(inv_ha);
	}

	// We create one more XOR gate than half adders, because
	// instead of an extra half adder, we only need an XOR gate.
	output_2C_adder_xor = make_shared<Xor>(conv_name_prefix + "XOR");
	const auto &xor_wire = output_2C_xors.back()->GetWire(PORTS::O);
	output_2C_adder_xor->Connect(PORTS::B, xor_wire);

	// Connect the carry out of the half adders to the
	// B input of the next half adder.
	for (size_t i = 0; i < num_bits_O - 2; ++i) {
		conv_name = conv_name_prefix + to_string(i) + "_Cout";
		const auto cout_wire = make_shared<Wire>(conv_name);
		output_2C_adders[i]->Connect(PORTS::Cout, cout_wire);
		output_2C_adders[i+1]->Connect(PORTS::B, cout_wire);
		internal_wires.emplace_back(cout_wire);
	}

	// Connect the carry out of the last half adder to
	// the A input of the output adder XOR gate.
	auto wire = make_shared<Wire>(
		conv_name_prefix + to_string(num_bits_O - 2) + "_Cout");
	output_2C_adders.back()->Connect(PORTS::Cout, wire);
	output_2C_adder_xor->Connect(PORTS::A, wire);
	internal_wires.emplace_back(wire);

	// Connect the XOR gates to the last level of adders.
	conv_name_prefix = name + "_in_2C_XOR_";
	const auto &last_row = adders.back();

	for (size_t i = 0; i < num_bits_O - 1; ++i) {
		conv_name = conv_name_prefix + to_string(i);
		const auto xor_wire = make_shared<Wire>(conv_name);

		if (i == (num_bits_O - 1)) {
			last_row.back()->Connect(PORTS::Cout, xor_wire);
		} else if (i > (num_bits_O - num_adders_per_level - 1)) {
			last_row[i - num_adders_per_level + 1]->Connect(PORTS::O, xor_wire);
		} else if (i == 0) {
			// Index zero is the output of the first AND gate.
			ands[0][0]->Connect(PORTS::O, xor_wire);
		} else {
			adders[i - 1][0]->Connect(PORTS::O, xor_wire);
		}
		output_2C_xors[i]->Connect(PORTS::A, xor_wire);
		internal_wires.emplace_back(xor_wire);
	}

	// Connect the carry out of the last adder of the last
	// level to the A input of the output 2C adder XOR gate.
	wire = make_shared<Wire>(conv_name_prefix + to_string(num_bits_O - 1));
	last_row.back()->Connect(PORTS::Cout, wire);
	output_2C_xors.back()->Connect(PORTS::A, wire);
	internal_wires.emplace_back(wire);

	// Connect the output of the different_sign XOR gate to the
	// output 2C XOR gates and the first output 2C half
	// adder B input.
	different_sign = make_shared<Xor>(name + "_different_sign");
	wire = make_shared<Wire>(name + "_different_sign_O");
	different_sign->Connect(PORTS::O, wire);

	for (const auto &x : output_2C_xors) {
		x->Connect(PORTS::B, wire);
	}
	output_2C_adders.front()->Connect(PORTS::B, wire);
	internal_wires.emplace_back(wire);
}

void Multiplier_2C::GenerateCarrySaveBaughWooleyHardware() {
	// Names for the adders.
	string row_name_prefix = string(name) + "_S_0_";
	string row_name = row_name_prefix + "0";

	// Create the inverted input signals.
	for (size_t a = 0; a < num_bits_A; ++a) {
		string not_name = 'A' + to_string(a) + "_n";

		const auto notA = make_shared<Not>(not_name);
		const auto wire = make_shared<Wire>(not_name + "_O");
		notA->Connect(PORTS::O, wire);

		input_nots_A.emplace_back(notA);
		internal_wires.emplace_back(wire);
	}
	for (size_t b = 0; b < num_bits_B; ++b) {
		string not_name = 'B' + to_string(b) + "_n";

		const auto notB = make_shared<Not>(not_name);
		const auto wire = make_shared<Wire>(not_name + "_O");
		notB->Connect(PORTS::O, wire);

		input_nots_B.emplace_back(notB);
		internal_wires.emplace_back(wire);
	}

	// First row consists of only HalfAdders.
	vector<comp_t> adders_row;

	for (size_t a = 0; a < num_adders_per_level; ++a) {
		row_name = row_name_prefix + to_string(a);

		adders_row.emplace_back(make_shared<HalfAdder>(row_name));
	}
	adders.emplace_back(adders_row);
	adders_row.clear();

	// The rest of the rows consist of only FullAdders,
	// and are regular until the last two rows.
	for (size_t b = 1; b < num_adder_levels; ++b) {
		row_name_prefix = name + "_S_" + to_string(b) + "_";

		for (size_t a = 0; a < num_adders_per_level; ++a) {
			row_name = row_name_prefix + to_string(a);
			adders_row.emplace_back(make_shared<FullAdder>(row_name));
		}

		if (b >= (num_adder_levels - 2)) {
			// Second to last row.
			adders_row.emplace_back(make_shared<FullAdder>(row_name_prefix + to_string(num_adders_per_level)));

			if (b >= (num_adder_levels - 1)) {
				// Last row.
				adders_row.emplace_back(make_shared<FullAdder>(row_name_prefix + to_string(num_adders_per_level + 1)));
			}
		}

		adders.emplace_back(adders_row);
		adders_row.clear();
	}

	// Connect the O outputs of the full adders.
	for (size_t y = 0; y < (num_adder_levels - 1); ++y) {
		row_name_prefix = name + "_S_" + to_string(y);

		if (y < (num_adder_levels - 2)) {
			for (size_t x = 1; x < num_adders_per_level; ++x) {
				row_name = row_name_prefix + '_' + to_string(x) + "_O";

				const auto wire = make_shared<Wire>(row_name);
				adders[y][x]->Connect(PORTS::O, wire);
				adders[y + 1][x - 1]->Connect(PORTS::A, wire);
				internal_wires.emplace_back(wire);
			}
		} else {
			for (size_t x = 0; x < (num_adders_per_level + 1); ++x) {
				row_name = row_name_prefix + '_' + to_string(x) + "_O";

				const auto wire = make_shared<Wire>(row_name);
				adders[y][x]->Connect(PORTS::O, wire);
				adders[y + 1][x]->Connect(PORTS::A, wire);
				internal_wires.emplace_back(wire);
			}
		}
	}

	// Create the AND gates that connect to the inputs of the adders.
	vector<and_t> ands_row;

	// First level of AND gates is slightly different, so handle it
	// separately. The first AND gate is directly connected to bit 0
	// of the result.
	row_name_prefix = name + "_AND_0_";
	ands_row.emplace_back(make_shared<And>(row_name_prefix + "0"));

	for (size_t a = 1; a < num_bits_A; ++a) {
		row_name = row_name_prefix + to_string(a);

		const auto and_gate = make_shared<And>(row_name);
		const auto and_wire = make_shared<Wire>(row_name + "_O");
		and_gate->Connect(PORTS::O, and_wire);
		adders[0][a - 1]->Connect(PORTS::A, and_wire);

		if (a == (num_bits_A - 1)) {
			and_gate->Connect(PORTS::B, input_nots_B[0]->GetWire(PORTS::O));
		}

		ands_row.emplace_back(and_gate);
		internal_wires.emplace_back(and_wire);
	}
	ands.emplace_back(ands_row);
	ands_row.clear();

	// The next levels of AND gates have a regular structure except the last two.
	for (size_t b = 1; b < (num_and_levels - 1); ++b) {
		row_name_prefix = name + "_AND_" + to_string(b) + '_';

		for (size_t a = 0; a < num_bits_A; ++a) {
			row_name = row_name_prefix + to_string(a);

			const auto and_gate = make_shared<And>(row_name);
			const auto and_wire = make_shared<Wire>(row_name + "_O");
			and_gate->Connect(PORTS::O, and_wire);

			if (a == (num_bits_A - 1)) {
				and_gate->Connect(PORTS::B, input_nots_B[b]->GetWire(PORTS::O));
				adders[b][a - 1]->Connect(PORTS::A, and_wire);
			} else {
				adders[b - 1][a]->Connect(PORTS::B, and_wire);
			}

			ands_row.emplace_back(and_gate);
			internal_wires.emplace_back(and_wire);
		}
		ands.emplace_back(ands_row);
		ands_row.clear();
	}

	// The last two levels are different, so handle them separately.
	{
		const size_t b = num_and_levels - 1;
		row_name_prefix = name + "_AND_" + to_string(b) + '_';

		// Second to last level.
		for (size_t a = 0; a < num_bits_A; ++a) {
			row_name = row_name_prefix + to_string(a);

			const auto and_gate = make_shared<And>(row_name);
			const auto and_wire = make_shared<Wire>(row_name + "_O");
			and_gate->Connect(PORTS::O, and_wire);

			if (a == (num_bits_A - 1)) {
				const auto adder = adders[b - 1][a];
				adder->Connect(PORTS::B, and_wire);
				adder->Connect(PORTS::A, input_nots_A.back()->GetWire(PORTS::O));
				adder->Connect(PORTS::Cin, input_nots_B.back()->GetWire(PORTS::O));
			} else {
				and_gate->Connect(PORTS::A, input_nots_A[a]->GetWire(PORTS::O));
				adders[b - 1][a]->Connect(PORTS::B, and_wire);
			}

			ands_row.emplace_back(and_gate);
			internal_wires.emplace_back(and_wire);
		}
		ands.emplace_back(ands_row);
		ands_row.clear();

		// The last level just requires hooking up the MSB of A and B,
		// and a hardcoded '1' to the last FA.
		const auto wire = make_shared<Wire>(name + "_hardcoded_1");
		wire->SetValue(true);
		adders.back().back()->Connect(PORTS::A, wire);
		internal_wires.emplace_back(wire);
	}

	// Create the Cout connections for each adder level except the last.
	for (size_t b = 0; b < (num_adder_levels - 1); ++b) {
		row_name_prefix = name + "_Cout_" + to_string(b) + "_";

		if (b < (num_adder_levels - 2)) {
			for (size_t a = 0; a < num_adders_per_level; ++a) {
				row_name = row_name_prefix + to_string(a);

				const auto wire = make_shared<Wire>(row_name);
				adders[b][a]->Connect(PORTS::Cout, wire);
				adders[b + 1][a]->Connect(PORTS::Cin, wire);

				internal_wires.emplace_back(wire);
			}
		} else {
			// Second to last row.
			for (size_t a = 0; a < num_bits_A; ++a) {
				row_name = row_name_prefix + to_string(a);

				const auto wire = make_shared<Wire>(row_name);
				adders[b][a]->Connect(PORTS::Cout, wire);
				adders[b + 1][a + 1]->Connect(PORTS::Cin, wire);

				internal_wires.emplace_back(wire);
			}
		}
	}

	// Create the Cout connections for the last level.
	row_name_prefix = name + "_Cout_" + to_string(num_adder_levels - 1) + "_";
	for (size_t a = 0; a < num_bits_A; ++a) {
		row_name = row_name_prefix + to_string(a);

		const auto wire = make_shared<Wire>(row_name);
		adders.back()[a]->Connect(PORTS::Cout, wire);
		adders.back()[a + 1]->Connect(PORTS::B, wire);

		internal_wires.emplace_back(wire);
	}
}

void Multiplier_2C::GenerateVHDLEntity(const string &path) const {

}

const string Multiplier_2C::GenerateVHDLInstance() const {
	return string("");
}
