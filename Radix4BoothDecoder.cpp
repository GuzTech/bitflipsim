#include "main.h"

Radix4BoothDecoder::Radix4BoothDecoder(string _name,
									   size_t _num_bits)
	: Component(_name, 3)
	, num_bits(_num_bits)
	, num_bits_O(_num_bits - 1)
{
	if (num_bits < 2) {
		cout << "[Error] Number of bits in Radix4BoothDecoder \"" << _name
			 << "\" must be at least 2.\n";
		exit(1);
	}

	GenerateDecoderHardware();
}

void Radix4BoothDecoder::Update(bool propagating) {
	if (needs_update || !propagating) {
		for (size_t i = 0; i < longest_path; ++i) {
			for (size_t j = 0; j < num_bits; ++j) {
				yj_neg[j]->Update(propagating);
			}
			for (size_t j = 0; j < num_bits_O; ++j) {
				yj_x1b[j]->Update(propagating);
			}
			for (size_t j = 0; j < num_bits_O; ++j) {
				yj_m1_z_x2b[j]->Update(propagating);
			}
			for (size_t j = 0; j < num_bits_O; ++j) {
				ppt_j[j]->Update(propagating);
			}
		}

		needs_update = false;
	}
}

void Radix4BoothDecoder::Connect(PORTS port, const wire_t &wire, size_t index) {
	if (port == PORTS::Yj && index >= num_bits) {
		cout << "[Error] Index " << index << " of port Yj is out of "
			 << "bounds for Radix4BoothDecoder \"" << name << "\".\n";
		exit(1);
	} else if (port == PORTS::O && index >= num_bits_O) {
		cout << "[Error] Index " << index << " of port O is out of "
			 << "bounds for Radix4BoothDecoder \"" << name << "\".\n";
		exit(1);
	}

	auto error_undefined_port = [&](const auto &wire) {
		cout << "[Error] Trying to connect wire \"" << wire->GetName()
			 << "\" to undefined port of Radix4BoothDecoder "
			 << "\"" << name << "\".\n";
		exit(1);
	};

	switch (port) {
	case PORTS::Yj:
		yj_neg[index]->Connect(PORTS::A, wire);
		break;
	case PORTS::NEG:
		for (size_t i = 0; i < num_bits; ++i) {
			yj_neg[i]->Connect(PORTS::B, wire);
		}
		break;
	case PORTS::X1_b:
		for (size_t i = 0; i < num_bits_O; ++i) {
			yj_x1b[i]->Connect(PORTS::B, wire);
		}
		break;
	case PORTS::X2_b:
		for (size_t i = 0; i < num_bits_O; ++i) {
			yj_m1_z_x2b[i]->Connect(PORTS::C, wire);
		}
		break;
	case PORTS::Z:
		for (size_t i = 0; i < num_bits_O; ++i) {
			yj_m1_z_x2b[i]->Connect(PORTS::B, wire);
		}
		break;
	case PORTS::O:
		ppt_j[index]->Connect(PORTS::O, wire);
		output_wires.emplace_back(wire);
		break;
	default:
		error_undefined_port(wire);
	}
}

void Radix4BoothDecoder::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		cout << "[Error] Wire bundle \"" << wires->GetName()
			 << " accessed with index " << wire_idx
			 << " but has size " << wires->GetSize() << ".\n";
		exit(1);
	}

	const wire_t &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const vector<wire_t> Radix4BoothDecoder::GetWires() const {
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
	wires.insert(wires.end(),
				 output_wires.begin(),
				 output_wires.end());

	return wires;
}

const vector<wire_t> Radix4BoothDecoder::GetInputWires() const {
	vector<wire_t> input_wires;

	// Add the A inputs of the XNOR gates.
	for (size_t i = 0; i < num_bits; ++i) {
		const auto &xnor_i = yj_neg[i];
		const auto &wire = xnor_i->GetWire(PORTS::A);
		input_wires.emplace_back(wire);
	}

	// Add the NEG input which is connected to the B input of all XNOR gates.
	input_wires.emplace_back(yj_neg[0]->GetWire(PORTS::B));

	// Add the X1_b input which is connected to the B input of all OR gates.
	input_wires.emplace_back(yj_x1b[0]->GetWire(PORTS::B));

	// Add the X2_b input which is connected to the C input of all OR3 gates.
	input_wires.emplace_back(yj_m1_z_x2b[0]->GetWire(PORTS::C));

	// Add the Z input which is connected to the B input of all OR3 gates.
	input_wires.emplace_back(yj_m1_z_x2b[0]->GetWire(PORTS::B));

	return input_wires;
}

const wire_t Radix4BoothDecoder::GetWire(PORTS port, size_t index) const {
	return nullptr;
}

void Radix4BoothDecoder::GenerateDecoderHardware() {
	// Generate input XNOR gates.
	string name_prefix = name + "_yj_neg_";
	for (size_t i = 0; i < num_bits; ++i) {
		yj_neg.emplace_back(make_shared<Xnor>(name_prefix + to_string(i)));
	}

	// Generate the yj_x1b OR gates.
	name_prefix = name + "_yj_x1b_";
	for (size_t i = 0; i < num_bits_O; ++i) {
		yj_x1b.emplace_back(make_shared<Or>(name_prefix + to_string(i)));
	}

	// Generate the yj_m1_z_x2b OR3 gates.
	name_prefix = name + "_yj_m1_z_x2b_";
	for (size_t i = 0; i < num_bits_O; ++i) {
		yj_m1_z_x2b.emplace_back(make_shared<Or3>(name_prefix + to_string(i)));
	}

	// Generate the ppt_j NAND gates.
	name_prefix = name + "_ppt_j_";
	for (size_t i = 0; i < num_bits_O; ++i) {
		ppt_j.emplace_back(make_shared<Nand>(name_prefix + to_string(i)));
	}

	// Connect the output of the XNOR gates to the OR and OR3 gates.
	name_prefix = name + "_xj_neg_";
	for (size_t i = 0; i < num_bits; ++i) {
		const auto wire = make_shared<Wire>(name_prefix + to_string(i) + "_O");
		yj_neg[i]->Connect(PORTS::O, wire);

		if (i != 0) {
			yj_x1b[i - 1]->Connect(PORTS::A, wire);
		}

		if (i != (num_bits - 1)) {
			yj_m1_z_x2b[i]->Connect(PORTS::A, wire);
		}

		internal_wires.emplace_back(wire);
	}

	// Connect the output of the OR gates to the NAND gates.
	name_prefix = name + "_or_";
	for (size_t i = 0; i < num_bits_O; ++i) {
		const auto wire = make_shared<Wire>(name_prefix + to_string(i) + "_O");
		yj_x1b[i]->Connect(PORTS::O, wire);
		ppt_j[i]->Connect(PORTS::A, wire);
		internal_wires.emplace_back(wire);
	}

	// Connect the output of the OR3 gates to the NAND gates.
	name_prefix = name + "_or3_";
	for (size_t i = 0; i < num_bits_O; ++i) {
		const auto wire = make_shared<Wire>(name_prefix + to_string(i) + "_O");
		yj_m1_z_x2b[i]->Connect(PORTS::O, wire);
		ppt_j[i]->Connect(PORTS::B, wire);
		internal_wires.emplace_back(wire);
	}
}
