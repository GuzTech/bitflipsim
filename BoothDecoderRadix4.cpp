#include "main.h"

BoothDecoderRadix4::BoothDecoderRadix4(string _name)
	: Component(_name, 3)
{
	yj_neg = make_shared<Xnor>(name + "_yj_neg");
	yj_m1_neg = make_shared<Xnor>(name + "_yj_m1_neg");
	yj_x1b = make_shared<Or>(name + "_yj_x1b");
	yj_m1_z_x2b = make_shared<Or3>(name + "_yj_m1_x_x2b");
	ppt_j = make_shared<Nand>(name + "_ppt_j");

	yj_neg_o = make_shared<Wire>(name + "_yj_neg_O");
	yj_m1_neg_o = make_shared<Wire>(name + "_yj_m1_neg_O");
	yj_neg->Connect(PORTS::O, yj_neg_o);
	yj_m1_neg->Connect(PORTS::O, yj_m1_neg_o);
	yj_x1b->Connect(PORTS::A, yj_neg_o);
	yj_m1_z_x2b->Connect(PORTS::B, yj_m1_neg_o);

	yj_x1b_o = make_shared<Wire>(name + "_yj_x1b_O");
	yj_m1_z_x2b_o = make_shared<Wire>(name + "_yj_m1_z_x2b_O");
	yj_x1b->Connect(PORTS::O, yj_x1b_o);
	yj_m1_z_x2b->Connect(PORTS::O, yj_m1_z_x2b_o);
	ppt_j->Connect(PORTS::A, yj_x1b_o);
	ppt_j->Connect(PORTS::B, yj_m1_z_x2b_o);
}

void BoothDecoderRadix4::Update(bool propagating) {
	if (needs_update || !propagating) {
		yj_neg->Update(propagating);
		yj_m1_neg->Update(propagating);
		yj_x1b->Update(propagating);
		yj_m1_z_x2b->Update(propagating);
		ppt_j->Update(propagating);

		needs_update = false;
	}
}

void BoothDecoderRadix4::Connect(PORTS port, const wire_t &wire, size_t index) {
	auto error_undefined_port = [&](const auto &wire) {
		cout << "[Error] Trying to connect wire \"" << wire->GetName()
			 << "\" to undefined port of BoothDecoderRadix4 "
			 << "\"" << name << "\".\n";
		exit(1);
	};

	switch (port) {
	/* Inputs */
	case PORTS::X1_b:
		yj_x1b->Connect(PORTS::B, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::X2_b:
		yj_m1_z_x2b->Connect(PORTS::C, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::Z:
		yj_m1_z_x2b->Connect(PORTS::A, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::NEG:
		yj_neg->Connect(PORTS::B, wire);
		yj_m1_neg->Connect(PORTS::B, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::Yj:
		yj_neg->Connect(PORTS::A, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::Yj_m1:
		yj_m1_neg->Connect(PORTS::A, wire);
		input_wires.emplace_back(wire);
		break;
	/* Outputs */
	case PORTS::PPTj:
		ppt_j->Connect(PORTS::O, wire);
		output_wires.emplace_back(wire);
		break;
	default:
		error_undefined_port(wire);
	}
}

void BoothDecoderRadix4::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		cout << "[Error] Wire bundle \"" << wires->GetName()
			 << " accessed with index " << wire_idx
			 << " but has size " << wires->GetSize() << ".\n";
		exit(1);
	}

	const wire_t &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const wire_t BoothDecoderRadix4::GetWire(PORTS port, size_t index) const {
	return wire_t();
}
