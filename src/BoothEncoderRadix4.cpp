#include "main.h"

/*
 * Booth radix-4 encoder as described in
 * "High-Speed Booth Encoded Parallel Multiplier Design" by Yeh and Jen,
 * the "Neg_cin" term implemented as described in
 * "High-Speed and Low-Power Multipliers Using the Baugh-Wooley Algorithm
 * and HPM Reduction Tree" by Sjalander and Larsson-Edefors,
 * and the "SE" term implemented as a corrected version of the final version of
 * "Fast Multiplication: Algorithms and Implementation" by Bewick.
 *
 * Schematic:
 *
 * X_2i+1 ------------------- Neg
 *
 * X_2i-1 --+--|X|
 *          |  |N|
 *          |  |O|----------- X1_b
 * X_2i --+----|R|
 *        | |
 *        | |--|X|
 *        |    |O|--+-------- X2_b
 *        |----|R|  |
 *                  |--|A|
 *                     |N|--- Row_LSB_i
 * Y_LSB --------------|D|
 *
 * Y_MSB ------|X|
 *             |N|
 *             |O|---|
 * X_2i+1 -----|R|   |
 *                   |
 * X_2i-1 -----|N|   |
 * X_2i -------|O|   |--|O|
 * X_2i+1 -----|R|------|R|--- SE
 *             |3|   |--|3|
 *                   |
 * X_2i-1 -----|A|   |
 * X_2i -------|N|   |
 * X_2i+1 -----|D|---|
 *             |3|
 *
 * X_2i ---+--------|N|
 *         |        |O|---|
 *         |   |----|R|   |  X_2i+1 --|
 * X_2i+1 -----+          |           |--|A|
 *         |   |----|N|   |--|O|         |N|--- Neg_cin_i
 *         |        |O|------|R|---------|D|
 *         |   |----|R|   |--|3|
 * Y_LSB ------+          |
 *         |   |----|N|   |
 *         |        |O|---|
 *         |--------|R|
 */

bool BoothEncoderRadix4::entityGenerated = false;

