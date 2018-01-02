#include "main.h"
#include <experimental/filesystem>

void System::AddComponent(comp_t component) {
	components.insert(pair<string, comp_t>(component->GetName(), component));

	// Add all wires of this component.
	for (const auto &w : component->GetWires()) {
		if (!w) {
			cout << "[Warning] Component \"" << component->GetName()
				 << "\" has one or more ports that are not connected.\n";
		} else {
			wires.insert(pair<string, wire_t>(w->GetName(), w));

			if (w->IsInputWire()) {
				// Add this wire to the list of input wires
				// if we haven't done so already.
				if (find(input_wires.begin(),
						 input_wires.end(),
						 w) == input_wires.end())
				{
					input_wires.emplace_back(w);
				}
			} else if (w->IsOutputWire()) {
				// Since outputs can only be driven by one
				// component only, we do not have to check if
				// we already have this wire is the list of
				// output wires.
				output_wires.emplace_back(w);
			}

			const auto &wb = w->GetWireBundle();
			if (wb) {
				const auto wb_name = wb->GetName();

				if (wire_bundles.find(wb_name) == wire_bundles.end())
				{
					wire_bundles.insert(pair<string, wb_t>(wb_name, wb));

					if (wb->IsInputBundle()) {
						// Add this wire to the list of input bundles
						// if we haven't done so already.
						if (find(input_bundles.begin(),
								 input_bundles.end(),
								 wb) == input_bundles.end()) {
							input_bundles.emplace_back(wb);
						}
					} else if (wb->IsOutputBundle()) {
						// Since outputs can only be driven by one
						// component only, we do not have to check if
						// we already have this bundle is the list of
						// output bundles.
						output_bundles.emplace_back(wb);
					}
				}
			}
		}
	}
}

void System::FindLongestPathInSystem() {
	// Start with the global input wires.
	vector<wire_t> wires_to_process(input_wires);
	vector<comp_t> components_to_process;

	cout << "Finding longest path in the system.\n";

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

const size_t System::GetNumToggles() const {
	size_t toggle_count = 0;

	// Only wires keep track of how many times they toggled.
	for (const auto &[name, wire] : wires) {
		if (wire) {
			toggle_count += wire->GetNumToggles();
		}
	}

	return toggle_count;
}

const comp_t System::GetComponent(const string &comp_name) const {
	if (components.find(comp_name) != components.end()) {
		return components.at(comp_name);
	} else {
		return nullptr;
	}
}

const vector<comp_t> System::GetComponents() const {
	vector<comp_t> c;

	for (const auto &[name, comp] : components) {
		c.push_back(comp);
	}

	return c;
}

const wire_t System::GetWire(const string &wire_name) const {
	if (wires.find(wire_name) != wires.end()) {
		return wires.at(wire_name);
	} else {
		return nullptr;
	}
}

const wb_t System::GetWireBundle(const string &bundle_name) const {
	if (wire_bundles.find(bundle_name) != wire_bundles.end()) {
		return wire_bundles.at(bundle_name);
	} else {
		return nullptr;
	}
}

const void System::GenerateVHDL(const string &path) const {
	// Recursively create the folders of the path.
	//
	experimental::filesystem::create_directory(path);

	for (const auto &comp : components) {
		comp.second->GenerateVHDLEntity(path);
	}
}
