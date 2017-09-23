#include "main.h"

void System::AddComponent(comp_t component) {
	// Add the component if it was not added before.
	if (std::find(components.begin(), components.end(), component) == components.end()) {
		components.push_back(component);
	}

	// Add all wires of this component if they were not added before.
	for (auto w : component->GetWires()) {
		if (std::find(wires.begin(), wires.end(), w) == wires.end()) {
			wires.push_back(w);
		}
	}
}

void System::Update() {
	for (auto c : components) {
		if (c) {
			c->Update();
		}
	}
}

uint64_t System::GetNumToggles() {
	uint64_t toggle_count = 0;

	for (auto c : components) {
		if (c) {
			toggle_count += c->GetNumToggles();
		}
	}

	for (auto w : wires) {
		if (w) {
			toggle_count += w->GetNumToggles();
		}
	}

	return toggle_count;
}
