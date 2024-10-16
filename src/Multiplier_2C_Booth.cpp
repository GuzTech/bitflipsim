#include "main.h"
#include <cassert>

/*
  Twos-complement Radix-4 Booth multiplier based on
  "High-Speed Booth Encoded Parallel Multiplier Design" by Yeh and Jen.
*/

bool Multiplier_2C_Booth::entityGenerated = false;

Multiplier_2C_Booth::Multiplier_2C_Booth(string _name,
										 size_t _num_bits_A,
										 size_t _num_bits_B)
	: Component(_name)
	, num_bits_A(_num_bits_A)
	, num_bits_B(_num_bits_B)
	, num_bits_O(_num_bits_A + _num_bits_B)
	, num_encoders((_num_bits_B + 1) / 2)
	, num_decoders_per_row(_num_bits_A)
	, num_ppt_adders(max((size_t)1, num_encoders - 1))
	, adder_size_level_0(num_bits_A + 3)
	, final_adder_size(num_bits_O - num_encoders)
	, sign_extend_B_input_size(_num_bits_B % 2)
{
	if (num_bits_A == 0) {
		Error("Size of port A of Multiplier_2C_Booth \"" + _name + "\" must be larger than zero.\n");
	} else if (num_bits_B == 0) {
		Error("Size of port B of Multiplier_2C_Booth \"" + _name + "\" must be larger than zero.\n");
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
		const size_t enc_idx = index / 2; // Which encoder use to connect to.
		const size_t enc_bit_idx = (index + 1) % 2; // Which bit to connect to.

		switch (enc_bit_idx) {
		case 0:// encoders[enc_idx]->Connect(PORTS::X_2I, wire); break;
			encoders[enc_idx]->Connect(PORTS::X_2I_PLUS_ONE, wire);
			// X_2i+1 is also the NEG signal.
			decoders[enc_idx]->Connect(PORTS::NEG, wire);

			if ((enc_idx + 1) < num_encoders) {
				encoders[enc_idx + 1]->Connect(PORTS::X_2I_MINUS_ONE, wire);
			}
			break;
		case 1:	encoders[enc_idx]->Connect(PORTS::X_2I, wire); break;
		default:
			// Cannot happen.
			assert(false);
		}

		// Sign extension of the B input only is necessary if
		// we connect to the last encoder.
		if (enc_idx == (num_encoders - 1) && enc_bit_idx == 1 && sign_extend_B_input_size > 0) {
			encoders[enc_idx]->Connect(PORTS::X_2I_PLUS_ONE, wire);
			decoders[enc_idx]->Connect(PORTS::NEG, wire);
		}
		input_wires.emplace_back(wire);
		break;
	}
	case PORTS::O:
		if (index == 0) {
			encoders.front()->Connect(PORTS::ROW_LSB, wire);
		} else if (index < (num_bits_O - final_adder_size)) {
			cs_adders[index - 1]->Connect(PORTS::O, wire, 0);
		} else {
			final_adder->Connect(PORTS::O, wire, index - (num_bits_O - final_adder_size));
		}
		output_wires.emplace_back(wire);
		break;
	default:
		Error("Trying to connect wire \"" + wire->GetName() + "\" to undefined port of Multiplier_2C_Booth \"" + name + "\"\n");
	}
}

void Multiplier_2C_Booth::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		Error("Wire bundle \"" + wires->GetName() + " accessed with index " + to_string(wire_idx)
			  + " but has size " + to_string(wires->GetSize()) + ".\n");
	}

	const auto &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const wire_t Multiplier_2C_Booth::GetWire(PORTS port, size_t index) const {
	CheckIfIndexIsInRange(port, index);

	switch (port) {
		/* Inputs */
	case PORTS::A: return decoders.front()->GetWire(PORTS::Yj, index);
	case PORTS::B:
	{
		const size_t enc_idx = index / 2;
		const size_t enc_bit_idx = (index + 1) % 2;

		switch (enc_bit_idx) {
		case 0: return decoders[enc_idx]->GetWire(PORTS::NEG);
		case 1: return encoders[enc_idx]->GetWire(PORTS::X_2I);
		default:
			// Cannot happen.
			assert(false);
			return nullptr;
		}
	}
	case PORTS::O:
		if (index == 0) {
			return encoders.front()->GetWire(PORTS::ROW_LSB);
		} else if (index < (num_bits_O - final_adder_size)) {
			return cs_adders[index - 1]->GetWire(PORTS::O, 0);
		} else {
			return final_adder->GetWire(PORTS::O, index - (num_bits_O - final_adder_size));
		}
	default:
		Error("Trying to get wire of undefined port of Multiplier_2C_Booth \"" + name + "\".\n");
	}

	return nullptr;
}

