#include "main.h"
#include <yaml-cpp/yaml.h>

using namespace std;

bool IsComponentDeclared(map<string, comp_t> &comps, string &name) {
	return comps.find(name) != comps.end();
}

void Connect(comp_t component, string port_name, wire_t wire) {
	auto fa_comp   = dynamic_pointer_cast<FullAdder>(component);
	auto ha_comp   = dynamic_pointer_cast<HalfAdder>(component);
	auto and_comp  = dynamic_pointer_cast<And>(component);
	auto or_comp   = dynamic_pointer_cast<Or>(component);
	auto xor_comp  = dynamic_pointer_cast<Xor>(component);
	auto nand_comp = dynamic_pointer_cast<Nand>(component);
	auto nor_comp  = dynamic_pointer_cast<Nor>(component);
	auto xnor_comp = dynamic_pointer_cast<Xnor>(component);

	if (fa_comp != nullptr) {
		if (port_name.compare("A") == 0)         fa_comp->Connect(FullAdder::PORTS::A, wire);
		else if (port_name.compare("B") == 0)    fa_comp->Connect(FullAdder::PORTS::B, wire);
		else if (port_name.compare("Cin") == 0)  fa_comp->Connect(FullAdder::PORTS::Cin, wire);
		else if (port_name.compare("S") == 0)    fa_comp->Connect(FullAdder::PORTS::S, wire);
		else if (port_name.compare("Cout") == 0) fa_comp->Connect(FullAdder::PORTS::Cout, wire);
		else goto error;
	} else if (ha_comp != nullptr) {
		if (port_name.compare("A") == 0)      ha_comp->Connect(HalfAdder::PORTS::A, wire);
		else if (port_name.compare("B") == 0) ha_comp->Connect(HalfAdder::PORTS::B, wire);
		else if (port_name.compare("S") == 0) ha_comp->Connect(HalfAdder::PORTS::S, wire);
		else if (port_name.compare("C") == 0) ha_comp->Connect(HalfAdder::PORTS::C, wire);
		else goto error;
	} else if (and_comp != nullptr) {
		if (port_name.compare("A") == 0)      and_comp->Connect(And::PORTS::A, wire);
		else if (port_name.compare("B") == 0) and_comp->Connect(And::PORTS::B, wire);
		else if (port_name.compare("O") == 0) and_comp->Connect(And::PORTS::O, wire);
		else goto error;
	} else if (or_comp != nullptr) {
		if (port_name.compare("A") == 0)      or_comp->Connect(Or::PORTS::A, wire);
		else if (port_name.compare("B") == 0) or_comp->Connect(Or::PORTS::B, wire);
		else if (port_name.compare("O") == 0) or_comp->Connect(Or::PORTS::O, wire);
		else goto error;
	} else if (xor_comp != nullptr) {
		if (port_name.compare("A") == 0)      xor_comp->Connect(Xor::PORTS::A, wire);
		else if (port_name.compare("B") == 0) xor_comp->Connect(Xor::PORTS::B, wire);
		else if (port_name.compare("O") == 0) xor_comp->Connect(Xor::PORTS::O, wire);
		else goto error;
	} else if (nand_comp != nullptr) {
		if (port_name.compare("A") == 0)      nand_comp->Connect(Nand::PORTS::A, wire);
		else if (port_name.compare("B") == 0) nand_comp->Connect(Nand::PORTS::B, wire);
		else if (port_name.compare("O") == 0) nand_comp->Connect(Nand::PORTS::O, wire);
		else goto error;
	} else if (nor_comp != nullptr) {
		if (port_name.compare("A") == 0)      nor_comp->Connect(Nor::PORTS::A, wire);
		else if (port_name.compare("B") == 0) nor_comp->Connect(Nor::PORTS::B, wire);
		else if (port_name.compare("O") == 0) nor_comp->Connect(Nor::PORTS::O, wire);
		else goto error;
	} else if (xnor_comp != nullptr) {
		if (port_name.compare("A") == 0)      xnor_comp->Connect(Xnor::PORTS::A, wire);
		else if (port_name.compare("B") == 0) xnor_comp->Connect(Xnor::PORTS::B, wire);
		else if (port_name.compare("O") == 0) xnor_comp->Connect(Xnor::PORTS::O, wire);
		else goto error;
	}

	return;

error:
	cout << "[Error] Wire \"" << wire->GetName() << "\" wants to connect to non-existent port \""
		 << port_name << "\" of component \"" << component->GetName() << "\".\n";
	exit(1);
}

