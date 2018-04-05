#include "main.h"

/*
 Signed-magnitude to two's complement converter.

 Schematic:


*/

bool SmagTo2C::entityGenerated = false;

SmagTo2C::SmagTo2C(string _name, size_t _num_bits)
	: Component(_name)
	, num_bits(_num_bits)
{
	// Reserve space.
	xors.reserve(num_bits);
	adders.reserve(num_bits);

	// Create the wries between the XORs and adders.
	auto wb_xor = make_shared<WireBundle>(_name + "_xor_to_ha_A", num_bits - 1);
	wb_xor->Init();
	for (auto &w : wb_xor->GetWires()) {
		internal_wires.emplace_back(w);
	}

	// Create the wires between adders.
	auto wb_ha = make_shared<WireBundle>(_name + "_ha_C_to_ha_B", num_bits);
	wb_ha->Init();
	for (auto &w : wb_ha->GetWires()) {
		internal_wires.emplace_back(w);
	}

	// Create the XORs.
	for (size_t i = 0; i < (num_bits - 1); ++i) {
		string xor_name(_name);
		xor_name += "_xor_";
		xor_name += to_string(i);

		const auto xor_c = make_shared<Xor>(xor_name);
		xor_c->Connect(PORTS::O, (*wb_xor)[i]);
		xors.emplace_back(xor_c);
	}

	// Create the half adders.
	for (size_t i = 0; i < num_bits; ++i) {
		string ha_name(_name);
		ha_name += "_ha_";
		ha_name += to_string(i);

		const auto ha = make_shared<HalfAdder>(ha_name);
		if (i < (num_bits - 1)) {
			ha->Connect(PORTS::A, (*wb_xor)[i]);
		}
		adders.emplace_back(ha);
	}

	// Connect the wires between the half adders.
	for (size_t i = 0; i < (num_bits - 1); ++i) {
		adders[i]->Connect(PORTS::Cout, (*wb_ha)[i]);
		adders[i + 1]->Connect(PORTS::B, (*wb_ha)[i]);
	}
}

void SmagTo2C::Update(bool propagating) {
	if (needs_update || !propagating) {
		for (size_t i = 0; i < longest_path; ++i) {
			for (const auto &xor_c : xors) {
				xor_c->Update(propagating);
			}

			for (const auto &ha : adders) {
				ha->Update(propagating);
			}
		}

		needs_update = false;
	}
}

void SmagTo2C::Connect(PORTS port, const wire_t &wire, size_t index) {
	if (index >= num_bits) {
		Error("Index " + to_string(index) + " out of bounds for SmagTo2C \"" + name + "\".\n");
	}

	switch (port) {
	case PORTS::A:
		if (index == (num_bits - 1)) {
			for (const auto &x : xors) {
				x->Connect(PORTS::B, wire);
			}

			adders.front()->Connect(PORTS::B, wire);
			adders.back()->Connect(PORTS::A, wire);
		} else {
			xors[index]->Connect(PORTS::A, wire);
		}
		input_wires.emplace_back(wire);
		break;
	case PORTS::O:
		adders[index]->Connect(PORTS::O, wire);
		output_wires.emplace_back(wire);
		break;	
	default:
		Error("Trying to connect to undefined port of SmagTo2C \"" + name + "\".\n");
	}
}

void SmagTo2C::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		Error("Wire bundle \"" + wires->GetName() + " accessed with index " + to_string(wire_idx)
			  + " but has size " + to_string(wires->GetSize()) + ".\n");
	}

	const wire_t &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const wire_t SmagTo2C::GetWire(PORTS port, size_t index) const {
	if (index >= num_bits) {
		Error("Index " + to_string(index) + " out of bounds for SmagTo2C \"" + name + "\".\n");
	}
	
	switch (port) {
	case PORTS::A:
		if (index == (num_bits - 1)) {
			return xors.back()->GetWire(PORTS::B);
		} else {
			return xors[index]->GetWire(port);
		}
	case PORTS::O:
		return adders[index]->GetWire(port);
	default:
		Error("Trying to retrieve undefined port of SmagTo2C \"" + name + "\".\n");
	}
}

const PORT_DIR SmagTo2C::GetPortDirection(PORTS port) const {
	switch (port) {
	case PORTS::A:
		return PORT_DIR::INPUT;
	case PORTS::O:
		return PORT_DIR::OUTPUT;
	default:
		Error("Trying to get port direction of undefined port in SmagTo2C \"" + name + "\".\n");
	}
}

void SmagTo2C::GenerateVHDLEntity(const string &path) const {
	// We only need to do it once, since all instances of the And gate are identical.
	if (!entityGenerated) {
		string output;
		//TemplateDictionary entity("And");
		//ExpandTemplate("src/templates/VHDL/And_entity.tpl", DO_NOT_STRIP, &entity, &output);

		//auto outfile = ofstream(path + "/And.vhd");
		//outfile << output;
		//outfile.close();

		//entityGenerated = true;
	}
}

const string SmagTo2C::GenerateVHDLInstance() const {
	string output;
	TemplateDictionary inst("SmagTo2C");


	GenerateAssignments(PORTS::A, num_bits, "A", inst);
	GenerateAssignments(PORTS::O, num_bits, "O", inst, true);

	inst.SetValue("NAME", name);
	inst.SetValue("SIZE", to_string(num_bits));
	ExpandTemplate("src/templates/VHDL/SmagTo2C_inst.tpl", DO_NOT_STRIP, &inst, &output);

	return output;
}
