#include "main.h"

CarrySaveAdder::CarrySaveAdder(string _name,
							   size_t _num_bits)
	: Component(_name)
	, num_bits(_num_bits)
{
	if (num_bits == 0) {
		cout << "[Error] Number of bits in CarrySaveAdder \"" << _name
			 << "\" must be at least 1.\n";
		exit(1);
	}

	for (size_t i = 0; i < num_bits; ++i) {
		full_adders.emplace_back(make_shared<FullAdder>(name + "_fa_" + to_string(i)));
	}
}

void CarrySaveAdder::Update(bool propagating) {
	if (needs_update || !propagating) {
		for (const auto &fa : full_adders) {
			fa->Update(propagating);
		}

		needs_update = false;
	}
}

void CarrySaveAdder::Connect(PORTS port, const wire_t &wire, size_t index) {
	CheckIfIndexIsInRange(port, index);

	auto error_undefined_port = [&](const auto &wire) {
		cout << "[Error] Trying to connect wire \"" << wire->GetName()
			 << "\" to undefined port of CarrySaveAdder "
			 << "\"" << name << "\".\n";
		exit(1);
	};

	switch (port) {
	case PORTS::A:
		full_adders[index]->Connect(PORTS::A, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::B:
		full_adders[index]->Connect(PORTS::B, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::Cin:
		full_adders[index]->Connect(PORTS::Cin, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::O:
		full_adders[index]->Connect(PORTS::O, wire);
		output_wires.emplace_back(wire);
		break;
	case PORTS::Cout:
		full_adders[index]->Connect(PORTS::Cout, wire);
		output_wires.emplace_back(wire);
		break;
	default:
		error_undefined_port(wire);
	}
}

void CarrySaveAdder::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		cout << "[Error] Wire bundle \"" << wires->GetName()
			 << " accessed with index " << wire_idx
			 << " but has size " << wires->GetSize() << ".\n";
		exit(1);
	}

	const auto &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const size_t CarrySaveAdder::GetNumToggles() {
	toggle_count = 0;

	return toggle_count;
}

const wire_t CarrySaveAdder::GetWire(PORTS port, size_t index) const {
	CheckIfIndexIsInRange(port, index);

	auto error_undefined_port = [&]() {
		cout << "[Error] Trying to get wire of undefined port of CarrySaveAdder "
			 << "\"" << name << "\".\n";
		exit(1);
	};

	switch (port) {
	case PORTS::A:
	case PORTS::B:
	case PORTS::Cin:
	case PORTS::O:
	case PORTS::Cout:
		return full_adders[index]->GetWire(port);
	default:
		error_undefined_port();
		return nullptr;
	}
}

void CarrySaveAdder::CheckIfIndexIsInRange(PORTS port, size_t index) const {
	if (port == PORTS::A && index >= num_bits) {
		cout << "[Error] Index " << index << " of port A is out of "
			 << "bounds for CarrySaveAdder \"" << name << "\".\n";
		exit(1);
	} else if (port == PORTS::B && index >= num_bits) {
		cout << "[Error] Index " << index << " of port B is out of "
			 << "bounds for CarrySaveAdder \"" << name << "\".\n";
		exit(1);
	} else if (port == PORTS::Cin && index >= num_bits) {
		cout << "[Error] Index " << index << " of port Cin is out of "
			 << "bounds for CarrySaveAdder \"" << name << "\".\n";
		exit(1);
	} else if (port == PORTS::O && index >= num_bits) {
		cout << "[Error] Index " << index << " of port O is out of "
			 << "bounds for CarrySaveAdder \"" << name << "\".\n";
		exit(1);
	} else if (port == PORTS::Cout && index >= num_bits) {
		cout << "[Error] Index " << index << " of port Cout is out of "
			 << "bounds for CarrySaveAdder \"" << name << "\".\n";
		exit(1);
	}
}
