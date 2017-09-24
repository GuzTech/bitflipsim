#include "main.h"

void System::AddComponent(comp_t component) {
	components.insert(std::pair<std::string, comp_t>(component->GetName(), component));

	// Add all wires of this component.
	for (auto w : component->GetWires()) {
		wires.insert(std::pair<std::string, wire_t>(w->GetName(), w));
	}
}

void System::Update() {
	for (auto c : components) {
		if (c.second) {
			c.second->Update();
		}
	}
}

uint64_t System::GetNumToggles() {
	uint64_t toggle_count = 0;

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
