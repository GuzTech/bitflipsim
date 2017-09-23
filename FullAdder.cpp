#include "main.h"

/*
  Full-adder implementation. Keeps track of toggle rates of internal
  wires iw_1, iw_2, and iw_3.

  Schematic:

   A ----+-|X|   iw_1
         | |O|-----+----|X|
   B --+---|R|     |    |O|---------------- S
 Cin -----------+-------|R|
     | |        |  |
     | |        |  |----|A| iw_2
     | |        |       |N|------|
     | |        |-------|D|      |---|O|
     | |                             | |--- Cout
     | |----------------|A| iw_3 |---|R|
     |                  |N|------|
     |------------------|D|
 */

void FullAdder::Update() {
	if (needs_update) {
		bool i_A, i_B, i_C, o_S, o_C;

		// Get the wire values.
		i_A = A ? A->GetValue() : false;
		i_B = B ? B->GetValue() : false;
		i_C = Cin ? Cin->GetValue() : false;

		// Update the current state.
		iw_1_curr = i_A ^ i_B;
		iw_3_curr = i_A & i_B;
		iw_2_curr = iw_1_curr & i_C;
		o_S = iw_1_curr ^ i_C;
		o_C = iw_2_curr | iw_3_curr;

		// Set the output values.
		if (S) {
			S->SetValue(o_S);
		}
		if (Cout) {
			Cout->SetValue(o_C);
		}

		// Count number of toggles.
		if (iw_1_curr != iw_1_prev) toggle_count++;
		if (iw_2_curr != iw_2_prev) toggle_count++;
		if (iw_3_curr != iw_3_prev) toggle_count++;

		// Save the current state.
		iw_1_prev = iw_1_curr;
		iw_2_prev = iw_2_curr;
		iw_3_prev = iw_3_curr;

		needs_update = false;
	}
}

void FullAdder::Connect(PORTS port, wire_t wire) {
	switch (port) {
	case PORTS::A:    A    = wire; wire->SetDrives(this->shared_from_base<FullAdder>()); break;
	case PORTS::B:    B    = wire; wire->SetDrives(this->shared_from_base<FullAdder>()); break;
	case PORTS::Cin:  Cin  = wire; wire->SetDrives(this->shared_from_base<FullAdder>()); break;
	case PORTS::S:    S    = wire; break;
	case PORTS::Cout: Cout = wire; break;
	}
}

std::vector<wire_t> FullAdder::GetWires() {
	return {A, B, Cin, S, Cout};
}
