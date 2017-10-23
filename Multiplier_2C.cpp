#include "main.h"

/*
  Twos-complement multiplier implementation.
*/

Multiplier_2C::Multiplier_2C(string _name,
							 size_t _num_bits_A,
							 size_t _num_bits_B,
							 MUL_TYPE _type)
	: Component(_name, _num_bits_B)
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
		cout << "[Error] Size of port A of Multiplier_2C \"" << _name
			 << "\" must be larger than zero.\n";
		exit(1);
	} else if (num_bits_B == 0) {
		cout << "[Error] Size of port B of Multiplier_2C \"" << _name
			 << "\" must be larger than zero.\n";
		exit(1);
	}

	// Generate the requested hardware.
	switch (type) {
	case MUL_TYPE::ARRAY_SIGN_EXTEND: GenerateSignExtendHardware(); break;
	case MUL_TYPE::ARRAY_INVERSION:   GenerateInversionHardware(); break;
	default:
		cout << "[Error] Unknown type supplied for generating Multiplier_2C \""
			 << name << "\"\n";
		exit(1);
	}
}

void Multiplier_2C::Update(bool propagating) {
	if (needs_update || !propagating) {
		switch (type) {
		case MUL_TYPE::ARRAY_SIGN_EXTEND:
		case MUL_TYPE::ARRAY_INVERSION:
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
		}

		needs_update = false;
	}
}

void Multiplier_2C::Connect(PORTS port, wire_t wire, size_t index) {
	if (port == PORTS::A && index >= num_bits_A) {
		cout << "[Error] Index " << index << " of port A is out of "
			 << "bounds for Multiplier_2C \"" << name << "\"\n";
		exit(1);
	} else if (port == PORTS::B && index >= num_bits_B) {
		cout << "[Error] Index " << index << " of port B is out of "
			 << "bounds for Multiplier_2C \"" << name << "\"\n";
		exit(1);
	} else if (port == PORTS::O && index >= num_bits_O) {
		cout << "[Error] Index " << index << " of port O is out of "
			 << "bounds for Multiplier_2C \"" << name << "\"\n";
		exit(1);
	}

	switch (type) {
	case MUL_TYPE::ARRAY_SIGN_EXTEND: break;
	case MUL_TYPE::ARRAY_INVERSION:
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
			break;
		case PORTS::O:
			if (index == (num_bits_O - 1)) {
				output_2C_adder_xor->Connect(PORTS::O, wire);
			} else {
				output_2C_adders[index]->Connect(PORTS::O, wire);
			}
			break;
		default:
			cout << "[Error] Trying to connect wire \"" << wire->GetName()
				 << "\" to undefined port of Multiplier_2C "
				 << "\"" << name << "\"\n";
			exit(1);
		}
		break;
	}
}

size_t Multiplier_2C::GetNumToggles() {
	toggle_count = 0;

	// Only full adders have internal state, and therefore only
	// they can increase the toggle count. The twos-complement
	// hardware only use half adders, so they do not contribute
	// to the toggle_count. Internal wires are handled by System.
	for (const auto &add_row : adders) {
		for (const auto &add : add_row) {
			toggle_count += add->GetNumToggles();
		}
	}

	return toggle_count;
}

vector<wire_t> Multiplier_2C::GetWires() {
	vector<wire_t> wires;

	// Add all input wires.
	const vector<wire_t> &input = GetInputWires();
	wires.insert(wires.end(),
				 input.begin(),
				 input.end());

	// Add all internal wires.
	wires.insert(wires.end(),
				 internal_wires.begin(),
				 internal_wires.end());

	// Add all output wires.
	const vector<wire_t> &output = GetOutputWires();
	wires.insert(wires.end(),
				 output.begin(),
				 output.end());

	return wires;
}

vector<wire_t> Multiplier_2C::GetInputWires() {
	vector<wire_t> input_wires;

	switch (type) {
	case MUL_TYPE::ARRAY_SIGN_EXTEND: break;
	case MUL_TYPE::ARRAY_INVERSION:
		// Add the A and B inputs of the 2C XOR gates
		for (const auto &xor_i : input_2C_xors_A) {
			const auto &wire_A = xor_i->GetWire(PORTS::A);
			const auto &wire_B = xor_i->GetWire(PORTS::B);

			input_wires.push_back(wire_A);
			input_wires.push_back(wire_B);
		}

		for (const auto &xor_i : input_2C_xors_B) {
			const auto &wire_A = xor_i->GetWire(PORTS::A);
			const auto &wire_B = xor_i->GetWire(PORTS::B);

			input_wires.push_back(wire_A);
			input_wires.push_back(wire_B);
		}
		break;
	}
	
	return input_wires;
}

vector<wire_t> Multiplier_2C::GetOutputWires() {
	vector<wire_t> output_wires;

	// Add all the output port wires of the half adders.
	for (const auto &o : output_2C_adders) {
		output_wires.push_back(o->GetWire(PORTS::O));
	}

	output_wires.push_back(output_2C_adder_xor->GetWire(PORTS::O));

	return output_wires;
}

wire_t Multiplier_2C::GetWire(PORTS port, size_t index) {
	return nullptr;
}

void Multiplier_2C::GenerateSignExtendHardware() {
}

