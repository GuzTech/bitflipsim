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
			for (const auto &d : decoders) {
				d->Update(propagating);
			}
			for (const auto &c : cs_adders) {
				c->Update(propagating);
			}
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
		case 0: encoders[enc_idx]->Connect(PORTS::X_2I_MINUS_ONE, wire); break;
		case 1: encoders[enc_idx]->Connect(PORTS::X_2I, wire); break;
		case 2:
			encoders[enc_idx]->Connect(PORTS::X_2I_PLUS_ONE, wire);
			// X_2i+1 is also the NEG signal.
			decoders[enc_idx]->Connect(PORTS::NEG, wire);
			break;
		default:
			// Cannot happen.
			assert(false);
		}
		input_wires.emplace_back(wire);
		break;
	}
	case PORTS::O:
		cs_adders.back()->Connect(PORTS::O, wire, index);
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
	// Generate the encoders.
	string name_prefix = name + "_enc_";
	for (size_t i = 0; i < num_encoders; ++i) {
		encoders.emplace_back(make_shared<BoothEncoderRadix4>(name_prefix + to_string(i)));
	}

	// Generate the decoders.
	name_prefix = name + "_dec_";
	for (size_t i = 0; i < num_encoders; ++i) {
		decoders.emplace_back(
			make_shared<Radix4BoothDecoder>(
				name_prefix + to_string(i), num_decoders_per_row));
	}

	// Connect the encoders to the decoders.
	name_prefix = name + "_enc_";
	for (size_t i = 0; i < num_encoders; ++i) {
		const auto i_str = to_string(i);
		//const auto Neg = make_shared<Wire>(name_prefix + "Neg_" + i_str + "_O");
		const auto X1_b = make_shared<Wire>(name_prefix + "X1_b_" + i_str + "_O");
		const auto X2_b = make_shared<Wire>(name_prefix + "X2_b_" + i_str + "_O");
		const auto Z = make_shared<Wire>(name_prefix + "Z_" + i_str + "_O");
		//const auto Row_LSB = make_shared<Wire>(name_prefix + "Row_LSB_" + i_str + "_O");
		//const auto SE = make_shared<Wire>(name_prefix + "SE_" + i_str + "_O");
		//const auto Neg_cin = make_shared<Wire>(name_prefix + "Neg_cin_" + i_str + "_O");

		const auto &e = encoders[i];
		//e->Connect(PORTS::NEG, Neg);
		e->Connect(PORTS::X1_b, X1_b);
		e->Connect(PORTS::X2_b, X2_b);
		e->Connect(PORTS::Z, Z);

		const auto &d = decoders[i];
		d->Connect(PORTS::X1_b, X1_b);
		d->Connect(PORTS::X2_b, X2_b);
		d->Connect(PORTS::Z, Z);

		//internal_wires.emplace_back(Neg);
		internal_wires.emplace_back(X1_b);
		internal_wires.emplace_back(X2_b);
		internal_wires.emplace_back(Z);
		//internal_wires.emplace_back(Row_LSB);
		//internal_wires.emplace_back(SE);
		//internal_wires.emplace_back(Neg_cin);

		// TODO: CONNECT Row_LSB, SE, AND Neg_cin TOO!
	}

	// Generate the Carry-Save adders. First adder is slightly
	// larger due to more sign-extension bits.
	name_prefix = name + "_csa_";
	cs_adders.emplace_back(
		make_shared<CarrySaveAdder>(
			name_prefix + '0', num_bits_A + 3));

	for (size_t i = 1; i < (num_encoders - 1); ++i) {
		size_t bit_size = 0;

		if (i != (num_encoders - 2)) {
			// The other adders except the last one
			// are the same size.
			bit_size = num_bits_A + 2;
		} else {
			// The last adder is one smaller than
			// the middle ones.
			bit_size = num_bits_A + 1;
		}

		assert(bit_size != 0);

		cs_adders.emplace_back(
			make_shared<CarrySaveAdder>(
				name_prefix + to_string(i), bit_size));
	}

	assert(encoders.size() == decoders.size());

	// Connect the outputs of the decoders to the Carry-Save adders.
	{
		name_prefix = name + "_dec_0_";
		const auto &e0 = encoders[0];
		const auto &d0 = decoders[0];
		const auto &c0 = cs_adders[0];

		for (size_t i = 0; i < (num_bits_A - 1); ++i) {
			const auto wire = make_shared<Wire>(name_prefix + to_string(i) + "_O");
			d0->Connect(PORTS::PPTj, wire, i);
			c0->Connect(PORTS::A, wire, i + 1);
			internal_wires.emplace_back(wire);
		}
		const auto row_lsb = make_shared<Wire>(name_prefix + "row_lsb_O");
		e0->Connect(PORTS::ROW_LSB, row_lsb);
		c0->Connect(PORTS::A, row_lsb, 0);
		internal_wires.emplace_back(row_lsb);

		const auto se = make_shared<Wire>(name_prefix + "se_O");
		e0->Connect(PORTS::SE, se);
		c0->Connect(PORTS::A, se, (num_bits_A - 1));
		internal_wires.emplace_back(se);
		
		for (size_t j = 1; j < num_encoders; ++j) {
			name_prefix = name + "_dec_" + to_string(j) + '_';
			const auto &e = encoders[j];
			const auto &d = decoders[j];
			const auto &c = cs_adders[j - 1];

			for (size_t i = 1; i < (num_bits_A - 1); ++i) {
				const auto wire = make_shared<Wire>(name_prefix + to_string(i) + "_O");
				d->Connect(PORTS::PPTj, wire, i);
				c->Connect(PORTS::B, wire, i);
				internal_wires.emplace_back(wire);
			}

			const auto row_lsb = make_shared<Wire>(name_prefix + "row_lsb_O");
			e->Connect(PORTS::ROW_LSB, row_lsb);
			c->Connect(PORTS::B, row_lsb, 0);
			internal_wires.emplace_back(row_lsb);

			const auto se = make_shared<Wire>(name_prefix + "se_O");
			e->Connect(PORTS::SE, se);
			c->Connect(PORTS::A, se, (num_bits_A - 1));
			internal_wires.emplace_back(se);
		}
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
