#include "main.h"
#include <cassert>

/*
  Twos-complement Radix-4 Booth multiplier based on
  "High-Speed Booth Encoded Parallel Multiplier Design" by Yeh and Jen.
*/

Multiplier_2C_Booth::Multiplier_2C_Booth(string _name,
										 size_t _num_bits_A,
										 size_t _num_bits_B)
	: Component(_name)
	, num_bits_A(_num_bits_A)
	, num_bits_B(_num_bits_B)
	, num_bits_O(_num_bits_A + _num_bits_B)
	, num_encoders((_num_bits_B + 1) / 2)
	, num_decoders_per_row(_num_bits_A)
	, num_ppt_adders(min((size_t)1, num_encoders - 1))
	, adder_size_level_0(min(num_bits_O - 1, num_bits_A + 2))
	, final_adder_size(num_bits_O - 2) // TODO: Check if this is correct!
	, sign_extend_B_input_size((_num_bits_B + 1) % 3)
{
	if (num_bits_A == 0) {
		cout << "[Error] Size of port A of Multiplier_2C_Booth \"" << _name
			 << "\" must be larger than zero.\n";
		exit(1);
	} else if (num_bits_B == 0) {
		cout << "[Error] Size of port B of Multiplier_2C_Booth \"" << _name
			 << "\" must be larger than zero.\n";
		exit(1);
	}

	Generate2CBoothHardware();
}

void Multiplier_2C_Booth::Update(bool propagating) {
	if (needs_update || !propagating) {
		for (size_t i = 0; i < longest_path; ++i) {
			for (const auto &e : encoders) {
				e->Update(propagating);
			}
			se_not->Update(propagating);
			for (const auto &d : decoders) {
				d->Update(propagating);
			}
			for (const auto &c : cs_adders) {
				c->Update(propagating);
			}
			final_adder->Update(propagating);
		}

		needs_update = false;
	}
}

void Multiplier_2C_Booth::Connect(PORTS port, const wire_t &wire, size_t index) {
	CheckIfIndexIsInRange(port, index);

	auto error_undefined_port = [&](const auto &wire) {
		cout << "[Error] Trying to connect wire \"" << wire->GetName()
			 << "\" to undefined port of Multiplier_2C_Booth "
			 << "\"" << name << "\"\n";
		exit(1);
	};

	switch (port) {
	case PORTS::A:
		// ACHTUNG: should decoders[0][0] be hardcoded to zero?
		for (size_t i = 0; i < num_encoders; ++i) {
			decoders[i]->Connect(PORTS::Yj, wire, index);
		}

		if (index == 0) {
			for (size_t i = 0; i < num_encoders; ++i) {
				encoders[i]->Connect(PORTS::Y_LSB, wire);
			}
		} else if (index == (num_bits_A - 1)) {
			for (size_t i = 0; i < num_encoders; ++i) {
				encoders[i]->Connect(PORTS::Y_MSB, wire);
			}
		}
		input_wires.emplace_back(wire);
		break;
	case PORTS::B:
	{
		const size_t enc_idx = (index + 1) / 3; // Which encoder use to connect to.
		const size_t enc_bit_idx = (index + 1) % 3; // Which bit to connect to.

		switch (enc_bit_idx) {
		case 0: encoders[enc_idx]->Connect(PORTS::X_2I, wire); break;
		case 1:	encoders[enc_idx]->Connect(PORTS::X_2I, wire); break;
		case 2:
			encoders[enc_idx]->Connect(PORTS::X_2I_PLUS_ONE, wire);
			// X_2i+1 is also the NEG signal.
			decoders[enc_idx]->Connect(PORTS::NEG, wire);

			if (enc_idx < num_encoders) {
				encoders[enc_idx + 1]->Connect(PORTS::X_2I_MINUS_ONE, wire);
			}
			break;
		default:
			// Cannot happen.
			assert(false);
		}

		// Sign extension of the B input only is necessary if
		// we connect to the last encoder.
		if (enc_idx == (num_encoders - 1) && sign_extend_B_input_size > 0) {
			switch (sign_extend_B_input_size) {
			case 1:
				encoders[enc_idx]->Connect(PORTS::X_2I_PLUS_ONE, wire);
				decoders[enc_idx]->Connect(PORTS::NEG, wire);
				break;
			case 2:
				encoders[enc_idx]->Connect(PORTS::X_2I, wire);
				encoders[enc_idx]->Connect(PORTS::X_2I_PLUS_ONE, wire);
				decoders[enc_idx]->Connect(PORTS::NEG, wire);
				break;
			default:
				// Cannot happen.
				assert(false);
			}
		}
		input_wires.emplace_back(wire);
		break;
	}
	case PORTS::O:
		if (index == 0) {
			encoders.front()->Connect(PORTS::ROW_LSB, wire);
		} else if (index < (num_bits_O - final_adder_size)) {
			cs_adders[min((size_t)0, index - 1)]->Connect(PORTS::O, wire, 0);
		} else {
			final_adder->Connect(PORTS::O, wire, index - (num_bits_O - final_adder_size));
		}
		output_wires.emplace_back(wire);
		break;
	default:
		error_undefined_port(wire);
	}
}

