#include "main.h"

/*
 * Booth radix-4 encoder as described in
 * "High-Speed Booth Encoded Parallel Multiplier Design" by Yeh and Jen,
 * with the "Neg_cin" term implemented as described in
 * "High-Speed and Low-Power Multipliers Using the Baugh-Wooley Algorithm
 * and HPM Reduction Tree" by Sjalander and Larsson-Edefors.
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
 *             |O|----------- SE
 * X_2i+1 -----|R|
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

BoothEncoderRadix4::BoothEncoderRadix4(string _name)
	: Component(_name, 3)
{
	X2_b = make_shared<Xor>(name + "_X2_b");
	X1_b = make_shared<Xnor>(name + "_X1_b");
	Z = make_shared<Xnor>(name + "_Z");
	SE = make_shared<Xnor>(name + "_SE");
	Row_LSB = make_shared<And>(name + "_Row_LSB");
	Neg_cin_nor_1 = make_shared<Nor>(name + "_neg_cin_nor_1");
	Neg_cin_nor_2 = make_shared<Nor>(name + "_neg_cin_nor_2");
	Neg_cin_nor_3 = make_shared<Nor>(name + "_neg_cin_nor_3");
	Neg_cin_or3 = make_shared<Or3>(name + "_neg_cin_or3");
	Neg_cin_and = make_shared<And>(name + "_neg_cin_and");

	nor_1_o = make_shared<Wire>(name + "_neg_cin_nor_1_O");
	nor_2_o = make_shared<Wire>(name + "_neg_cin_nor_2_O");
	nor_3_o = make_shared<Wire>(name + "_neg_cin_nor_3_O");
	or3_o = make_shared<Wire>(name + "_neg_cin_or3_O");
	Neg_cin_nor_1->Connect(PORTS::O, nor_1_o);
	Neg_cin_nor_2->Connect(PORTS::O, nor_2_o);
	Neg_cin_nor_3->Connect(PORTS::O, nor_3_o);
	Neg_cin_or3->Connect(PORTS::A, nor_1_o);
	Neg_cin_or3->Connect(PORTS::B, nor_2_o);
	Neg_cin_or3->Connect(PORTS::C, nor_3_o);
	Neg_cin_or3->Connect(PORTS::O, or3_o);
	Neg_cin_and->Connect(PORTS::B, or3_o);
}

void BoothEncoderRadix4::Update(bool propagating) {
	if (needs_update || !propagating) {
		for (size_t i = 0; i < longest_path; ++i) {
			X1_b->Update(propagating);
			X2_b->Update(propagating);
			Z->Update(propagating);
			Row_LSB->Update(propagating);
			SE->Update(propagating);
			Neg_cin_nor_1->Update(propagating);
			Neg_cin_nor_2->Update(propagating);
			Neg_cin_nor_3->Update(propagating);
			Neg_cin_or3->Update(propagating);
			Neg_cin_and->Update(propagating);
		}

		if (print_debug) {
			PrintDebug();
		}

		needs_update = false;
	}
}

void BoothEncoderRadix4::Connect(PORTS port, const wire_t &wire, size_t index) {
	auto error_undefined_port = [&](const auto &wire) {
		cout << "[Error] Trying to connect wire \"" << wire->GetName()
			 << "\" to undefined port of BoothEncoderRadix4 "
			 << "\"" << name << "\".\n";
		exit(1);
	};

	switch (port) {
	/* Inputs */
	case PORTS::X_2I:
		X1_b->Connect(PORTS::B, wire);
		X2_b->Connect(PORTS::B, wire);
		Z->Connect(PORTS::B, wire);
		Neg_cin_nor_1->Connect(PORTS::A, wire);
		Neg_cin_nor_3->Connect(PORTS::B, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::X_2I_MINUS_ONE:
		X1_b->Connect(PORTS::A, wire);
		X2_b->Connect(PORTS::A, wire);
		Neg_cin_nor_1->Connect(PORTS::B, wire);
		Neg_cin_nor_2->Connect(PORTS::A, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::X_2I_PLUS_ONE:
		Z->Connect(PORTS::A, wire);
		Neg_cin_and->Connect(PORTS::A, wire);
		SE->Connect(PORTS::B, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::Y_LSB:
		Row_LSB->Connect(PORTS::B, wire);
		Neg_cin_nor_2->Connect(PORTS::B, wire);
		Neg_cin_nor_3->Connect(PORTS::A, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::Y_MSB:
		SE->Connect(PORTS::A, wire);
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
		SE->Connect(PORTS::O, wire);
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
		cout << "[Error] Wire bundle \"" << wires->GetName()
			 << " accessed with index " << wire_idx
			 << " but has size " << wires->GetSize() << ".\n";
		exit(1);
	}

	const wire_t &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const wire_t BoothEncoderRadix4::GetWire(PORTS port, size_t index) const {
	auto error_undefined_port = [&]() {
		cout << "[Error] Trying to get wire of undefined port of BoothEncoderRadix4 "
			 << "\"" << name << "\".\n";
		exit(1);
	};

	switch (port) {
	/* Inputs */
	case PORTS::X_2I: return X1_b->GetWire(PORTS::B);
	case PORTS::X_2I_MINUS_ONE: return X1_b->GetWire(PORTS::A);
	case PORTS::X_2I_PLUS_ONE: return Z->GetWire(PORTS::A);
	case PORTS::Y_LSB: return Row_LSB->GetWire(PORTS::B);
	case PORTS::Y_MSB: return SE->GetWire(PORTS::A);
	/* Outputs */
	case PORTS::NEG: return Z->GetWire(PORTS::A);
	case PORTS::ROW_LSB: return Row_LSB->GetWire(PORTS::O);
	case PORTS::X1_b: return X1_b->GetWire(PORTS::O);
	case PORTS::X2_b: return X2_b->GetWire(PORTS::O);
	case PORTS::SE: return SE->GetWire(PORTS::O);
	case PORTS::Z: return Z->GetWire(PORTS::O);
	case PORTS::NEG_CIN: return Neg_cin_and->GetWire(PORTS::O);
	default:
		error_undefined_port();
		return wire_t();
	}
}

void BoothEncoderRadix4::PrintDebug() const {
	cout << '\n' << name << ":\n";

	const auto &x_2i = X1_b->GetWire(PORTS::B);
	const auto &x_2i_m1 = X1_b->GetWire(PORTS::A);
	const auto &x_2i_p1 = Z->GetWire(PORTS::A);
	const auto &y_lsb = Row_LSB->GetWire(PORTS::B);
	const auto &y_msb = SE->GetWire(PORTS::A);
	const auto &row_lsb = Row_LSB->GetWire(PORTS::O);
	const auto &x1_b = X1_b->GetWire(PORTS::O);
	const auto &x2_b = X2_b->GetWire(PORTS::O);
	const auto &se = SE->GetWire(PORTS::O);
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

	cout << '\n';
}