BoothEncoderRadix4::BoothEncoderRadix4(string _name)
	: Component(_name, 3)
{
	X2_b = make_shared<Xor>(name + "_X2_b");
	X1_b = make_shared<Xnor>(name + "_X1_b");
	Z = make_shared<Xnor>(name + "_Z");
	Row_LSB = make_shared<And>(name + "_Row_LSB");
	Neg_cin_nor_1 = make_shared<Nor>(name + "_neg_cin_nor_1");
	Neg_cin_nor_2 = make_shared<Nor>(name + "_neg_cin_nor_2");
	Neg_cin_nor_3 = make_shared<Nor>(name + "_neg_cin_nor_3");
	Neg_cin_or3 = make_shared<Or3>(name + "_neg_cin_or3");
	Neg_cin_and = make_shared<And>(name + "_neg_cin_and");

	const auto nor_1_o = make_shared<Wire>(name + "_neg_cin_nor_1_O");
	const auto nor_2_o = make_shared<Wire>(name + "_neg_cin_nor_2_O");
	const auto nor_3_o = make_shared<Wire>(name + "_neg_cin_nor_3_O");
	const auto or3_o = make_shared<Wire>(name + "_neg_cin_or3_O");
	Neg_cin_nor_1->Connect(PORTS::O, nor_1_o);
	Neg_cin_nor_2->Connect(PORTS::O, nor_2_o);
	Neg_cin_nor_3->Connect(PORTS::O, nor_3_o);
	Neg_cin_or3->Connect(PORTS::A, nor_1_o);
	Neg_cin_or3->Connect(PORTS::B, nor_2_o);
	Neg_cin_or3->Connect(PORTS::C, nor_3_o);
	Neg_cin_or3->Connect(PORTS::O, or3_o);
	Neg_cin_and->Connect(PORTS::B, or3_o);

	internal_wires.emplace_back(nor_1_o);
	internal_wires.emplace_back(nor_2_o);
	internal_wires.emplace_back(nor_3_o);
	internal_wires.emplace_back(or3_o);

#ifdef METHOD_BEWICK
	longest_path = 4;
	SE_xnor = make_shared<Xnor>(name + "_SE_xnor");
	SE_nor3 = make_shared<Nor3>(name + "_SE_nor3");
	SE_and3 = make_shared<And3>(name + "_SE_and3");
	SE_or = make_shared<Or>(name + "_SE_or");
	SE_and = make_shared<And>(name + "_SE_and");
	SE_xor = make_shared<Xor>(name + "_SE_xor");

	const auto SE_nor3_o = make_shared<Wire>(name + "_SE_nor3_O");
	const auto SE_and3_o = make_shared<Wire>(name + "_SE_and3_O");
	const auto SE_or_o = make_shared<Wire>(name + "_SE_or_O");
	const auto SE_and_o = make_shared<Wire>(name + "_SE_and_O");
	const auto SE_xnor_o = make_shared<Wire>(name + "_SE_xnor_O");
	SE_nor3->Connect(PORTS::O, SE_nor3_o);
	SE_and3->Connect(PORTS::O, SE_and3_o);
	SE_or->Connect(PORTS::A, SE_nor3_o);
	SE_or->Connect(PORTS::B, SE_and3_o);
	SE_or->Connect(PORTS::O, SE_or_o);
	SE_and->Connect(PORTS::A, SE_or_o);
	SE_xnor->Connect(PORTS::O, SE_xnor_o);
	SE_xor->Connect(PORTS::A, SE_or_o);
	SE_xor->Connect(PORTS::B, SE_xnor_o);

	internal_wires.emplace_back(SE_nor3_o);
	internal_wires.emplace_back(SE_and3_o);
	internal_wires.emplace_back(SE_or_o);
	internal_wires.emplace_back(SE_and_o);
	internal_wires.emplace_back(SE_xnor_o);
#else
	longest_path = 3;
	SE_xnor = make_shared<Xnor>(name + "_SE_xnor");
	SE_nor3 = make_shared<Nor3>(name + "_SE_nor3");
	SE_and3 = make_shared<And3>(name + "_SE_and3");
	SE_or3 = make_shared<Or3>(name + "_SE_or3");

	const auto SE_nor3_o = make_shared<Wire>(name + "_SE_nor3_O");
	const auto SE_and3_o = make_shared<Wire>(name + "_SE_and3_O");
	const auto SE_xnor_o = make_shared<Wire>(name + "_SE_xnor_O");
	SE_nor3->Connect(PORTS::O, SE_nor3_o);
	SE_and3->Connect(PORTS::O, SE_and3_o);
	SE_xnor->Connect(PORTS::O, SE_xnor_o);
	SE_or3->Connect(PORTS::A, SE_nor3_o);
	SE_or3->Connect(PORTS::B, SE_and3_o);
	SE_or3->Connect(PORTS::C, SE_xnor_o);
#endif
}

void BoothEncoderRadix4::Update(bool propagating) {
	if (needs_update || !propagating) {
		for (size_t i = 0; i < longest_path; ++i) {
			X1_b->Update(propagating);
			X2_b->Update(propagating);
			Z->Update(propagating);
			Row_LSB->Update(propagating);
#ifdef METHOD_BEWICK
			SE_nor3->Update(propagating);
			SE_and3->Update(propagating);
			SE_or->Update(propagating);
			SE_and->Update(propagating);
			SE_xnor->Update(propagating);
			SE_xor->Update(propagating);
#else
			SE_nor3->Update(propagating);
			SE_and3->Update(propagating);
			SE_xnor->Update(propagating);
			SE_or3->Update(propagating);
#endif
			Neg_cin_nor_1->Update(propagating);
			Neg_cin_nor_2->Update(propagating);
			Neg_cin_nor_3->Update(propagating);
			Neg_cin_or3->Update(propagating);
			Neg_cin_and->Update(propagating);
		}

		if (!propagating && print_debug) {
			PrintDebug();
		}

		needs_update = false;
	}
}

