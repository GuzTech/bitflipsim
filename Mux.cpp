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

void Mux::Update() {
	if (needs_update) {
		bool inA, inB, inS;

		inA = A ? A->GetValue() : false;
		inB = B ? B->GetValue() : false;
		inS = S ? B->GetValue() : false;

		if (O) {
			O->SetValue(inS ? inB : inA);
		}

		needs_update = false;
	}
}

void Mux::Connect(PORTS port, wire_t wire) {
	switch(port) {
	case PORTS::A: A = wire; wire->AddOutput(this->shared_from_base<Mux>()); break;
	case PORTS::B: B = wire; wire->AddOutput(this->shared_from_base<Mux>()); break;
	case PORTS::S: S = wire; wire->AddOutput(this->shared_from_base<Mux>()); break;
	case PORTS::O: O = wire; break;
	}
}

std::vector<wire_t> Mux::GetWires() {
	return {A, B, S, O};
}
