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

void Mux::Connect(PORTS port, wire_t wire, std::size_t index) {
	switch(port) {
	case PORTS::A: A = wire; wire->AddOutput(this->shared_from_base<Mux>()); break;
	case PORTS::B: B = wire; wire->AddOutput(this->shared_from_base<Mux>()); break;
	case PORTS::S: S = wire; wire->SetInput(this->shared_from_base<Mux>()); break;
	case PORTS::O: O = wire; wire->SetInput(this->shared_from_base<Mux>()); break;
	default:
		std::cout << "[Error] Trying to connect to undefined port of Mux "
				  << "\"" << name << "\"n";
		exit(1);
	}
}

std::vector<wire_t> Mux::GetWires() {
	return {A, B, S, O};
}

std::vector<wire_t> Mux::GetInputWires() {
	return {A, B};
}

std::vector<wire_t> Mux::GetOutputWires() {
	return {S, O};
}