void ParseComponents(map<string, comp_t> &comps, YAML::Node config) {
	auto components = config["components"];

	if (components.size() == 0) {
		cout << "[Error] No components found.\n";
		exit(1);
	}

	for (YAML::const_iterator it = components.begin(); it != components.end(); ++it) {
		string comp_type = it->first.as<string>();
		string comp_name;

		if (it->second) {
			comp_name = it->second.as<string>();
		} else {
			cout << "[Error] Component must have a name.\n";
			exit(1);
		}

		if (comp_type.compare("FullAdder") == 0) comps[comp_name] = make_shared<FullAdder>(comp_name);
		else if (comp_type.compare("HalfAdder") == 0) comps[comp_name] = make_shared<HalfAdder>(comp_name);
		else if (comp_type.compare("And") == 0) comps[comp_name] = make_shared<And>(comp_name);
		else if (comp_type.compare("Or") == 0) comps[comp_name] = make_shared<Or>(comp_name);
		else if (comp_type.compare("Xor") == 0) comps[comp_name] = make_shared<Xor>(comp_name);
		else if (comp_type.compare("Nand") == 0) comps[comp_name] = make_shared<Nand>(comp_name);
		else if (comp_type.compare("Nor") == 0) comps[comp_name] = make_shared<Nor>(comp_name);
		else if (comp_type.compare("Xnor") == 0) comps[comp_name] = make_shared<Xnor>(comp_name);
		else {
			cout << "[Error] Component type \"" << comp_type << "\" not recognized.\n";
			exit(1);
		}
	}
}