void Multiplier_2C_Booth::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		cout << "[Error] Wire bundle \"" << wires->GetName()
			 << " accessed with index " << wire_idx
			 << " but has size " << wires->GetSize() << ".\n";
		exit(1);
	}

	const auto &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const wire_t Multiplier_2C_Booth::GetWire(PORTS port, size_t index) const {
	CheckIfIndexIsInRange(port, index);

	

	return nullptr;
}

void Multiplier_2C_Booth::Generate2CBoothHardware() {
	// Generate the inverted sign-extension hardware.
	se_not = make_shared<Not>(name + "_dec_0_se_n");
	se_not_o = make_shared<Wire>(name + "_dec_0_se_n_O");
	se_not->Connect(PORTS::O, se_not_o);
	internal_wires.emplace_back(se_not_o);

	// Generate the hardcoded '1' wire;
	hardcoded_1 = make_shared<Wire>(name + "_hardcoded_1");
	hardcoded_1->SetValue(true);

	// Generate the final ripple carry adder.
	final_adder = make_shared<RippleCarryAdder>(name + "_final_adder", final_adder_size);
	final_adder->PrintDebugAfterUpdate(true);

	// Generate the encoders.
	string name_prefix = name + "_enc_";
	for (size_t i = 0; i < num_encoders; ++i) {
		encoders.emplace_back(make_shared<BoothEncoderRadix4>(name_prefix + to_string(i)));
		encoders.back()->PrintDebugAfterUpdate(true);
	}

	// Generate the decoders.
	name_prefix = name + "_dec_";
	for (size_t i = 0; i < num_encoders; ++i) {
		decoders.emplace_back(
			make_shared<Radix4BoothDecoder>(
				name_prefix + to_string(i), num_decoders_per_row));
		decoders.back()->PrintDebugAfterUpdate(true);
	}

	// Connect the encoders to the decoders.
	name_prefix = name + "_enc_";
	for (size_t i = 0; i < num_encoders; ++i) {
		const auto i_str = to_string(i);
		const auto X1_b = make_shared<Wire>(name_prefix + i_str + "_X1_b_O");
		const auto X2_b = make_shared<Wire>(name_prefix + i_str + "_X2_b_O");
		const auto Z = make_shared<Wire>(name_prefix + i_str + "_Z_O");

		const auto &e = encoders[i];
		e->Connect(PORTS::X1_b, X1_b);
		e->Connect(PORTS::X2_b, X2_b);
		e->Connect(PORTS::Z, Z);

		const auto &d = decoders[i];
		d->Connect(PORTS::X1_b, X1_b);
		d->Connect(PORTS::X2_b, X2_b);
		d->Connect(PORTS::Z, Z);

		internal_wires.emplace_back(X1_b);
		internal_wires.emplace_back(X2_b);
		internal_wires.emplace_back(Z);
	}

	// Generate the Carry-Save adders. First adder row is
	// slightly larger due to more sign-extension bits.
	name_prefix = name + "_csa_";

	for (size_t i = 0; i < num_ppt_adders; ++i) {
		cs_adders.emplace_back(
			make_shared<CarrySaveAdder>(
				name_prefix + to_string(i), adder_size_level_0 - i));
		cs_adders.back()->PrintDebugAfterUpdate(true);
	}

	assert(encoders.size() == decoders.size());

	// Connect the outputs of the decoders to the Carry-Save adders.
	{
		// First level is different, so handle separately.
		{
			name_prefix = name + "_dec_0_";
			const auto &e0 = encoders[0];
			const auto &d0 = decoders[0];
			const auto &c0 = cs_adders[0];

			// Decoder PPTj
			for (size_t i = 0; i < (num_bits_A - 1); ++i) {
				const auto wire = make_shared<Wire>(name_prefix + to_string(i) + "_O");
				d0->Connect(PORTS::PPTj, wire, i);
				c0->Connect(PORTS::A, wire, i);
				internal_wires.emplace_back(wire);
			}

			// Neg_cin
			const auto neg_cin = make_shared<Wire>(name_prefix + "neg_cin_O");
			e0->Connect(PORTS::NEG_CIN, neg_cin);
			c0->Connect(PORTS::B, neg_cin, 0);
			internal_wires.emplace_back(neg_cin);

			// Sign-extension
			const auto se = make_shared<Wire>(name_prefix + "se_O");
			e0->Connect(PORTS::SE, se);
			se_not->Connect(PORTS::I, se);

			c0->Connect(PORTS::A, se_not_o, (num_bits_A - 1));
			c0->Connect(PORTS::A, se_not_o, num_bits_A);

			if (num_bits_O > (num_bits_A + 2)) {
				c0->Connect(PORTS::A, se, (num_bits_A + 1));
			}
			internal_wires.emplace_back(se);
		}

		// Second level of decoders.
		if (num_encoders > 1) {
			name_prefix = name + "_dec_1_";
			const auto &e1 = encoders[1];
			const auto &d1 = decoders[1];
			const auto &c0 = cs_adders[0];

			// Row_LSB
			const auto row_lsb = make_shared<Wire>(name_prefix + "row_lsb_O");
			e1->Connect(PORTS::ROW_LSB, row_lsb);
			c0->Connect(PORTS::B, row_lsb, 1);

			// Decoder PPTj
			for (size_t i = 0; i < (num_bits_A - 1); ++i) {
				const auto wire = make_shared<Wire>(name_prefix + to_string(i) + "_O");
				d1->Connect(PORTS::PPTj, wire, i);
				c0->Connect(PORTS::B, wire, i + 2);
				internal_wires.emplace_back(wire);
			}

			// Neg_cin
			const auto neg_cin = make_shared<Wire>(name_prefix + "neg_cin_O");
			e1->Connect(PORTS::NEG_CIN, neg_cin);
			c0->Connect(PORTS::Cin, neg_cin, 2);
			internal_wires.emplace_back(neg_cin);

			// Sign-extension
			if (num_bits_O > (num_bits_A + 2)) {
				const auto se = make_shared<Wire>(name_prefix + "se_O");
				e1->Connect(PORTS::SE, se);
				c0->Connect(PORTS::B, se, (num_bits_A + 1));
				internal_wires.emplace_back(se);

				if (num_encoders > 2) {
					c0->Connect(PORTS::B, hardcoded_1, (num_bits_A + 2));
				}
			}
		}

		// Handle the rest of the levels.
		for (size_t j = 2; j < num_encoders; ++j) {
			name_prefix = name + "_dec_" + to_string(j) + '_';
			const auto &e = encoders[j];
			const auto &d = decoders[j];
			const auto &c = cs_adders[j - 1];

			const auto row_lsb = make_shared<Wire>(name_prefix + "row_lsb_O");
			e->Connect(PORTS::ROW_LSB, row_lsb);
			c->Connect(PORTS::B, row_lsb, 1);
			internal_wires.emplace_back(row_lsb);

			for (size_t i = 0; i < (num_bits_A - 1); ++i) {
				const auto wire = make_shared<Wire>(name_prefix + to_string(i) + "_O");
				d->Connect(PORTS::PPTj, wire, i);
				c->Connect(PORTS::Cin, wire, i + (j + 2));
				internal_wires.emplace_back(wire);
			}

			// Neg_cin
			const auto neg_cin = make_shared<Wire>(name_prefix + "neg_cin_O");
			e->Connect(PORTS::NEG_CIN, neg_cin);
			if (j == 1) {
				c->Connect(PORTS::Cin, neg_cin, 2);
			} else {
				c->Connect(PORTS::Cin, neg_cin, 0);
			}

			// Sign-extension
			const auto se = make_shared<Wire>(name_prefix + "se_O");
			e->Connect(PORTS::SE, se);
			c->Connect(PORTS::B, se, (num_bits_A - 1) + (2 * j));
			internal_wires.emplace_back(se);

			// The MSB of the partial product is a hardcoded '1',
			// except for the last level.
			if (j != (num_encoders - 1)) {
				c->Connect(PORTS::B, hardcoded_1, (num_bits_A + 1));
			}
		}
	}

	// Connect the output of the last Carry-Save adder to the final adder.
	name_prefix = name + "_csa_" + to_string(num_encoders - 2) + '_';

	for (size_t i = 0; i < final_adder_size; ++i) {
		const auto cs_o = make_shared<Wire>(name_prefix + to_string(i + 1) + "_o");
		const auto cs_co = make_shared<Wire>(name_prefix + to_string(i) + "_cout");

		cs_adders.back()->Connect(PORTS::O, cs_o, i + 1);
		cs_adders.back()->Connect(PORTS::Cout, cs_co, i);
		final_adder->Connect(PORTS::A, cs_o, i);
		final_adder->Connect(PORTS::B, cs_co, i);
		internal_wires.emplace_back(cs_o);
		internal_wires.emplace_back(cs_co);
	}
}

void Multiplier_2C_Booth::CheckIfIndexIsInRange(PORTS port, size_t index) const {
	if (port == PORTS::A && index >= num_bits_A) {
		cout << "[Error] Index " << index << " of port A is out of "
			 << "bounds for Multiplier_2C_Booth \"" << name << "\".\n";
		exit(1);
	} else if (port == PORTS::B && index >= num_bits_B) {
		cout << "[Error] Index " << index << " of port B is out of "
			 << "bounds for Multiplier_2C_Booth \"" << name << "\".\n";
		exit(1);
	} else if (port == PORTS::O && index >= num_bits_O) {
		cout << "[Error] Index " << index << " of port O is out of "
			 << "bounds for Multiplier_2C_Booth \"" << name << "\".\n";
		exit(1);
	}
}
