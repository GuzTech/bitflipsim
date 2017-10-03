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
	// Keep track of which components still need to be processed.
	std::vector<comp_t> input_components;

	// Initially fill the list with all components
	// that are connected to global input wires.
	for (auto &iw : input_wires) {
		std::vector<std::weak_ptr<Component>> outputs = iw->GetOutputs();

		for (auto &o : outputs) {
			input_components.push_back(o.lock());
		}
	}

	// Keep track of components that are connected to global output wires.
	std::vector<comp_t> components_to_process;

	// Initially fill the list with all components that are directly
	// connected to global output wires, and then work your way to the input.
	for (auto &ow : output_wires) {
		components_to_process.push_back(ow->GetInput().lock());
	}

	while (input_components.size() != 0) {
		auto it = components_to_process.begin();

		// Find all components that are connected to global output wires,
		// and see if they are connected to components that are connected
		// to global input wires.
		// If there are still "input" components, then find the "output"
		// components that are one "level" closer to the "input" components,
		// and repeat the process.
		while (it != components_to_process.end()) {
			auto found = std::find(input_components.begin(),
								   input_components.end(),
								   (*it));
			if (found != input_components.end()) {
				input_components.erase(found);
			} else {
				it++;
			}
		}

		// First clear the list of output wires.
		output_wires.empty();

		// Update the longest path length.
		longest_path++;

		// Check if we are done.
		if (input_components.size() == 0) {
			break;
		}

		// Find all wires that connected to the input ports of the
		// components to process.
		for (auto &c : components_to_process) {
			for (auto &w : c->GetInputWires()) {
				output_wires.push_back(w);
			}
		}

		// Clear the components to process.
		components_to_process.empty();

		// Add the components that are connected to the new output
		// wires on the opposite side.
		for (auto &w : output_wires) {
			components_to_process.push_back(w->GetInput().lock());
		}
	}

	std::cout << "longest path: " << longest_path << "\n";
}

void System::Update() {
	for (auto &c : components) {
		if (c.second) {
			c.second->Update();
		}
	}
}

std::size_t System::GetNumToggles() {
	std::size_t toggle_count = 0;

	for (auto &c : components) {
		if (c.second) {
			toggle_count += c.second->GetNumToggles();
		}
	}

	for (auto &w : wires) {
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
