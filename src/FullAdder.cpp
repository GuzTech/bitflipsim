#include "main.h"

/*
  Full-adder implementation. Keeps track of toggle rates of internal
  wires iw_1, iw_2, and iw_3.

  Schematic:

   A ----+-|X|   iw_1
         | |O|-----+----|X|
   B --+---|R|     |    |O|---------------- O
 Cin -----------+-------|R|
       | |      |  |
       | |      |  |----|A| iw_2
       | |      |       |N|------|
       | |      |-------|D|      |---|O|
       | |                           | |--- Cout
       | |--------------|A| iw_3 |---|R|
       |                |N|------|
       |----------------|D|
 */

bool FullAdder::entityGenerated = false;

FullAdder::FullAdder(string _name)
	: Component(_name, 3)
{
	xor_ab = make_shared<Xor>(_name + "_xor_ab");
	xor_cin = make_shared<Xor>(_name + "_xor_cin");
	and_cin = make_shared<And>(_name + "_and_cin");
	and_ab = make_shared<And>(_name + "_and_ab");
	or_cout = make_shared<Or>(_name + "_or_cout");

	iw_1 = make_shared<Wire>(_name + "_iw_1");
	xor_ab->Connect(PORTS::O, iw_1);
	xor_cin->Connect(PORTS::A, iw_1);
	and_cin->Connect(PORTS::A, iw_1);
	internal_wires.emplace_back(iw_1);

	iw_2 = make_shared<Wire>(_name + "_iw_2");
	and_cin->Connect(PORTS::O, iw_2);
	or_cout->Connect(PORTS::A, iw_2);
	internal_wires.emplace_back(iw_2);

	iw_3 = make_shared<Wire>(_name + "_iw_3");
	and_ab->Connect(PORTS::O, iw_3);
	or_cout->Connect(PORTS::B, iw_3);
	internal_wires.emplace_back(iw_3);
}

void FullAdder::Update(bool propagating) {
	if (needs_update || !propagating) {
		for (size_t i = 0; i < longest_path; ++i) {
			xor_ab->Update(propagating);
			xor_cin->Update(propagating);
			and_cin->Update(propagating);
			and_ab->Update(propagating);
			or_cout->Update(propagating);
		}

		needs_update = false;
	}
}

void FullAdder::Connect(PORTS port, const wire_t &wire, size_t index) {
	switch (port) {
	case PORTS::A:
		xor_ab->Connect(PORTS::A, wire);
		and_ab->Connect(PORTS::A, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::B:
		xor_ab->Connect(PORTS::B, wire);
		and_ab->Connect(PORTS::B, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::Cin:
		xor_cin->Connect(PORTS::B, wire);
		and_cin->Connect(PORTS::B, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::O:
		xor_cin->Connect(PORTS::O, wire);
		output_wires.emplace_back(wire);
		break;
	case PORTS::Cout:
		or_cout->Connect(PORTS::O, wire);
		output_wires.emplace_back(wire);
		break;
	default:
		cout << "[Error] Trying to connect to undefined port of FullAdder "
			 << "\"" << name << "\".\n";
		exit(1);
	}
}

void FullAdder::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		cout << "[Error] Wire bundle \"" << wires->GetName()
			 << " accessed with index " << wire_idx
			 << " but has size " << wires->GetSize() << ".\n";
		exit(1);
	}

	const wire_t &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const wire_t FullAdder::GetWire(PORTS port, size_t index) const {
	switch (port) {
	case PORTS::A: return xor_ab->GetWire(PORTS::A);
	case PORTS::B: return xor_ab->GetWire(PORTS::B);
	case PORTS::Cin: return xor_cin->GetWire(PORTS::B);
	case PORTS::O : return xor_cin->GetWire(PORTS::O);
	case PORTS::Cout: return or_cout->GetWire(PORTS::O);
	default:
		cout << "[Error] Trying to retrieve an undefined port of FullAdder "
			 << "\"" << name << "\".\n";
		exit(1);
	}
}

const PORT_DIR FullAdder::GetPortDirection(PORTS port) const {
	switch (port) {
	case PORTS::A:
	case PORTS::B:
	case PORTS::Cin:
		return PORT_DIR::INPUT;
	case PORTS::Cout:
	case PORTS::O:
		return PORT_DIR::OUTPUT;
	default:
		cout << "[Error] Trying to get port direction of undefined port in FullAdder "
			 << "\"" << name << "\".\n";
		exit(1);
	}
}

void FullAdder::GenerateVHDLEntity(const string &path) const {
	// We only need to do it once, since all instances of the FullAdder are identical.
	if (!entityGenerated) {
		string output;
		TemplateDictionary entity("FullAdder");
		ExpandTemplate("src/templates/VHDL/FullAdder_entity.tpl", DO_NOT_STRIP, &entity, &output);

		auto outfile = ofstream(path + "/FullAdder.vhd");
		outfile << output;
		outfile.close();

		entityGenerated = true;
	}
}

const string FullAdder::GenerateVHDLInstance() const {
	string output;
	TemplateDictionary inst("FullAdder");
	inst.SetValue("NAME", name);
	ExpandTemplate("src/templates/VHDL/FullAdder_inst.tpl", DO_NOT_STRIP, &inst, &output);

	return output;
}
