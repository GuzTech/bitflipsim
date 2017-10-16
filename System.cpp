#include "main.h"

void System::AddComponent(comp_t component) {
	components.insert(std::pair<std::string, comp_t>(component->GetName(), component));

	// Add all wires of this component.
	for (auto &w : component->GetWires()) {
		if (!w) {
			std::cout << "[Warning] Component \"" << component->GetName()
					  << "\" has one or more ports that are not connected.\n";
		} else {
			wires.insert(std::pair<std::string, wire_t>(w->GetName(), w));

			if (w->IsInputWire()) {
				if (std::find(input_wires.begin(),
							  input_wires.end(),
							  w) == input_wires.end())
				{
					input_wires.push_back(w);
				}
			} else if (w->IsOutputWire()) {
				output_wires.push_back(w);
			}
		}
	}
}

void System::FindLongestPathInSystem() {
	// Start with the global input wires.
	std::vector<wire_t> wires_to_process(input_wires);
	std::vector<comp_t> components_to_process;

	do {
		components_to_process.clear();
		
		// Find all the components that the wires to be processed are connected to.
		for (auto &w : wires_to_process) {
			for (auto &c : w->GetOutputs()) {
				auto comp = c.lock();

				if (std::find(components_to_process.begin(),
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

		for (auto &c : components_to_process) {
			for (auto &w : c->GetOutputWires()) {
				if (std::find(wires_to_process.begin(),
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
	for (std::size_t i = 0; i < longest_path; ++i) {
		for (auto &[name, component] : components) {
			if (component) {
				component->Update(false);
			}
		}
	}
}

void System::Update() {
	for (std::size_t i = 0; i < longest_path - 1; ++i) {
		for (auto &[name, component] : components) {
			if (component) {
				component->Update(true);
			}
		}
	}

	for (auto &[name, component] : components) {
		if (component) {
			component->Update(false);
		}
	}
}

std::size_t System::GetNumToggles() {
	std::size_t toggle_count = 0;

	for (auto &[name, component] : components) {
		if (component) {
			toggle_count += component->GetNumToggles();
		}
	}

	for (auto &[name, wire] : wires) {
		if (wire) {
			toggle_count += wire->GetNumToggles();
		}
	}

	return toggle_count;
}

wire_t System::GetWire(std::string wire_name) {
	if (wires.find(wire_name) != wires.end()) {
		return wires[wire_name];
	} else {
		return nullptr;
	}
}

std::vector<wire_t> System::GetWires() {
	std::vector<wire_t> w;

	for (auto &[name, wire] : wires) {
		w.push_back(wire);
	}

	return w;
}
