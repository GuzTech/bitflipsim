#include "main.h"

void System::AddComponent(comp_t component) {
	components.insert(pair<string, comp_t>(component->GetName(), component));

	// Add all wires of this component.
	for (const auto &w : component->GetWires()) {
		if (!w) {
			cout << "[Warning] Component \"" << component->GetName()
				 << "\" has one or more ports that are not connected.\n";
		} else {
			wires.insert(pair<string, wire_t>(w->GetName(), w));
			all_wires.push_back(w);

			if (w->IsInputWire()) {
				if (find(input_wires.begin(),
						 input_wires.end(),
						 w) == input_wires.end())
				{
					input_wires.push_back(w);
				}
			} else if (w->IsOutputWire()) {
				output_wires.push_back(w);
			}

			const auto &wb_o = w->GetWireBundle();
			if (wb_o) {
				const auto &wb = *wb_o;
				const auto wb_name = wb->GetName();

				if (wire_bundles.find(wb_name) == wire_bundles.end())
				{
					wire_bundles.insert(pair<string, wb_t>(wb_name, wb));
				}
			}
		}
	}
}

void System::FindLongestPathInSystem() {
	// Start with the global input wires.
	vector<wire_t> wires_to_process(input_wires);
	vector<comp_t> components_to_process;

	do {
		components_to_process.clear();
		
		// Find all the components that the wires to be processed are connected to.
		for (const auto &w : wires_to_process) {
			for (const auto &c : w->GetOutputs()) {
				const auto comp = c.lock();

				if (find(components_to_process.begin(),
						 components_to_process.end(),
						 comp) == components_to_process.end()) {
					components_to_process.push_back(comp);
				}
			}
		}

		if (!components_to_process.empty()) {
			longest_path++;
		}

		// Populate the list of wires to be processed with the outputs of
		// the components to process.
		wires_to_process.clear();

		for (const auto &c : components_to_process) {
			for (const auto &w : c->GetOutputWires()) {
				if (find(wires_to_process.begin(),
						 wires_to_process.end(),
						 w) == wires_to_process.end()) {
					wires_to_process.push_back(w);
				}
			}
		}
	} while (!components_to_process.empty());
}

// Finds the initial state which is the state of the system
// when all inputs are 0.
void System::FindInitialState() {
	for (size_t i = 0; i < longest_path; ++i) {
		for (const auto &[name, component] : components) {
			if (component) {
				component->Update(false);
			}
		}
	}
}

void System::Update() {
	for (size_t i = 0; i < longest_path - 1; ++i) {
		for (const auto &[name, component] : components) {
			if (component) {
				component->Update(true);
			}
		}
	}

	for (const auto &[name, component] : components) {
		if (component) {
			component->Update(false);
		}
	}
}

size_t System::GetNumToggles() {
	size_t toggle_count = 0;

	for (const auto &[name, component] : components) {
		if (component) {
			toggle_count += component->GetNumToggles();
		}
	}

	for (const auto &[name, wire] : wires) {
		if (wire) {
			toggle_count += wire->GetNumToggles();
		}
	}

	return toggle_count;
}

comp_t System::GetComponent(const string &comp_name) {
	if (components.find(comp_name) != components.end()) {
		return components[comp_name];
	} else {
		return nullptr;
	}
}

vector<comp_t> System::GetComponents() {
	vector<comp_t> c;

	for (const auto &[name, comp] : components) {
		c.push_back(comp);
	}

	return c;
}

wire_t System::GetWire(const string &wire_name) {
	if (wires.find(wire_name) != wires.end()) {
		return wires[wire_name];
	} else {
		return nullptr;
	}
}