void ParseWires(map<string, comp_t> &comps, YAML::Node config) {
	auto wires = config["wires"];

	if (wires.size() == 0) {
		cout << "[Error] \"wires\" section is empty.\n";
		exit(1);
	}

	for (YAML::const_iterator it = wires.begin(); it != wires.end(); ++it) {
		auto wire_name = it->first.as<string>();

		if (it->second.size() == 2) {
			auto wire = make_shared<Wire>(wire_name);

			// There is always a single source for a Wire.
			auto from = it->second[0];

			// Check for consistency of the "from:" section.
			if (from.IsNull()) {
				cout << "[Error] No \"from:\" section found for wire \"" << wire_name << "\".\n";
				exit(1);
			}
			if (from.IsScalar()) {
				cout << "[Error] \"from:\" section of wire \"" << wire_name << "\" is a scalar, but it must be a map.\n";
				exit(1);
			}
			if (from.IsSequence()) {
				cout << "[Error] \"from:\" section of wire \"" << wire_name << "\" is a sequence, but it must be a map.\n";
				exit(1);
			}
			if (from.IsMap()) {
				auto from_node = from["from"];
				if (!from_node.IsDefined()) {
					cout << "[Error] No \"from:\" section found for wire \"" << wire_name << "\".\n";
					exit(1);
				} else if (from_node.IsNull()) {
					cout << "[Error] \"from:\" section of wire \"" << wire_name << "\" is empty.\n";
					exit(1);
				} else if (!from_node.IsScalar()) {
					cout << "[Error] \"from:\" section of wire \"" << wire_name << "\" must be a scalar.\n";
					exit(1);
				}
			}

			auto from_name = from["from"].as<string>();
			bool from_is_input = from_name.compare("input") == 0;
			
			// Check if a component with this name exists.
			if (!from_is_input && !IsComponentDeclared(comps, from_name)) {
				cout << "[Error] \"from:\" section of wire \"" << wire_name << "\" points to component \""
					 << from_name << "\" which does not exist.\n";
				exit(1);
			}

			auto from_port_node = from["port"];

			// Check for consistency of the "port:" section if the wire input is not "input".
			if (!from_is_input) {
				if (!from_port_node.IsDefined()) {
					cout << "[Error] No \"port:\" section found for wire \"" << wire_name << "\".\n";
					exit(1);
				}
				if (from_port_node.IsNull()) {
					cout << "[Error] \"port:\" section of wire \"" << wire_name << "\" is empty.\n";
					exit(1);
				}
				if (!from_port_node.IsScalar()) {
					cout << "[Error] \"port:\" section of wire \"" << wire_name << "\" must be a scalar.\n";
					exit(1);
				}
			}

			// There can be multiple sinks for a Wire.
			auto to = it->second[1];

			// Check for consistency of the "to:" section.
			if (to.IsNull()) {
				cout << "[Error] No \"to:\" section found for wire \"" << wire_name << "\".\n";
				exit(1);
			}
			if (to.IsScalar()) {
				cout << "[Error] \"to:\" section for wire \"" << wire_name << "\" is a scalar, but it must be a map.\n";
				exit(1);
			}
			if (to.IsSequence()) {
				cout << "[Error] \"to:\" section for wire \"" << wire_name << "\" is a sequence, but it must be a map.\n";
				exit(1);
			}
			if (to.IsMap()) {
				auto to_node = to["to"];
				if (!to_node.IsDefined()) {
					cout << "[Error] No \"to:\" section found for wire \"" << wire_name << "\".\n";
					exit(1);
				} else if (to_node.IsNull()) {
					cout << "[Error] \"to:\" section is empty for wire \"" << wire_name << "\".\n";
					exit(1);
				} else if (to_node.IsSequence() && to_node.begin() == to_node.end()) {
					cout << "[Error] \"to:\" section contains empty sequence for wire \"" << wire_name << "\".\n";
					exit(1);
				} else if (to_node.IsMap() && to_node.begin() == to_node.end()) {
					cout << "[Error] \"to:\" section contains empty map for wire \"" << wire_name << "\".\n";
					exit(1);
				}
			}

			vector<string> to_port_names; // Names of the sink ports.
			vector<comp_t> to_components; // Sink Components.

			auto to_comp_node = to["to"];
			auto to_port_node = to["port"];
			bool output = false;

			bool comp_node_def = to_comp_node.IsDefined();
			bool port_node_def = to_port_node.IsDefined();

			// If both "to:" and "port:" keys exist and have a scalar value.
			if (comp_node_def && to_comp_node.IsScalar() &&
				port_node_def && to_port_node.IsScalar()) {
				auto comp_name = to_comp_node.as<string>();

				if (comp_name.compare("output") == 0) {
					cout << "[Error] Wire \"" << wire_name << "\" is an \"output\" wire, but has a port declared.\n";
					cout << "Either remove the \"port:\" key, or connect the port to an existing component.\n";
					exit(1);
				}
				if (IsComponentDeclared(comps, comp_name)) {
					to_components.push_back(comps[comp_name]);
					to_port_names.push_back(to_port_node.as<string>());
				} else {
					cout << "[Error] Wire \"" << wire_name << "\" refers to component \""
						 << comp_name << "\" which does not exist.\n";
					exit(1);
				}
			}
			// If both "to:" and "port:" keys exist and have sequence values.
			else if (comp_node_def && to_comp_node.IsSequence() &&
					 port_node_def && to_port_node.IsSequence()) {
				for (auto comp : to_comp_node) {
					auto comp_name = comp.as<string>();

					if (IsComponentDeclared(comps, comp_name)) {
						to_components.push_back(comps[comp_name]);
					} else {
						cout << "[Error] Wire \"" << wire_name << "\" refers to component \""
							 << comp_name << "\" which does not exist.\n";
						exit(1);
					}
				}

				for (auto port : to_port_node) {
					to_port_names.push_back(port.as<string>());
				}

				if (to_components.size() != to_port_names.size()) {
					cout << "[Error] Wire \"" << wire_name << "\" output component and port sequences must be same length.\n";
					exit(1);
				}
			}
			// If only the "to:" key exists.
			else if (!port_node_def && comp_node_def && to_comp_node.IsScalar()) {
				auto comp_name = to_comp_node.as<string>();

				// Node value must be "output".
				if (comp_name.compare("output") == 0) {
					output = true;
				} else {
					cout << "[Error] Wire \"" << wire_name << "\" has only a \"to:\" section without \"port:\".\n"
						 << "In that case, the only valid value for \"to:\" is \"output\", but is now \""
						 << comp_name << "\".\n";
					exit(1);
				}
			}
			// Something is configured incorrectly if we are here.
			else {
				if ((comp_node_def && port_node_def) &&
					(to_comp_node.Type() != to_port_node.Type())) {
					cout << "[Error] Wire \"" << wire_name
						 << "\" output \"to:\" and \"port:\" must both be scalars or sequences.\n";
					exit(1);
				}
			}

			if (from_name.compare("input") == 0) {
				if (to.size() == 2) {
					for (int i = 0; i < to_components.size(); ++i) {
						Connect(to_components[i], to_port_names[i], wire);
					}
				} else {
					cout << "[Error] Input wire \"" << wire_name << "\" has incomplete declared output.\n";
					exit(1);
				}
			} else if (output) {
				if (from.size() == 2) {
					auto from_comp = comps[from_name];
					auto from_port = from["port"].as<string>();

					Connect(from_comp, from_port, wire);
				} else {
					cout << "[Error] Output wire \"" << wire_name << "\" has incomplete declared input.\n";
					exit(1);
				}
			} else {
				if (IsComponentDeclared(comps, from_name)) {
					for (int i = 0; i < to_components.size(); ++i) {
						if (comps.find(to_components[i]->GetName()) != comps.end()) {
							auto from_comp = comps[from_name];
							auto to_comp = to_components[i];
							auto from_port = from["port"].as<string>();
							auto to_port = to_port_names[i];

							Connect(from_comp, from_port, wire);
							Connect(to_comp, to_port, wire);
						} else {
							cout << "[Error] Wire \"" << wire_name << "\" input component does not exist.\n";
							exit(1);
						}
					}
				} else {
					cout << "[Error] Wire \"" << wire_name << "\" output component does not exist.\n";
					exit(1);
				}
			}
		} else {
			cout << "[Error] Wire \"" << wire_name << "\" declaration needs exactly 1 \"from\" section and 1 \"to\" section.\n";
			exit(1);
		}
	}
}

