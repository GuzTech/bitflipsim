#include "main.h"
#include <yaml-cpp/yaml.h>

using namespace std;

bool IsComponentDeclared(map<string, comp_t> &comps, string &name) {
	return comps.find(name) != comps.end();
}

void Connect(comp_t component, string port_name, wire_t wire, size_t index = 0) {
	auto fa_comp   = dynamic_pointer_cast<FullAdder>(component);
	auto ha_comp   = dynamic_pointer_cast<HalfAdder>(component);
	auto and_comp  = dynamic_pointer_cast<And>(component);
	auto or_comp   = dynamic_pointer_cast<Or>(component);
	auto xor_comp  = dynamic_pointer_cast<Xor>(component);
	auto nand_comp = dynamic_pointer_cast<Nand>(component);
	auto nor_comp  = dynamic_pointer_cast<Nor>(component);
	auto xnor_comp = dynamic_pointer_cast<Xnor>(component);
	auto not_comp  = dynamic_pointer_cast<Not>(component);
	auto mux_comp  = dynamic_pointer_cast<Mux>(component);
	auto rca_comp  = dynamic_pointer_cast<RippleCarryAdder>(component);
	auto m2C_comp  = dynamic_pointer_cast<Multiplier_2C>(component);
	auto smag_comp = dynamic_pointer_cast<Multiplier_Smag>(component);

	if (fa_comp != nullptr) {
		if (port_name.compare("A") == 0)         fa_comp->Connect(PORTS::A, wire);
		else if (port_name.compare("B") == 0)    fa_comp->Connect(PORTS::B, wire);
		else if (port_name.compare("Cin") == 0)  fa_comp->Connect(PORTS::Cin, wire);
		else if (port_name.compare("S") == 0)    fa_comp->Connect(PORTS::S, wire);
		else if (port_name.compare("Cout") == 0) fa_comp->Connect(PORTS::Cout, wire);
		else goto error;
	} else if (ha_comp != nullptr) {
		if (port_name.compare("A") == 0)         ha_comp->Connect(PORTS::A, wire);
		else if (port_name.compare("B") == 0)    ha_comp->Connect(PORTS::B, wire);
		else if (port_name.compare("S") == 0)    ha_comp->Connect(PORTS::S, wire);
		else if (port_name.compare("Cout") == 0) ha_comp->Connect(PORTS::Cout, wire);
		else goto error;
	} else if (and_comp != nullptr) {
		if (port_name.compare("A") == 0)      and_comp->Connect(PORTS::A, wire);
		else if (port_name.compare("B") == 0) and_comp->Connect(PORTS::B, wire);
		else if (port_name.compare("O") == 0) and_comp->Connect(PORTS::O, wire);
		else goto error;
	} else if (or_comp != nullptr) {
		if (port_name.compare("A") == 0)      or_comp->Connect(PORTS::A, wire);
		else if (port_name.compare("B") == 0) or_comp->Connect(PORTS::B, wire);
		else if (port_name.compare("O") == 0) or_comp->Connect(PORTS::O, wire);
		else goto error;
	} else if (xor_comp != nullptr) {
		if (port_name.compare("A") == 0)      xor_comp->Connect(PORTS::A, wire);
		else if (port_name.compare("B") == 0) xor_comp->Connect(PORTS::B, wire);
		else if (port_name.compare("O") == 0) xor_comp->Connect(PORTS::O, wire);
		else goto error;
	} else if (nand_comp != nullptr) {
		if (port_name.compare("A") == 0)      nand_comp->Connect(PORTS::A, wire);
		else if (port_name.compare("B") == 0) nand_comp->Connect(PORTS::B, wire);
		else if (port_name.compare("O") == 0) nand_comp->Connect(PORTS::O, wire);
		else goto error;
	} else if (nor_comp != nullptr) {
		if (port_name.compare("A") == 0)      nor_comp->Connect(PORTS::A, wire);
		else if (port_name.compare("B") == 0) nor_comp->Connect(PORTS::B, wire);
		else if (port_name.compare("O") == 0) nor_comp->Connect(PORTS::O, wire);
		else goto error;
	} else if (xnor_comp != nullptr) {
		if (port_name.compare("A") == 0)      xnor_comp->Connect(PORTS::A, wire);
		else if (port_name.compare("B") == 0) xnor_comp->Connect(PORTS::B, wire);
		else if (port_name.compare("O") == 0) xnor_comp->Connect(PORTS::O, wire);
		else goto error;
	} else if (not_comp != nullptr) {
		if (port_name.compare("I") == 0)      not_comp->Connect(PORTS::I, wire);
		else if (port_name.compare("O") == 0) not_comp->Connect(PORTS::O, wire);
		else goto error;
	} else if (mux_comp != nullptr) {
		if (port_name.compare("A") == 0)      mux_comp->Connect(PORTS::A, wire);
		else if (port_name.compare("B") == 0) mux_comp->Connect(PORTS::B, wire);
		else if (port_name.compare("S") == 0) mux_comp->Connect(PORTS::S, wire);
		else if (port_name.compare("O") == 0) mux_comp->Connect(PORTS::O, wire);
		else goto error;
	} else if (rca_comp != nullptr) {
		if (port_name.compare("A") == 0)         rca_comp->Connect(PORTS::A, wire, index);
		else if (port_name.compare("B") == 0)    rca_comp->Connect(PORTS::B, wire, index);
		else if (port_name.compare("S") == 0)    rca_comp->Connect(PORTS::S, wire, index);
		else if (port_name.compare("Cin") == 0)  rca_comp->Connect(PORTS::Cin, wire, index);
		else if (port_name.compare("Cout") == 0) rca_comp->Connect(PORTS::Cout, wire, index);
		else goto error;
	} else if (m2C_comp != nullptr) {
		if (port_name.compare("A") == 0) m2C_comp->Connect(PORTS::A, wire, index);
		else if (port_name.compare("B") == 0) m2C_comp->Connect(PORTS::B, wire, index);
		else if (port_name.compare("O") == 0) m2C_comp->Connect(PORTS::O, wire, index);
		else goto error;
	}else if (smag_comp != nullptr) {
		if (port_name.compare("A") == 0) smag_comp->Connect(PORTS::A, wire, index);
		else if (port_name.compare("B") == 0) smag_comp->Connect(PORTS::B, wire, index);
		else if (port_name.compare("O") == 0) smag_comp->Connect(PORTS::O, wire, index);
		else goto error;
	}

	return;

error:
	cout << "[Error] Wire \"" << wire->GetName() << "\" wants to connect to non-existent port \""
		 << port_name << "\" of component \"" << component->GetName() << "\".\n";
	exit(1);
}

