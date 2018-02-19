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
			} else {
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
			}

			if (w->IsInputWire()) {
				// Add this wire to the list of input wires
				// if we haven't done so already.
				if (find(all_input_wires.begin(),
						 all_input_wires.end(),
						 w) == all_input_wires.end())
				{
					all_input_wires.emplace_back(w);
				}
			} else if (w->IsOutputWire()) {
				if (find(all_output_wires.begin(),
						 all_output_wires.end(),
						 w) == all_output_wires.end())
				{
					all_output_wires.emplace_back(w);
				}
			}
		}
	}
}

void System::FindLongestPathInSystem() {
	// Start with the global input wires.
	vector<wire_t> wires_to_process(all_input_wires);
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

	assert(longest_path != 0);
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

const void System::GenerateVHDL(const string &template_name, const string &path) const {
	// Create the top level and testbench files.
	string output;
	TemplateDictionary toplevel("top");
	TemplateDictionary tb("tb");
	toplevel.SetValue("TOP_NAME", template_name);
	tb.SetValue("TOP_NAME", template_name);

	// Create the ports.
	{
		string ports;
		string variables;

		// Input wire bundles.
		if (input_bundles.size() > 0) {
			for (const auto &ib : input_bundles) {
			    const auto &name = ib->GetName();
			    const auto &size_str =
			        "STD_LOGIC_VECTOR(" + to_string(ib->GetSize() - 1) + " DOWNTO 0);";

				ports += name + " : IN " + size_str + "\n\t";
				variables += "VARIABLE " + name + "_val : " + size_str + "\n\t\t";
			}
		}

		// Output wire bundles.
		if (output_bundles.size() > 0) {
			for (const auto &ob : output_bundles) {
				const auto &name = ob->GetName();
				const auto &size_str =
					"STD_LOGIC_VECTOR(" + to_string(ob->GetSize() - 1) + " DOWNTO 0)";

				ports += name + " : OUT " + size_str + ";\n\t";
				variables += "VARIABLE " + name + "_val : " + size_str + ";\n\t\t";
				variables += "VARIABLE " + name + "_val_prev : " + size_str + " := (OTHERS => 'U');\n\t\t";
			}
		}

		// Input wires.
		for (const auto &iw : input_wires) {
			const auto &ib = iw->GetWireBundle();
			if (ib) {
				if (find(input_bundles.begin(),
						 input_bundles.end(),
						 ib) == input_bundles.end()) {
					// Something went wrong, because it's not possible for
					// an input wire to be part of a bundle that is not an
					// input bundle.
					cout << "[Error] Input wire that is part of a non-input wire bundle "
						 << "detected. This should not be possible.\n";
					exit(1);
				}
			} else {
				// This wire was declared as not part of a bundle.
				ports += iw->GetName() + " : IN STD_LOGIC;\n\t";
			}
		}

		// Output wires.
		for (const auto &ow : output_wires) {
			const auto &ob = ow->GetWireBundle();
			if (ob) {
				if (find(output_bundles.begin(),
						 output_bundles.end(),
						 ob) == output_bundles.end()) {
					// Something went wrong, because it's not possible for
					// an output wire to be part of a bundle that is not an
					// output bundle.
					cout << "[Error] Output wire that is part of a non-output wire bundle "
						 << "detected. This should not be possible.\n";
					exit(1);
				}
			} else {
				// This wire was declared as not part of a bundle.
				ports += ow->GetName() + " : OUT STD_LOGIC;\n\t";
			}
		}

		// Remove last ";" from the ports string.
		ports = ports.substr(0, ports.find_last_of(";"));

		toplevel.SetValue("PORTS", ports);
		tb.SetValue("VARIABLES", variables);
	}

	// Signals and output signal assignments
	{
		string signals;
		string input_assignments;
		string output_assignments;

		for (const auto &ib : input_bundles) {
			signals += "SIGNAL int_" + ib->GetName() + " : STD_LOGIC_VECTOR(" +
				to_string(ib->GetSize() - 1) + " DOWNTO 0);\n\t";
			input_assignments += "int_" + ib->GetName() + " <= " + ib->GetName() + ";\n\t\t\t";
		}

		for (const auto &ob : output_bundles) {
			signals += "SIGNAL int_" + ob->GetName() + " : STD_LOGIC_VECTOR(" +
				to_string(ob->GetSize() - 1) + " DOWNTO 0);\n\t";
			output_assignments += ob->GetName() + " <= int_" + ob->GetName() + ";\n\t\t\t";
		}

		size_t found = signals.find_last_of("\n");
		signals = signals.substr(0, found);
		found = output_assignments.find_last_of("\n");
		output_assignments = output_assignments.substr(0, found);

		if (found == string::npos) {
			found = input_assignments.find_last_of("\n");
			input_assignments = input_assignments.substr(0, found);
		}

		toplevel.SetValue("SIGNALS", signals);
		tb.SetValue("SIGNALS", signals);
		toplevel.SetValue("REG_ASSIGNMENTS", input_assignments + output_assignments);
	}

	// Assignments
	{

	}

	// Instances
	{
	    // Top-level
		string instances_top_level;

		for (const auto &comp : components) {
			instances_top_level += comp.second->GenerateVHDLInstance() + '\n';
		}

		toplevel.SetValue("INSTANCES", instances_top_level);

        // Testbench
        string instances_tb = "\tDUT : ENTITY work." + template_name;
        instances_tb += "\n\tPORT MAP (";
        instances_tb += "\n\t\ti_clk => i_clk,";
        instances_tb += "\n\t\ti_rst => i_rst,";

        for (const auto &ib : input_bundles) {
            const auto &name = ib->GetName();
            instances_tb += "\n\t\t" + name + " => int_" + name + ',';
        }

        for (const auto &ob : output_bundles) {
            const auto &name = ob->GetName();
            instances_tb += "\n\t\t" + name + " => int_" + name + ',';
        }

        instances_tb = instances_tb.substr(0, instances_tb.find_last_of(","));

        instances_tb += "\n\t);";
		tb.SetValue("INSTANCES", instances_tb);
	}

	// Testbench assignments.
	{
	    string read_stimuli;
	    string assign_stimuli;

	    for (const auto &ib : input_bundles) {
	        const auto &name = ib->GetName();

            read_stimuli += "READ(rdline, " + name + "_val);\n\t\t\t";
            assign_stimuli += "int_" + name + " <= " + name + "_val;\n\t\t\t";
        }

        tb.SetValue("READ_STIMULI", read_stimuli);
        tb.SetValue("ASSIGN_STIMULI", assign_stimuli);
	}

	// Expected output
	{
		string exp_stimuli;
		string assert_stimuli;
		string update_prev_stimuli;

		for (const auto &ob : output_bundles) {
			const auto &name = ob->GetName();

			exp_stimuli += "READ(exline, " + name + "_val);\n\t\t\t\t";
			assert_stimuli += "ASSERT (int_" + name + " = " + name + "_val_prev) REPORT \"" +
				"Output not as expected\" SEVERITY FAILURE;\n\t\t\t\t\t";
			update_prev_stimuli += name + "_val_prev := " + name + "_val;\n\t\t\t";
		}

		tb.SetValue("EXP_STIMULI", exp_stimuli);
		tb.SetValue("ASSERT_STIMULI", assert_stimuli);
		tb.SetValue("UPDATE_PREV_STIMULI", update_prev_stimuli);
	}

	ExpandTemplate("src/templates/VHDL/top_level.tpl", DO_NOT_STRIP, &toplevel, &output);
	auto outfile = ofstream(path + "/top.vhd");
	outfile << output;
	outfile.close();

	for (const auto &comp : components) {
		comp.second->GenerateVHDLEntity(path);
	}

    output.clear();
	ExpandTemplate("src/templates/VHDL/top_level_tb.tpl", DO_NOT_STRIP, &tb, &output);
	outfile = ofstream(path + "/top_tb.vhd");
	outfile << output;
	outfile.close();
}
