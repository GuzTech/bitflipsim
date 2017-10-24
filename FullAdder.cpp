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

void FullAdder::Update(bool propagating) {
	if (needs_update || !propagating) {
		bool i_A, i_B, i_C, o_O, o_C;

		// Get the wire values.
		i_A = A ? A->GetValue() : false;
		i_B = B ? B->GetValue() : false;
		i_C = Cin ? Cin->GetValue() : false;

		// Update the current state.
		iw_1_curr = i_A ^ i_B;
		iw_3_curr = i_A & i_B;
		iw_2_curr = iw_1_curr & i_C;
		o_O = iw_1_curr ^ i_C;
		o_C = iw_2_curr | iw_3_curr;

		// Set the output values.
		if (O) {
			O->SetValue(o_O, propagating);
		}
		if (Cout) {
			Cout->SetValue(o_C, propagating);
		}

		if (!propagating) {
			// Count number of toggles.
			if (iw_1_curr != iw_1_prev) {
				toggle_count++;
			}
			if (iw_2_curr != iw_2_prev) {
				toggle_count++;
			}
			if (iw_3_curr != iw_3_prev) {
				toggle_count++;
			}

			// Save the current state.
			iw_1_prev = iw_1_curr;
			iw_2_prev = iw_2_curr;
			iw_3_prev = iw_3_curr;
		}

		needs_update = false;
	}
}

void FullAdder::Connect(PORTS port, const wire_t &wire, size_t index) {
	switch (port) {
	case PORTS::A:    A    = wire; wire->AddOutput(this->shared_from_base<FullAdder>()); break;
	case PORTS::B:    B    = wire; wire->AddOutput(this->shared_from_base<FullAdder>()); break;
	case PORTS::Cin:  Cin  = wire; wire->AddOutput(this->shared_from_base<FullAdder>()); break;
	case PORTS::O:    O    = wire; wire->SetInput(this->shared_from_base<FullAdder>()); break;
	case PORTS::Cout: Cout = wire; wire->SetInput(this->shared_from_base<FullAdder>()); break;
	default:
		cout << "[Error] Trying to connect to undefined port of FullAdder "
			 << "\"" << name << "\"\n";
		exit(1);
	}
}

void FullAdder::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		cout << "[Error] Wire bundle \"" << wires->GetName()
			 << " accessed with index " << wire_idx
			 << " but has size " << wires->GetSize() << '\n';
		exit(1);
	}

	const wire_t &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

vector<wire_t> FullAdder::GetWires() {
	return {A, B, Cin, O, Cout};
}

vector<wire_t> FullAdder::GetInputWires() {
	return {A, B, Cin};
}

vector<wire_t> FullAdder::GetOutputWires() {
	return {O, Cout};
}

wire_t FullAdder::GetWire(PORTS port, size_t index) {
	switch (port) {
	case PORTS::A:    return A;
	case PORTS::B:    return B;
	case PORTS::Cin:  return Cin;
	case PORTS::O:    return O;
	case PORTS::Cout: return Cout;
	default:
		cout << "[Error] Trying to retrieve an undefined port of FullAdder "
			 << "\"" << name << "\"\n";
		exit(1);
	}
}
