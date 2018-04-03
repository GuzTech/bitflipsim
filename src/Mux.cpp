#include "main.h"

/*
  1 bit multiplexer implementation. When S is 0, then A
  is routed to O, else B is routed to O.

  Schematic:

  A ---|M|
       |U|--- O
  B ---|X|
        |
  S -----
*/

void Mux::Update(bool propagating) {
	if (needs_update || !propagating) {
		bool inA, inB, inS;

		inA = A ? A->GetValue() : false;
		inB = B ? B->GetValue() : false;
		inS = S ? B->GetValue() : false;

		if (O) {
			O->SetValue(inS ? inB : inA, propagating);
		}

		needs_update = false;
	}
}

void Mux::Connect(PORTS port, const wire_t &wire, size_t index) {
	switch (port) {
	case PORTS::A:
		A = wire;
		wire->AddOutput(this->shared_from_base<Mux>());
		input_wires.emplace_back(wire);
		break;
	case PORTS::B:
		B = wire;
		wire->AddOutput(this->shared_from_base<Mux>());
		input_wires.emplace_back(wire);
		break;
	case PORTS::S:
		S = wire;
		wire->AddOutput(this->shared_from_base<Mux>());
		input_wires.emplace_back(wire);
		break;
	case PORTS::O:
		O = wire;
		output_wires.emplace_back(O);
		break;
	default:
		Error("Trying to connect to undefined port of Mux \"" + name + "\".\n");
	}
}

void Mux::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		Error("Wire bundle \"" + wires->GetName() + " accessed with index " + to_string(wire_idx)
			  + " but has size " + to_string(wires->GetSize()) + ".\n");
	}

	const wire_t &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const wire_t Mux::GetWire(PORTS port, size_t index) const {
	switch (port) {
	case PORTS::A: return A;
	case PORTS::B: return B;
	case PORTS::S: return S;
	case PORTS::O: return O;
	default:
		Error("Trying to retrieve undefined port of Mux \"" + name + "\".\n");
	}
}

const PORT_DIR Mux::GetPortDirection(PORTS port) const {
	switch (port) {
	case PORTS::A:
	case PORTS::B:
	case PORTS::S:
		return PORT_DIR::INPUT;
	case PORTS::O:
		return PORT_DIR::OUTPUT;
	default:
		Error("Trying to get port direction of undefined port in Mux \"" + name + "\".\n");
	}
}

void Mux::GenerateVHDLEntity(const string &path) const {

}

const string Mux::GenerateVHDLInstance() const {
	return string("");
}