// This generates hardware that has a twos-complement operator for
// both A and B inputs and also the output. Effectively, this ensures
// that we always multiply two positive numbers, and adjust the output
// if the signs of A and B are different.
void Multiplier_2C::GenerateInversionHardware() {
	// Names for the adders.
	string row_name_prefix = string(name) + "_S_0_";
	string row_name = row_name_prefix + "0";

	// First row consists of #A - 2 FullAdders and 2 HalfAdders.
	vector<comp_t> adders_row;

	adders_row.push_back(make_shared<HalfAdder>(row_name));
	for (size_t a = 1; a < num_adders_per_level - 1; ++a) {
		row_name = row_name_prefix + to_string(a);

		adders_row.push_back(make_shared<FullAdder>(row_name));
	}
	row_name = row_name_prefix + to_string(num_adders_per_level - 1);
	adders_row.push_back(make_shared<HalfAdder>(row_name));

	adders.push_back(adders_row);
	adders_row.clear();

	// The rest of the rows consist of #A FullAdders and 1 HalfAdder.
	for (size_t b = 1; b < num_adder_levels; ++b) {
		row_name_prefix = name + "_S_" + to_string(b) + "_";
		row_name = row_name_prefix + "0";
		adders_row.push_back(make_shared<HalfAdder>(row_name));

		for (size_t a = 1; a < num_adders_per_level; ++a) {
			row_name = row_name_prefix + to_string(a);
			adders_row.push_back(make_shared<FullAdder>(row_name));
		}

		adders.push_back(adders_row);
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
			internal_wires.push_back(wire);
		}
	}

	// Create the AND gates that connect to the inputs of the adders.
	vector<and_t> ands_row;

	// First level of AND gates is slightly different, so handle it
	// separately. The first AND gate is directly connected to bit 0
	// of the result.
	row_name_prefix = name + "_AND_0_";
	ands_row.push_back(make_shared<And>(row_name_prefix + "0"));

	for (size_t a = 1; a < num_ands_per_level; ++a) {
		row_name = row_name_prefix + to_string(a);

		const auto and_gate = make_shared<And>(row_name);
		const auto and_wire = make_shared<Wire>(row_name + "_O");
		and_gate->Connect(PORTS::O, and_wire);
		adders[0][a-1]->Connect(PORTS::A, and_wire);
		ands_row.push_back(and_gate);

		internal_wires.push_back(and_wire);
	}
	ands.push_back(ands_row);
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
			ands_row.push_back(and_gate);

			internal_wires.push_back(and_wire);
		}

		ands.push_back(ands_row);
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
					internal_wires.push_back(wire);

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
				for (size_t a = 0; a < num_adders_per_level - 1; ++a) {
					row_name = row_name_prefix + to_string(a);

					const bool second_to_last_a = a == (num_adders_per_level - 2);

					const auto wire = make_shared<Wire>(row_name);
					adders[b][a]->Connect(PORTS::Cout, wire);
					internal_wires.push_back(wire);

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
					internal_wires.push_back(wire);

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

		input_2C_xors_A.push_back(inv_xor);
		internal_wires.push_back(wire);
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

		input_2C_adders_A.push_back(inv_ha);
		internal_wires.push_back(out_wire);
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

		internal_wires.push_back(wire);
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

		input_2C_xors_B.push_back(inv_xor);
		internal_wires.push_back(wire);
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

		input_2C_adders_B.push_back(inv_ha);
		internal_wires.push_back(out_wire);
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

		internal_wires.push_back(cout_wire);
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

		output_2C_xors.push_back(inv_xor);
		internal_wires.push_back(inv_wire);
	}

	// Create the half adders for the output.
	conv_name_prefix = name + "_out_2C_ha_";
	for (size_t i = 0; i < num_bits_O - 1; ++i) {
		conv_name = conv_name_prefix + to_string(i);
		const auto inv_ha = make_shared<HalfAdder>(conv_name);
		const auto &xor_wire = output_2C_xors[i]->GetWire(PORTS::O);
		inv_ha->Connect(PORTS::A, xor_wire);
		output_2C_adders.push_back(inv_ha);
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
		internal_wires.push_back(cout_wire);
	}

	// Connect the carry out of the last half adder to
	// the A input of the output adder XOR gate.
	auto wire = make_shared<Wire>(
		conv_name_prefix + to_string(num_bits_O - 2) + "_Cout");
	output_2C_adders.back()->Connect(PORTS::Cout, wire);
	output_2C_adder_xor->Connect(PORTS::A, wire);
	internal_wires.push_back(wire);

	// Connect the XOR gates to the last level of adders.
	conv_name_prefix = name + "_in_2C_XOR_";
	const auto &last_row = adders.back();

	for (size_t i = 0; i < num_bits_O - 1; ++i) {
		conv_name = conv_name_prefix + to_string(i);
		const auto xor_wire = make_shared<Wire>(conv_name);

		if (i == (num_bits_O - 1)) {
			last_row[num_adders_per_level - 1]->Connect(PORTS::Cout, xor_wire);
		} else if (i > (num_bits_O - num_adders_per_level - 1)) {
			last_row[i - num_adders_per_level + 1]->Connect(PORTS::O, xor_wire);
		} else if (i == 0) {
			// Index zero is the output of the first AND gate.
			ands[0][0]->Connect(PORTS::O, xor_wire);
		} else {
			adders[i - 1][0]->Connect(PORTS::O, xor_wire);
		}
		output_2C_xors[i]->Connect(PORTS::A, xor_wire);
		internal_wires.push_back(xor_wire);
	}

	// Connect the carry out of the last adder of the last
	// level to the A input of the output 2C adder XOR gate.
	wire = make_shared<Wire>(conv_name_prefix + to_string(num_bits_O - 1));
	last_row.back()->Connect(PORTS::Cout, wire);
	output_2C_adder_xor->Connect(PORTS::A, wire);
	internal_wires.push_back(wire);

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
	internal_wires.push_back(wire);
}