const PORT_DIR Multiplier_2C_Booth::GetPortDirection(PORTS port) const {
	switch (port) {
	case PORTS::A:
	case PORTS::B:
		return PORT_DIR::INPUT;
	case PORTS::O:
		return PORT_DIR::OUTPUT;
	default:
		Error("Trying to get port direction of undefined port in Multiplier_2C_Booth \"" + name + "\".\n");
	}
}

void Multiplier_2C_Booth::Generate2CBoothHardware() {
	const auto create_enc = [&](const auto &name) {
		const auto enc = make_shared<BoothEncoderRadix4>(name);
		encoders.emplace_back(enc);

		for (const auto &w : enc->GetInternalWires()) {
			internal_wires.emplace_back(w);
		}
	};

	const auto create_dec = [&](const auto &name, const size_t size) {
		const auto dec = make_shared<Radix4BoothDecoder>(name, size);
		decoders.emplace_back(dec);

		for (const auto &w : dec->GetInternalWires()) {
			internal_wires.emplace_back(w);
		}
	};

	const auto create_csa = [&](const auto &name, const size_t size) {
		const auto csa = make_shared<CarrySaveAdder>(name, size);
		cs_adders.emplace_back(csa);

		for (const auto &w : csa->GetInternalWires()) {
			internal_wires.emplace_back(w);
		}
	};

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
	for (const auto &w : final_adder->GetInternalWires()) {
		internal_wires.emplace_back(w);
	}

	// Generate the encoders.
	string name_prefix = name + "_enc_";
	for (size_t i = 0; i < num_encoders; ++i) {
		create_enc(name_prefix + to_string(i));
	}

	// Generate the decoders.
	name_prefix = name + "_dec_";
	for (size_t i = 0; i < num_encoders; ++i) {
		create_dec(name_prefix + to_string(i), num_decoders_per_row);
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
		create_csa(name_prefix + to_string(i), adder_size_level_0 + i);
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
				const auto wire = make_shared<Wire>(name_prefix + to_string(i + 1) + "_O");
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
			internal_wires.emplace_back(row_lsb);

			// Decoder PPTj
			for (size_t i = 0; i < (num_bits_A - 1); ++i) {
				const auto wire = make_shared<Wire>(name_prefix + to_string(i + 1) + "_O");
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
			if (adder_size_level_0 > (num_bits_A + 2)) {
				const auto se = make_shared<Wire>(name_prefix + "se_O");
				e1->Connect(PORTS::SE, se);
				c0->Connect(PORTS::B, se, (num_bits_A + 1));
				internal_wires.emplace_back(se);

				if (num_encoders > 2) {
					c0->Connect(PORTS::B, hardcoded_1, (num_bits_A + 2));
				}
			}
		}

		// Third level of decoders.
		if (num_encoders > 2) {
			name_prefix = name + "_dec_2_";
			const auto &e2 = encoders[2];
			const auto &d2 = decoders[2];
			const auto &c0 = cs_adders[0];
			const auto &c1 = cs_adders[1];

			// Row_LSB
			const auto row_lsb = make_shared<Wire>(name_prefix + "row_lsb_O");
			e2->Connect(PORTS::ROW_LSB, row_lsb);
			c0->Connect(PORTS::Cin, row_lsb, 3);
			internal_wires.emplace_back(row_lsb);

			// Decoder PPTj
			for (size_t i = 0; i < (num_bits_A - 1); ++i) {
				const auto wire = make_shared<Wire>(name_prefix + to_string(i + 1) + "_O");
				d2->Connect(PORTS::PPTj, wire, i);
				c0->Connect(PORTS::Cin, wire, i + 4);
				internal_wires.emplace_back(wire);
			}

			// Neg_cin
			const auto neg_cin = make_shared<Wire>(name_prefix + "neg_cin_O");
			e2->Connect(PORTS::NEG_CIN, neg_cin);
			c1->Connect(PORTS::Cin, neg_cin, 3);
			internal_wires.emplace_back(neg_cin);

			// Sign-extension
			const auto se = make_shared<Wire>(name_prefix + "se_O");
			e2->Connect(PORTS::SE, se);
			c1->Connect(PORTS::A, se, (num_bits_A + 2));
			internal_wires.emplace_back(se);

			if (num_encoders > 3) {
				c1->Connect(PORTS::A, hardcoded_1, (num_bits_A + 3));
			}

			// Connect the output of this level to the next level.
			name_prefix = name + "_csa_0_";
			for (size_t i = 0; i < adder_size_level_0; ++i) {
				if (i != (adder_size_level_0 - 1)) {
					const auto cs_o = make_shared<Wire>(name_prefix + to_string(i + 1) + "_O");
					c0->Connect(PORTS::O, cs_o, i + 1);
					c1->Connect(PORTS::A, cs_o, i);
					internal_wires.emplace_back(cs_o);
				}

				const auto cs_co = make_shared<Wire>(name_prefix + to_string(i) + "_Cout");
				c0->Connect(PORTS::Cout, cs_co, i);
				c1->Connect(PORTS::B, cs_co, i);
				internal_wires.emplace_back(cs_co);
			}
		}

		// Handle the rest of the levels.
		for (size_t j = 3; j < num_encoders; ++j) {
			const auto dec_prefix = name + "_dec_" + to_string(j) + '_';
			const auto enc_prefix = name + "_enc_" + to_string(j) + '_';
			const auto c_curr_prefix = name + "_csa_" + to_string(j - 2) + '_';
			const auto c_next_prefix = name + "_csa_" + to_string(j - 1) + '_';

			const auto &e = encoders[j];
			const auto &d = decoders[j];
			const auto &c_curr = cs_adders[j - 2]; // Current CSA
			const auto &c_next = cs_adders[j - 1]; // Next CSA

			const auto row_lsb = make_shared<Wire>(enc_prefix + "row_lsb_O");
			e->Connect(PORTS::ROW_LSB, row_lsb);
			c_curr->Connect(PORTS::Cin, row_lsb, j + 1);
			internal_wires.emplace_back(row_lsb);

			for (size_t i = 0; i < (num_bits_A - 1); ++i) {
				const auto wire = make_shared<Wire>(dec_prefix + to_string(i + 1) + "_O");
				d->Connect(PORTS::PPTj, wire, i);
				c_curr->Connect(PORTS::Cin, wire, i + (j + 2));
				internal_wires.emplace_back(wire);
			}

			// Neg_cin
			const auto neg_cin = make_shared<Wire>(enc_prefix + "neg_cin_O");
			e->Connect(PORTS::NEG_CIN, neg_cin);
			c_next->Connect(PORTS::Cin, neg_cin, j + 1);
			internal_wires.emplace_back(neg_cin);

			// Sign-extension
			const auto se = make_shared<Wire>(enc_prefix + "se_O");
			e->Connect(PORTS::SE, se);
			c_next->Connect(PORTS::A, se, j + num_bits_A);
			internal_wires.emplace_back(se);

			// The MSB of the partial product is a hardcoded '1'.
			c_next->Connect(PORTS::A, hardcoded_1, (num_bits_A + j + 1));

			// Connect the output of this level to the next level.
			for (size_t i = 0; i < (adder_size_level_0 + (j - 2)); ++i) {
				if (i != (adder_size_level_0 + (j - 3))) {
					const auto cs_o = make_shared<Wire>(c_curr_prefix + to_string(i + 1) + "_O");
					c_curr->Connect(PORTS::O, cs_o, i + 1);
					c_next->Connect(PORTS::A, cs_o, i);
					internal_wires.emplace_back(cs_o);
				}

				const auto cs_co = make_shared<Wire>(c_curr_prefix + to_string(i) + "_Cout");
				c_curr->Connect(PORTS::Cout, cs_co, i);
				c_next->Connect(PORTS::B, cs_co, i);
				internal_wires.emplace_back(cs_co);
			}
		}
	}

	// Connect the output of the last Carry-Save adder to the final adder.
	name_prefix = name + "_csa_" + to_string(cs_adders.size() - 1) + '_';

	if (num_encoders != 1) {
		for (size_t i = 0; i < final_adder_size; ++i) {
			const auto cs_o = make_shared<Wire>(name_prefix + to_string(i + 1) + "_O");
			cs_adders.back()->Connect(PORTS::O, cs_o, i + 1);
			final_adder->Connect(PORTS::A, cs_o, i);
			internal_wires.emplace_back(cs_o);

			const auto cs_co = make_shared<Wire>(name_prefix + to_string(i) + "_Cout");
			cs_adders.back()->Connect(PORTS::Cout, cs_co, i);
			final_adder->Connect(PORTS::B, cs_co, i);
			internal_wires.emplace_back(cs_co);
		}
	} else {
		for (size_t i = 0; i < final_adder_size; ++i) {
			const auto cs_o = make_shared<Wire>(name_prefix + to_string(i + 2) + "_O");
			cs_adders.back()->Connect(PORTS::O, cs_o, i + 2);
			final_adder->Connect(PORTS::A, cs_o, i);
			internal_wires.emplace_back(cs_o);

			const auto cs_co = make_shared<Wire>(name_prefix + to_string(i + 1) + "_Cout");
			cs_adders.back()->Connect(PORTS::Cout, cs_co, i + 1);
			final_adder->Connect(PORTS::B, cs_co, i);
			internal_wires.emplace_back(cs_co);
		}
	}
}

