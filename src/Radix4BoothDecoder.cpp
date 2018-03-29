#include "main.h"

bool Radix4BoothDecoder::entityGenerated = false;

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
			for (const auto &comp : yj_neg) {
				comp->Update(propagating);
			}
			for (const auto &comp : yj_x1b) {
				comp->Update(propagating);
			}
			for (const auto &comp : yj_m1_z_x2b) {
				comp->Update(propagating);
			}
			for (const auto &comp : ppt_j) {
				comp->Update(propagating);
			}
		}

		if (!propagating && print_debug) {
			PrintDebug();
		}

		needs_update = false;
	}
}

void Radix4BoothDecoder::Connect(PORTS port, const wire_t &wire, size_t index) {
	CheckIfIndexIsInRange(port, index);

	auto error_undefined_port = [&](const auto &wire) {
		cout << "[Error] Trying to connect wire \"" << wire->GetName()
			 << "\" to undefined port of Radix4BoothDecoder "
			 << "\"" << name << "\".\n";
		exit(1);
	};

	switch (port) {
	case PORTS::Yj:
		yj_neg[index]->Connect(PORTS::A, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::NEG:
		for (const auto &comp : yj_neg) {
			comp->Connect(PORTS::B, wire);
		}
		input_wires.emplace_back(wire);
		break;
	case PORTS::X1_b:
		for (const auto &comp : yj_x1b) {
			comp->Connect(PORTS::B, wire);
		}
		input_wires.emplace_back(wire);
		break;
	case PORTS::X2_b:
		for (const auto &comp : yj_m1_z_x2b) {
			comp->Connect(PORTS::C, wire);
		}
		input_wires.emplace_back(wire);
		break;
	case PORTS::Z:
		for (const auto &comp : yj_m1_z_x2b) {
			comp->Connect(PORTS::B, wire);
		}
		input_wires.emplace_back(wire);
		break;
	case PORTS::PPTj:
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

	const auto &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const wire_t Radix4BoothDecoder::GetWire(PORTS port, size_t index) const {
	CheckIfIndexIsInRange(port, index);

	auto error_undefined_port = [&]() {
		cout << "[Error] Trying to get wire of undefined port of Radix4BoothDecoder "
			 << "\"" << name << "\".\n";
		exit(1);
	};

	switch (port) {
	case PORTS::Yj:   return yj_neg[index]->GetWire(PORTS::A);
	case PORTS::NEG:  return yj_neg[index]->GetWire(PORTS::B);
	case PORTS::X1_b: return yj_x1b[index]->GetWire(PORTS::B);
	case PORTS::X2_b: return yj_m1_z_x2b[index]->GetWire(PORTS::C);
	case PORTS::Z:    return yj_m1_z_x2b[index]->GetWire(PORTS::B);
	case PORTS::PPTj: return ppt_j[index]->GetWire(PORTS::O);
	default:
		error_undefined_port();
		return nullptr;
	}
}

const PORT_DIR Radix4BoothDecoder::GetPortDirection(PORTS port) const {
	switch (port) {
	case PORTS::Yj:
	case PORTS::NEG:
	case PORTS::X1_b:
	case PORTS::X2_b:
	case PORTS::Z:
		return PORT_DIR::INPUT;
	case PORTS::PPTj:
		return PORT_DIR::OUTPUT;
	default:
		cout << "[Error] Trying to get port direction of undefined port in Radix4BoothDecoder "
			 << "\"" << name << "\".\n";
		exit(1);
	}
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

void Radix4BoothDecoder::CheckIfIndexIsInRange(PORTS port, size_t index) const {
	if (port == PORTS::Yj && index >= num_bits) {
		cout << "[Error] Index " << index << " of port Yj is out of "
			 << "bounds for Radix4BoothDecoder \"" << name << "\".\n";
		exit(1);
	} else if (port == PORTS::O && index >= num_bits_O) {
		cout << "[Error] Index " << index << " of port O is out of "
			 << "bounds for Radix4BoothDecoder \"" << name << "\".\n";
		exit(1);
	}
}

void Radix4BoothDecoder::PrintDebug() const {
	cout << "\n========================================\n";
	cout << name << ":\n";

	for (size_t i = 0; i < num_bits; ++i) {
		const auto &wire = yj_neg[i]->GetWire(PORTS::A);
		if (wire) {
			cout << "yj_neg[" << i << "] (" << wire->GetName() << "): " << (*wire)() << '\n';
		}
	}

	const auto &x1b = yj_x1b.front()->GetWire(PORTS::B);
	const auto &x2b = yj_m1_z_x2b.front()->GetWire(PORTS::C);
	const auto &neg = yj_neg.front()->GetWire(PORTS::B);
	const auto &z = yj_m1_z_x2b.front()->GetWire(PORTS::B);
	if (x1b) cout << "X1_b (" << x1b->GetName() << "): " << (*x1b)() << '\n';
	if (x2b) cout << "X2_b (" << x2b->GetName() << "): " << (*x2b)() << '\n';
	if (neg) cout << "NEG (" << neg->GetName() << "): " << (*neg)() << '\n';
	if (z)   cout << "Z (" << z->GetName() << "): " << (*z)() << '\n';

	for (const auto &o : ppt_j) {
		const auto &ow = o->GetWire(PORTS::O);
		if (ow) {
			cout << o->GetName() << " (" << ow->GetName() << "): " << (*ow)() << '\n';
		}
	}

	cout << "========================================\n";
}

void Radix4BoothDecoder::GenerateVHDLEntity(const string &path) const {
	// We only need to generate it once, since all
	// instances of Radix4BoothDecoder are identical.
	if (!entityGenerated) {
		string output;
		TemplateDictionary entity("Radix4BoothDecoder");
		ExpandTemplate("src/templates/VHDL/Radix4BoothDecoder_entity.tpl", DO_NOT_STRIP, &entity, &output);

		auto outfile = ofstream(path + "/Radix4BoothDecoder.vhd");
		outfile << output;
		outfile.close();

		entityGenerated = true;
	}
}

const string Radix4BoothDecoder::GenerateVHDLInstance() const {
	return string("");
}
