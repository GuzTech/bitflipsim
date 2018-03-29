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
	map<string, pair<size_t, size_t>> wire_indices;

	for (size_t i = 0; i < port_width; ++i) {
		const auto &w = GetWire(port, i);
		if (w) {
			const auto &wb = w->GetWireBundle();

			if (wb) {
				const auto &p = wire_indices[wb->GetName()];
				size_t min_idx = min(p.first, i);
				size_t max_idx = max(p.second, i);
				wire_indices[wb->GetName()] = pair<size_t, size_t>(min_idx, max_idx);
			} else {
				const auto &p = wire_indices[w->GetName()];
				size_t min_idx = min(p.first, i);
				size_t max_idx = max(p.second, i);
				wire_indices[w->GetName()] = pair<size_t, size_t>(min_idx, max_idx);
			}
		}
	}

	string signal = "";

	if (wire_indices.size()) {
		for (const auto &[name, indices] : wire_indices) {
			auto min_str = to_string(indices.first);
			auto max_str = to_string(indices.second);
			signal += signal_name + " => int_" + name + '(' + max_str + " DOWNTO " + min_str + "),\n";
		}
	} else {
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