void ParseStimuli(System &system, YAML::Node config) {
	auto stimuli = config["stimuli"];

	for (size_t i = 0; i < stimuli.size(); ++i) {
		for (auto step : stimuli[i]) {
			auto wire_name = step.first.as<string>();
			auto value_name = step.second.as<string>();
			auto value = false;

			if (value_name.compare("1") == 0 || value_name.compare("true") == 0) {
				value = true;
			} else if (value_name.compare("0") == 0 || value_name.compare("false") == 0) {
				value = false;
			} else {
				cout << "[Error] stimulus value of wire \"" << wire_name <<
					"\" has to be one of the following: 0, 1, true, false.\n";
				exit(1);
			}

			auto wire = system.GetWire(wire_name);
			if (wire) {
				wire->SetValue(value);
			} else {
				cout << "[Error] Non-existent wire \"" << wire_name << "\" found in stimuli section.\n";
				exit(1);
			}
		}

		system.Update();
	}
}

int main(int argc, char **argv) {
#if 1
	map<std::string, comp_t> comps;
	YAML::Node config;
	System system;
	string config_file_name;
	
	// Check if a configuration file was supplied.
	if (argc == 2) {
		config_file_name = string(argv[1]);

		try {
			config = YAML::LoadFile(config_file_name.c_str());
		} catch (YAML::BadFile e) {
			cout << "[Error] Could not load file \"" <<
				config_file_name.c_str() << "\": " << e.msg << "\n";
		} catch (YAML::ParserException e) {
			cout << "[Error] Could not parse file \"" <<
				config_file_name.c_str() << "\": " << e.msg << "\n";
		}
	} else {
		cout << "Usage: ./bitflipsim <configuration file>\n";
		exit(0);
	}

	if (!config.IsNull()) {
		// First check for obvious errors.
		if (!config["components"]) {
			cout << "[Error] No \"components\" section found in \"" << config_file_name << "\"\n";
			exit(1);
		}
		if (config["components"].size() == 0) {
			cout << "[Error] \"components\" section in \"" << config_file_name << "\" is empty.\n";
			exit(1);
		}
		if (!config["wires"]) {
			cout << "[Error] No \"wires\" section found in \"" << config_file_name << "\"\n";
			exit(1);
		}
		if (config["wires"].size() == 0) {
			cout << "[Error] \"wires\" section in \"" << config_file_name << "\" is empty.\n";
			exit(1);
		}
		if (!config["stimuli"]) {
			cout << "[Error] No \"stimuli\" section found in \"" << config_file_name << "\"\n";
			exit(1);
		}
		if (config["stimuli"].size() == 0) {
			cout << "[Error] \"stimuli\" section in \"" << config_file_name << "\" is empty.\n";
			exit(1);
		}

		ParseComponents(comps, config);
		ParseWires(comps, config);

		for (auto c : comps) {
			system.AddComponent(c.second);
		}

		cout << "Number of components: " << system.GetNumComponents() <<
			"\nNumber of wires: " << system.GetNumWires() << "\n";

		ParseStimuli(system, config);

		cout << "\nNumber of toggles: " << system.GetNumToggles() << "\n";
	}
#else
	auto A0 = make_shared<Wire>();
	auto B0 = make_shared<Wire>();
	auto A1 = make_shared<Wire>();
	auto B1 = make_shared<Wire>();

	auto S0 = make_shared<Wire>();
	auto S1 = make_shared<Wire>();
	auto C0 = make_shared<Wire>();
	auto C1 = make_shared<Wire>();
	auto C2 = make_shared<Wire>();

	auto fa1 = make_shared<FullAdder>();
	auto fa2 = make_shared<FullAdder>();

	fa1->Connect(FullAdder::PORTS::A, A0);
	fa1->Connect(FullAdder::PORTS::B, B0);
	fa2->Connect(FullAdder::PORTS::A, A1);
	fa2->Connect(FullAdder::PORTS::B, B1);
	fa1->Connect(FullAdder::PORTS::S, S0);
	fa2->Connect(FullAdder::PORTS::S, S1);
	fa1->Connect(FullAdder::PORTS::Cin, C0);
	fa1->Connect(FullAdder::PORTS::Cout, C1);
	fa2->Connect(FullAdder::PORTS::Cin, C1);
	fa2->Connect(FullAdder::PORTS::Cout, C2);

	System system;
	system.AddComponent(fa1);
	system.AddComponent(fa2);

	C0->SetValue(true);
	A0->SetValue(true);
	B0->SetValue(true);
	A1->SetValue(true);
	B1->SetValue(true);

	system.Update();

	C0->SetValue(false);
	A0->SetValue(false);
	B0->SetValue(false);
	A1->SetValue(false);
	B1->SetValue(false);

	system.Update();

	cout << "Full adder:\nInput: " <<
		A1->GetValue() << A0->GetValue() <<	" + " <<
		B1->GetValue() << B0->GetValue() <<	" + " <<
		C0->GetValue() << " = " <<
		C2->GetValue() << S1->GetValue() << S0->GetValue() <<
		"\nNumber of toggled bits: " << system.GetNumToggles() << "\n";

/*
	auto A = make_shared<Wire>();
	auto B = make_shared<Wire>();
	auto S = make_shared<Wire>();
	auto C = make_shared<Wire>();

	auto ha1 = make_shared<HalfAdder>();

	A->SetDrives(ha1);
	B->SetDrives(ha1);

	ha1->Connect(HalfAdder::PORTS::A, A);
	ha1->Connect(HalfAdder::PORTS::B, B);
	ha1->Connect(HalfAdder::PORTS::S, S);
	ha1->Connect(HalfAdder::PORTS::C, C);

	bool a, b;

	cout << "\nHalf adder:\nA: ";
	while (!(cin >> a)) {
		cin.clear();
		cout << "Please enter a boolean value.\nA: ";
	}

	cout << "B: ";
	while (!(cin >> b)) {
		cin.clear();
		cout << "Please enter a boolean value.\nB: ";
	}

	A->SetValue(true);
	B->SetValue(true);

	ha1->Update();

	cout << "S: " << S->GetValue() << " C: " << C->GetValue() << "\n";
*/
#endif
	return 0;
}
