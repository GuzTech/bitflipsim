#include "main.h"

void System::AddComponent(comp_t component) {
	components.insert(std::pair<std::string, comp_t>(component->GetName(), component));

	// Add all wires of this component.
	for (auto w : component->GetWires()) {
		if (!w) {
			std::cout << "[Warning] Component \"" << component->GetName()
					  << "\" has one or more ports that are not connected.\n";
		} else {
			wires.insert(std::pair<std::string, wire_t>(w->GetName(), w));

			if (w->IsInputWire()) {
				input_wires.push_back(w);
			} else if (w->IsOutputWire()) {
				output_wires.push_back(w);
			}
		}
	}
}

void System::Update() {
	for (auto c : components) {
		if (c.second) {
			c.second->Update();
		}
	}
}

std::size_t System::GetNumToggles() {
	std::size_t toggle_count = 0;

	for (auto c : components) {
		if (c.second) {
			toggle_count += c.second->GetNumToggles();
		}
	}

	for (auto w : wires) {
		if (w.second) {
			toggle_count += w.second->GetNumToggles();
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
