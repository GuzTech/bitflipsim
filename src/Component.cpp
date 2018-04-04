#include "main.h"

const vector<wire_t> Component::GetWires() const {
	vector<wire_t> wires;

	// Add all input wires.
	wires.insert(wires.end(),
				 input_wires.begin(),
				 input_wires.end());

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

void Component::GenerateAssignments(const PORTS port,
									const size_t port_width,
									const string &signal_name,
									TemplateDictionary &inst,
									const bool last_port) const {
	struct wireData {
		size_t min_idx = 256;
		size_t max_idx = 0;
		size_t port_size = 0;
	};

	map<string, wireData> wire_indices;

	for (size_t i = 0; i < port_width; ++i) {
		const auto &w = GetWire(port, i);
		if (w) {
			const auto &wb = w->GetWireBundle();

			if (wb) {
				wire_indices[wb->GetName()] = wireData{wb->GetFromMinIdx(), wb->GetFromMaxIdx(), wb->GetSize()};
			} else {
				const auto &p = wire_indices[w->GetName()];
				size_t min_idx = min(p.min_idx, i);
				size_t max_idx = max(p.max_idx, i);
				wire_indices[w->GetName()] = wireData{min_idx, max_idx, 1};
			}
		}
	}

	string signal = "";

	if (wire_indices.size()) {
		// At least one wire is connected to this port.
		for (const auto &[name, indices] : wire_indices) {
			if (indices.min_idx == indices.max_idx) {
				// This is a wire.
				signal += signal_name + " => int_" + name + ",\n";
			} else {
				// This is a wire bundle.
				if (GetPortDirection(port) == PORT_DIR::OUTPUT) {
					// We have to assign the port fully.
					signal += signal_name + " => int_" + name + ",\n";
				} else {
					if (indices.min_idx == 0 && indices.max_idx == (port_width - 1)) {
						// Fully assign the wire bundle.
						signal += signal_name + " => int_" + name + ",\n";
					} else {
						auto min_str = to_string(indices.min_idx);
						auto max_str = to_string(indices.max_idx);
						signal += signal_name + " => int_" + name + '(' + max_str + " DOWNTO " + min_str + "),\n";
					}
				}
			}
		}
	} else {
		// Not a single wire is connected to this port.
		const auto dir = GetPortDirection(port);

		switch (dir) {
		case PORT_DIR::INPUT:
			if (port_width > 1) {
				signal = signal_name + " => (OTHERS => '0'),\n";
			} else {
				signal = signal_name + " => '0',\n";
			}
			break;
		case PORT_DIR::OUTPUT:
			signal = signal_name + " => OPEN,\n";
			break;
		}
	}

	size_t found = 0;

	if (last_port) {
		found = signal.find_last_of(',');
	} else {
		found = signal.find_last_of('\n');
	}
	signal = signal.substr(0, found);

	inst.SetValue(signal_name, signal);
}