void BoothEncoderRadix4::Connect(PORTS port, const wire_t &wire, size_t index) {
	auto error_undefined_port = [&](const auto &wire) {
		Error("Trying to connect wire \"" + wire->GetName() + "\" to undefined port of BoothEncoderRadix4 \"" + name + "\".\n");
	};

	switch (port) {
	/* Inputs */
	case PORTS::X_2I:
		X1_b->Connect(PORTS::B, wire);
		X2_b->Connect(PORTS::B, wire);
		Z->Connect(PORTS::B, wire);
		Neg_cin_nor_1->Connect(PORTS::A, wire);
		Neg_cin_nor_3->Connect(PORTS::B, wire);
		SE_nor3->Connect(PORTS::B, wire);
		SE_and3->Connect(PORTS::B, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::X_2I_MINUS_ONE:
		X1_b->Connect(PORTS::A, wire);
		X2_b->Connect(PORTS::A, wire);
		Neg_cin_nor_1->Connect(PORTS::B, wire);
		Neg_cin_nor_2->Connect(PORTS::A, wire);
		SE_nor3->Connect(PORTS::A, wire);
		SE_and3->Connect(PORTS::A, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::X_2I_PLUS_ONE:
		Z->Connect(PORTS::A, wire);
		Neg_cin_and->Connect(PORTS::A, wire);
		SE_xnor->Connect(PORTS::B, wire);
		SE_nor3->Connect(PORTS::C, wire);
		SE_and3->Connect(PORTS::C, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::Y_LSB:
		Row_LSB->Connect(PORTS::B, wire);
		Neg_cin_nor_2->Connect(PORTS::B, wire);
		Neg_cin_nor_3->Connect(PORTS::A, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::Y_MSB:
		SE_xnor->Connect(PORTS::A, wire);
#ifdef METHOD_BEWICK
		SE_and->Connect(PORTS::B, wire);
#endif
		input_wires.emplace_back(wire);
		break;
	/* Outputs */
	case PORTS::NEG:
		break;
	case PORTS::ROW_LSB:
		Row_LSB->Connect(PORTS::O, wire);
		output_wires.emplace_back(wire);
		break;
	case PORTS::X1_b:
		X1_b->Connect(PORTS::O, wire);
		output_wires.emplace_back(wire);
		break;
	case PORTS::X2_b:
		X2_b->Connect(PORTS::O, wire);
		Row_LSB->Connect(PORTS::A, wire);
		output_wires.emplace_back(wire);
		break;
	case PORTS::SE:
#ifdef METHOD_BEWICK
		SE_xor->Connect(PORTS::O, wire);
#else
		SE_or3->Connect(PORTS::O, wire);
#endif
		output_wires.emplace_back(wire);
		break;
	case PORTS::Z:
		Z->Connect(PORTS::O, wire);
		output_wires.emplace_back(wire);
		break;
	case PORTS::NEG_CIN:
		Neg_cin_and->Connect(PORTS::O, wire);
		output_wires.emplace_back(wire);
		break;
	default:
		error_undefined_port(wire);
	}
}

void BoothEncoderRadix4::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		Error("Wire bundle \"" + wires->GetName() + " accessed with index " + to_string(wire_idx)
			  + " but has size " + to_string(wires->GetSize()) + ".\n");
	}

	const wire_t &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const wire_t BoothEncoderRadix4::GetWire(PORTS port, size_t index) const {
	switch (port) {
	/* Inputs */
	case PORTS::X_2I: return X1_b->GetWire(PORTS::B);
	case PORTS::X_2I_MINUS_ONE: return X1_b->GetWire(PORTS::A);
	case PORTS::X_2I_PLUS_ONE: return Z->GetWire(PORTS::A);
	case PORTS::Y_LSB: return Row_LSB->GetWire(PORTS::B);
	case PORTS::Y_MSB: return SE_xnor->GetWire(PORTS::A);
	/* Outputs */
	case PORTS::NEG: return Z->GetWire(PORTS::A);
	case PORTS::ROW_LSB: return Row_LSB->GetWire(PORTS::O);
	case PORTS::X1_b: return X1_b->GetWire(PORTS::O);
	case PORTS::X2_b: return X2_b->GetWire(PORTS::O);
	case PORTS::SE:
#ifdef METHOD_BEWICK
		return SE_xor->GetWire(PORTS::O);
#else
		return SE_or3->GetWire(PORTS::O);
#endif
	case PORTS::Z: return Z->GetWire(PORTS::O);
	case PORTS::NEG_CIN: return Neg_cin_and->GetWire(PORTS::O);
	default:
		Error("Trying to get wire of undefined port of BoothEncoderRadix4 \"" + name + "\".\n");
	}
}

const PORT_DIR BoothEncoderRadix4::GetPortDirection(PORTS port) const {
	switch (port) {
	case PORTS::X_2I:
	case PORTS::X_2I_MINUS_ONE:
	case PORTS::X_2I_PLUS_ONE:
	case PORTS::Y_LSB:
	case PORTS::Y_MSB:
		return PORT_DIR::INPUT;
	case PORTS::NEG:
	case PORTS::ROW_LSB:
	case PORTS::X1_b:
	case PORTS::X2_b:
	case PORTS::SE:
	case PORTS::Z:
	case PORTS::NEG_CIN:
		return PORT_DIR::OUTPUT;
	default:
		Error("Trying to get port direction of undefined port in BoothEncoderRadix4 \"" + name + "\".\n");
	}
}

void BoothEncoderRadix4::PrintDebug() const {
	cout << "\n========================================\n";
	cout << name << ":\n";

	const auto &x_2i = X1_b->GetWire(PORTS::B);
	const auto &x_2i_m1 = X1_b->GetWire(PORTS::A);
	const auto &x_2i_p1 = Z->GetWire(PORTS::A);
	const auto &y_lsb = Row_LSB->GetWire(PORTS::B);
	const auto &y_msb = SE_xnor->GetWire(PORTS::A);
	const auto &row_lsb = Row_LSB->GetWire(PORTS::O);
	const auto &x1_b = X1_b->GetWire(PORTS::O);
	const auto &x2_b = X2_b->GetWire(PORTS::O);
#ifdef METHOD_BEWICK
	const auto &se = SE_xor->GetWire(PORTS::O);
	const auto &se_nor3_o = SE_nor3->GetWire(PORTS::O);
	const auto &se_and3_o = SE_and3->GetWire(PORTS::O);
	const auto &se_or_o = SE_or->GetWire(PORTS::O);
	const auto &se_and_o = SE_and->GetWire(PORTS::O);
	const auto &se_xnor_o = SE_xnor->GetWire(PORTS::O);
#else
	const auto &se = SE_or3->GetWire(PORTS::O);
	const auto &se_nor3_o = SE_nor3->GetWire(PORTS::O);
	const auto &se_and3_o = SE_and3->GetWire(PORTS::O);
	const auto &se_xnor_o = SE_xnor->GetWire(PORTS::O);
#endif
	const auto &z = Z->GetWire(PORTS::O);
	const auto &neg_cin = Neg_cin_and->GetWire(PORTS::O);

	if (x_2i_m1) cout << "X_2I_MINUS_ONE (" << x_2i_m1->GetName() << "): " << (*x_2i_m1)() << '\n';
	if (x_2i) cout << "X_2I (" << x_2i->GetName() << "): " << (*x_2i)() << '\n';
	if (x_2i_p1) cout << "X_2I_PLUS_ONE (" << x_2i_p1->GetName() << "): " << (*x_2i_p1)() << '\n';
	if (y_lsb) cout << "Y_LSB (" << y_lsb->GetName() << "): " << (*y_lsb)() << '\n';
	if (y_msb) cout << "Y_MSB (" << y_msb->GetName() << "): " << (*y_msb)() << '\n';
	if (x1_b) cout << "X1_b (" << x1_b->GetName() << "): " << (*x1_b)() << '\n';
	if (x2_b) cout << "X2_b (" << x2_b->GetName() << "): " << (*x2_b)() << '\n';
	if (x_2i_p1) cout << "NEG (" << x_2i_p1->GetName() << "): " << (*x_2i_p1)() << '\n';
	if (z) cout << "Z (" << z->GetName() << "): " << (*z)() << '\n';
	if (row_lsb) cout << "ROW_LSB (" << row_lsb->GetName() << "): " << (*row_lsb)() << '\n';
	if (se) cout << "SE (" << se->GetName() << "): " << (*se)() << '\n';
	if (neg_cin) cout << "NEG_CIN (" << neg_cin->GetName() << "): " << (*neg_cin)() << '\n';

	// Sign-extension internals
	cout << "\nSign-extension internal signals:\n";
#ifdef METHOD_BEWICK
	if (se_nor3_o) cout << "SE_nor3_o (" << se_nor3_o->GetName() << "): " << (*se_nor3_o)() << '\n';
	if (se_and3_o) cout << "SE_and3_o (" << se_and3_o->GetName() << "): " << (*se_and3_o)() << '\n';
	if (se_or_o) cout << "SE_or_o (" << se_or_o->GetName() << "): " << (*se_or_o)() << '\n';
	if (se_and_o) cout << "SE_and_o (" << se_and_o->GetName() << "): " << (*se_and_o)() << '\n';
	if (se_xnor_o) cout << "SE_xnor_o (" << se_xnor_o->GetName() << "): " << (*se_xnor_o)() << '\n';
#else
	if (se_nor3_o) cout << "SE_nor3_o (" << se_nor3_o->GetName() << "): " << (*se_nor3_o)() << '\n';
	if (se_and3_o) cout << "SE_and3_o (" << se_and3_o->GetName() << "): " << (*se_and3_o)() << '\n';
	if (se_xnor_o) cout << "SE_xnor_o (" << se_xnor_o->GetName() << "): " << (*se_xnor_o)() << '\n';
#endif

	cout << "========================================\n";
}

void BoothEncoderRadix4::GenerateVHDLEntity(const string &path) const {
	// We only need to do it once, since all instances of the BoothEncoderRadix4 are identical.
	if (!entityGenerated) {
		string output;
		TemplateDictionary entity("BoothEncoderRadix4");
		ExpandTemplate("src/templates/VHDL/BoothEncoderRadix4_entity.tpl", DO_NOT_STRIP, &entity, &output);

		auto outfile = ofstream(path + "/BoothEncoderRadix4.vhd");
		outfile << output;
		outfile.close();

		entityGenerated = true;
	}
}

const string BoothEncoderRadix4::GenerateVHDLInstance() const {
	const auto errorInconsistentWireBundle = [](const auto &wire, const auto &wb) {
		Error("Wire \"" + wire->GetName() + "\" is part of bundle \"" + wb->GetName() + "\" but has no index.\n");
	};

	string output;
	TemplateDictionary inst("BoothEncoderRadix4");

	// Set up the I/O wires.
	inst.SetValue("NAME", name);
	// X_2I
	{
		const auto &wire = GetWire(PORTS::X_2I);
		if (wire) {
			const auto &wb = wire->GetWireBundle();
			if (wb) {
				const auto idx = wb->GetWireIndex(wire);

				if (idx) {
					inst.SetValue("X_2I_WIRE", wb->GetName() + "(" + to_string(*idx) + ")");
				} else {
					errorInconsistentWireBundle(wire, wb);
				}
			} else {
				inst.SetValue("X_2I_WIRE", wire->GetName());
			}
		} else {
			// No wire, so assign a '0';
			inst.SetValue("X_2I_WIRE", "'0'");
		}
	}

	// X_2I_M1
	{
		const auto &wire = GetWire(PORTS::X_2I_MINUS_ONE);
		if (wire) {
			const auto &wb = wire->GetWireBundle();
			if (wb) {
				const auto idx = wb->GetWireIndex(wire);

				if (idx) {
					inst.SetValue("X_2I_M1_WIRE", wb->GetName() + "(" + to_string(*idx) + ")");
				} else {
					errorInconsistentWireBundle(wire, wb);
				}
			} else {
				inst.SetValue("X_2I_M1_WIRE", wire->GetName());
			}
		} else {
			// No wire, so assign a '0';
			inst.SetValue("X_2I_M1_WIRE", "'0'");
		}
	}

	// X_2I_P1
	{
		const auto &wire = GetWire(PORTS::X_2I_PLUS_ONE);
		if (wire) {
			const auto &wb = wire->GetWireBundle();
			if (wb) {
				const auto idx = wb->GetWireIndex(wire);

				if (idx) {
					inst.SetValue("X_2I_P1_WIRE", wb->GetName() + "(" + to_string(*idx) + ")");
				} else {
					errorInconsistentWireBundle(wire, wb);
				}
			} else {
				inst.SetValue("X_2I_P1_WIRE", wire->GetName());
			}
		} else {
			// No wire, so assign a '0';
			inst.SetValue("X_2I_P1_WIRE", "'0'");
		}
	}

	// Y_LSB
	{
		const auto &wire = GetWire(PORTS::Y_LSB);
		if (wire) {
			const auto &wb = wire->GetWireBundle();
			if (wb) {
				const auto idx = wb->GetWireIndex(wire);

				if (idx) {
					inst.SetValue("Y_LSB_WIRE", wb->GetName() + "(" + to_string(*idx) + ")");
				} else {
					errorInconsistentWireBundle(wire, wb);
				}
			} else {
				inst.SetValue("Y_LSB_WIRE", wire->GetName());
			}
		} else {
			// No wire, so assign a '0';
			inst.SetValue("Y_LSB_WIRE", "'0'");
		}
	}

	// Y_MSB
	{
		const auto &wire = GetWire(PORTS::Y_MSB);
		if (wire) {
			const auto &wb = wire->GetWireBundle();
			if (wb) {
				const auto idx = wb->GetWireIndex(wire);

				if (idx) {
					inst.SetValue("Y_MSB_WIRE", wb->GetName() + "(" + to_string(*idx) + ")");
				} else {
					errorInconsistentWireBundle(wire, wb);
				}
			} else {
				inst.SetValue("Y_MSB_WIRE", wire->GetName());
			}
		} else {
			// No wire, so assign a '0';
			inst.SetValue("Y_MSB_WIRE", "'0'");
		}
	}

	// ROW_LSB
	{
		const auto &wire = GetWire(PORTS::ROW_LSB);
		if (wire) {
			const auto &wb = wire->GetWireBundle();
			if (wb) {
				const auto idx = wb->GetWireIndex(wire);

				if (idx) {
					inst.SetValue("ROW_LSB_WIRE", wb->GetName() + "(" + to_string(*idx) + ")");
				} else {
					errorInconsistentWireBundle(wire, wb);
				}
			} else {
				inst.SetValue("ROW_LSB_WIRE", wire->GetName());
			}
		} else {
			// No wire, so assign a '0';
			inst.SetValue("ROW_LSB_WIRE", "'0'");
		}
	}

	// X1_b
	{
		const auto &wire = GetWire(PORTS::X1_b);
		if (wire) {
			const auto &wb = wire->GetWireBundle();
			if (wb) {
				const auto idx = wb->GetWireIndex(wire);

				if (idx) {
					inst.SetValue("X1_b_WIRE", wb->GetName() + "(" + to_string(*idx) + ")");
				} else {
					errorInconsistentWireBundle(wire, wb);
				}
			} else {
				inst.SetValue("X1_b_WIRE", wire->GetName());
			}
		} else {
			// No wire, so assign a '0';
			inst.SetValue("X1_b_WIRE", "'0'");
		}
	}

	// X2_b
	{
		const auto &wire = GetWire(PORTS::X2_b);
		if (wire) {
			const auto &wb = wire->GetWireBundle();
			if (wb) {
				const auto idx = wb->GetWireIndex(wire);

				if (idx) {
					inst.SetValue("X2_b_WIRE", wb->GetName() + "(" + to_string(*idx) + ")");
				} else {
					errorInconsistentWireBundle(wire, wb);
				}
			} else {
				inst.SetValue("X2_b_WIRE", wire->GetName());
			}
		} else {
			// No wire, so assign a '0';
			inst.SetValue("X2_b_WIRE", "'0'");
		}
	}

	// SE
	{
		const auto &wire = GetWire(PORTS::SE);
		if (wire) {
			const auto &wb = wire->GetWireBundle();
			if (wb) {
				const auto idx = wb->GetWireIndex(wire);

				if (idx) {
					inst.SetValue("SE_WIRE", wb->GetName() + "(" + to_string(*idx) + ")");
				} else {
					errorInconsistentWireBundle(wire, wb);
				}
			} else {
				inst.SetValue("SE_WIRE", wire->GetName());
			}
		} else {
			// No wire, so assign a '0';
			inst.SetValue("SE_WIRE", "'0'");
		}
	}

	// Z
	{
		const auto &wire = GetWire(PORTS::Z);
		if (wire) {
			const auto &wb = wire->GetWireBundle();
			if (wb) {
				const auto idx = wb->GetWireIndex(wire);

				if (idx) {
					inst.SetValue("Z_WIRE", wb->GetName() + "(" + to_string(*idx) + ")");
				} else {
					errorInconsistentWireBundle(wire, wb);
				}
			} else {
				inst.SetValue("Z_WIRE", wire->GetName());
			}
		} else {
			// No wire, so assign a '0';
			inst.SetValue("Z_WIRE", "'0'");
		}
	}

	// NEG_CIN
	{
		const auto &wire = GetWire(PORTS::NEG_CIN);
		if (wire) {
			const auto &wb = wire->GetWireBundle();
			if (wb) {
				const auto idx = wb->GetWireIndex(wire);

				if (idx) {
					inst.SetValue("NEG_CIN_WIRE", wb->GetName() + "(" + to_string(*idx) + ")");
				} else {
					errorInconsistentWireBundle(wire, wb);
				}
			} else {
				inst.SetValue("NEG_CIN_WIRE", wire->GetName());
			}
		} else {
			// No wire, so assign a '0';
			inst.SetValue("NEG_CIN_WIRE", "'0'");
		}
	}

	ExpandTemplate("src/templates/VHDL/BoothEncoderRadix4_inst.tpl", DO_NOT_STRIP, &inst, &output);

	return output;
}