void ParsePortAndIndex(string wire_name, string port_string, string &port_name, size_t &index) {
	// Check if the name contains a space to indicate that the name
	// consists of the name of the port and the index of that port.
	size_t pos = port_string.find(" ");
	if (pos != string::npos) {
		// We assume the first value is the name.
		port_name = port_string.substr(0, pos);

		// We remove the name + " " part from the string, and search again.
		// There should be a second part which should be a number to indicate the index.
		port_string.erase(0, pos + 1);

		pos = port_string.find(" ");
		if (port_string.length() != 0 && pos == string::npos) {
			try {
				index = stoul(port_string.substr(0, pos));
			} catch (invalid_argument e) {
				cout << "[Error] Index for port \"" << port_name << "\" of wire \""
					 << wire_name << "\" is not a number.\n";
				exit(1);
			} catch (out_of_range e) {
				cout << "[Error] Index number for port \"" << port_name << "\" of wire \""
					 << wire_name << "\" is too large.\n";
				exit(1);
			}
		} else {
			cout << "[Error] One index should be given after port name \"" << port_name
				 << "\" for wire \"" << wire_name << "\".\n";
			exit(1);

			// This only happens if the user enters a port name with at least one space after it
			// in double quotation marks like so "A ". Else the YAML parser removes whitespace.
		}
	} else {
		// The name is just the port and does not contain an index.
		port_name = port_string;
	}
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

		size_t num_bits_A = 0;
		size_t num_bits_B = 0;
		auto value = it->second;
		if (value) {
			if (value.IsScalar()) {
				comp_name = value.as<string>();
			} else if (value.IsSequence()) {
				comp_name = value[0].as<string>();

				if (value.size() <= 3) {
					num_bits_A = value[1].as<size_t>();

					if (value.size() == 3) {
						num_bits_B = value[2].as<size_t>();
					}
				} else {
					cout << "[Error] Component \"" << comp_name << "\" can have at most two arguments.\n";
					exit(1);
				}
			} else {
				cout << "[Error] Component name must be a scalar or sequence.\n";
				exit(1);
			}
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
		else if (comp_type.compare("Not") == 0) comps[comp_name] = make_shared<Not>(comp_name);
		else if (comp_type.compare("Mux") == 0) comps[comp_name] = make_shared<Mux>(comp_name);
		else if (comp_type.compare("RippleCarryAdder") == 0) comps[comp_name] = make_shared<RippleCarryAdder>(comp_name, num_bits_A);
		else if (comp_type.compare("Multiplier_2C") == 0) comps[comp_name] = make_shared<Multiplier_2C>(comp_name, num_bits_A, num_bits_B);
		else if (comp_type.compare("Multiplier_Smag") == 0) comps[comp_name] = make_shared<Multiplier_Smag>(comp_name, num_bits_A, num_bits_B);
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

					//string port_name;
					//ParsePortAndIndex(wire_name, to_port_node.as<string>(), port_name, index);
					//to_port_names.push_back(port_name);
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
				for (YAML::const_iterator it = to_comp_node.begin(); it != to_comp_node.end(); ++it) {
					auto comp_name = (*it).as<string>();

					if (IsComponentDeclared(comps, comp_name)) {
						to_components.push_back(comps[comp_name]);
					} else if (comp_name.compare("output") == 0) {
						if (!output) {
							output = true;
						} else {
							cout << "[Error] \"to:\" section of wire \"" << wire_name
								 << "\" has multiple \"output\" items configured.\n";
							exit(1);
						}
					} else {
						cout << "[Error] Wire \"" << wire_name << "\" refers to component \""
							 << comp_name << "\" which does not exist.\n";
						exit(1);
					}
				}

				for (YAML::const_iterator it = to_port_node.begin(); it != to_port_node.end(); ++it) {
					to_port_names.push_back((*it).as<string>());
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
					for (std::size_t i = 0; i < to_components.size(); ++i) {
						string port_name;
						size_t index = 0;
						ParsePortAndIndex(wire_name, to_port_names[i], port_name, index);

						Connect(to_components[i], port_name, wire, index);
					}
				} else {
					cout << "[Error] Input wire \"" << wire_name << "\" has incomplete declared output.\n";
					exit(1);
				}
			} else {
				if (from.size() == 2) {
					auto from_comp = comps[from_name];
					auto from_port = from["port"].as<string>();

					string from_port_name;
					size_t from_index = 0;
					ParsePortAndIndex(wire_name, from_port, from_port_name, from_index);
					Connect(from_comp, from_port_name, wire, from_index);

					if (IsComponentDeclared(comps, from_name)) {
						for (std::size_t i = 0; i < to_components.size(); ++i) {
							if (comps.find(to_components[i]->GetName()) != comps.end()) {
								auto to_comp = to_components[i];
								auto to_port = to_port_names[i];
								
								string to_port_name;
								size_t to_index = 0;
								
								ParsePortAndIndex(wire_name, to_port, to_port_name, to_index);
								Connect(to_comp, to_port_name, wire, to_index);
							} else {
								cout << "[Error] Wire \"" << wire_name << "\" input component does not exist.\n";
								exit(1);
							}
						}
					} else {
						cout << "[Error] Output component of wire \"" << wire_name << "\" does not exist.\n";
						exit(1);
					}
				} else {
					cout << "[Error] Output wire \"" << wire_name << "\" input declaration needs one \"from:\" and "
						 << "one \"to:\" section.\n";
					exit(1);
				}
			}

			if (from_is_input) {
				wire->SetAsInputWire();
			} else if (output) {
				wire->SetAsOutputWire();
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
				wire->SetValue(value, false);
			} else {
				cout << "[Error] Non-existent wire \"" << wire_name << "\" found in stimuli section.\n";
				exit(1);
			}
		}

		system.Update();
	}
}

int main(int argc, char **argv) {
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
				config_file_name.c_str() << "\": " << e.msg << '\n';
			exit(1);
		} catch (YAML::ParserException e) {
			cout << "[Error] Could not parse file \"" <<
				config_file_name.c_str() << "\": " << e.msg << '\n';
			exit(1);
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

		for (auto &c : comps) {
			system.AddComponent(c.second);
		}

		system.FindLongestPathInSystem();
		system.FindInitialState();

		cout << "Longest path in the system: " << system.GetLongestPath() << '\n';
		cout << "Number of components: " << system.GetNumComponents() <<
			"\nNumber of wires: " << system.GetNumWires() << '\n';

		ParseStimuli(system, config);

		cout << "\nNumber of toggles: " << system.GetNumToggles() << '\n';

		cout << "\nValue of all wires:\n";
		for (auto &ow : system.GetWires()) {
			cout << "Wire \"" << ow->GetName()
				 << "\": " << ow->GetValue() << '\n';
		}
	}

	auto mul = Multiplier_Smag("mul", 6, 4);

	return 0;
}