void Multiplier_2C_Booth::CheckIfIndexIsInRange(PORTS port, size_t index) const {
	if (port == PORTS::A && index >= num_bits_A) {
		Error("Index " + to_string(index) + " of port A is out of bounds for Multiplier_2C_Booth \"" + name + "\".\n");
	} else if (port == PORTS::B && index >= num_bits_B) {
		Error("Index " + to_string(index) + " of port B is out of bounds for Multiplier_2C_Booth \"" + name + "\".\n");
	} else if (port == PORTS::O && index >= num_bits_O) {
		Error("Index " + to_string(index) + " of port O is out of bounds for Multiplier_2C_Booth \"" + name + "\".\n");
	}
}

void Multiplier_2C_Booth::GenerateVHDLEntity(const string &path) const {
	// We only need to do it once, since all instances of Multiplier_2C_Booth are identical.
	if (!entityGenerated) {
		string output;
		TemplateDictionary entity("Multiplier_2C_Booth");
		ExpandTemplate("src/templates/VHDL/Multiplier_2C_Booth_entity.tpl", DO_NOT_STRIP, &entity, &output);

		auto outfile = ofstream(path + "/Multiplier_2C_Booth.vhd");
		outfile << output;
		outfile.close();

		encoders.front()->GenerateVHDLEntity(path);
		decoders.front()->GenerateVHDLEntity(path);
		cs_adders.front()->GenerateVHDLEntity(path);
		final_adder->GenerateVHDLEntity(path);

		entityGenerated = true;
	}
}

const string Multiplier_2C_Booth::GenerateVHDLInstance() const {
	string output;
	TemplateDictionary inst("Multiplier_2C_Booth");

	// Set up the I/O wires.
	inst.SetValue("NAME", name);
	inst.SetValue("NUM_BITS_A", to_string(num_bits_A));
	inst.SetValue("NUM_BITS_B", to_string(num_bits_B));

	inst.SetValue("ARCH", "arch");

	GenerateAssignments(PORTS::A, num_bits_A, "A", inst);
	GenerateAssignments(PORTS::B, num_bits_B, "B", inst);
	GenerateAssignments(PORTS::O, num_bits_O, "O", inst, true);

	ExpandTemplate("src/templates/VHDL/Multiplier_2C_Booth_inst.tpl", DO_NOT_STRIP, &inst, &output);

	return output;
}
