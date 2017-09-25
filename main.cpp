#include "main.h"
#include <yaml-cpp/yaml.h>

using namespace std;

void Connect(comp_t component, string port_name, wire_t wire) {
	auto fa = dynamic_pointer_cast<FullAdder>(component);
	auto ha = dynamic_pointer_cast<HalfAdder>(component);

	if (fa != nullptr) {
		if (port_name.compare("A") == 0)         fa->Connect(FullAdder::PORTS::A, wire);
		else if (port_name.compare("B") == 0)    fa->Connect(FullAdder::PORTS::B, wire);
		else if (port_name.compare("Cin") == 0)  fa->Connect(FullAdder::PORTS::Cin, wire);
		else if (port_name.compare("S") == 0)    fa->Connect(FullAdder::PORTS::S, wire);
		else if (port_name.compare("Cout") == 0) fa->Connect(FullAdder::PORTS::Cout, wire);
		else goto error;
	} else if (ha != nullptr) {
		if (port_name.compare("A") == 0)      ha->Connect(HalfAdder::PORTS::A, wire);
		else if (port_name.compare("B") == 0) ha->Connect(HalfAdder::PORTS::B, wire);
		else if (port_name.compare("S") == 0) ha->Connect(HalfAdder::PORTS::S, wire);
		else if (port_name.compare("C") == 0) ha->Connect(HalfAdder::PORTS::C, wire);
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
		else {
			cout << "[Error] Component type \"" << comp_type << "\" not recognized.\n";
			exit(1);
		}
	}
}

void ParseWires(map<string, comp_t> &comps, YAML::Node config) {
	auto wires = config["wires"];

	if (wires.size() == 0) {
		cout << "[Error] No wires found.\n";
		exit(1);
	}

	for (YAML::const_iterator it = wires.begin(); it != wires.end(); ++it) {
		auto wire_name = it->first.as<string>();

		if (it->second.size() == 2) {
			auto wire = make_shared<Wire>(wire_name);

			// There is always a single source for a Wire.
			auto from = it->second[0];

			if (!from["from"].IsScalar()) {
				cout << "[Error] Wire \"" << wire_name << "\" from input must be a scalar.\n";
				exit(1);
			}

			auto from_name = from["from"].as<string>();
			
			if (!from["port"].IsScalar() && from_name.compare("input") != 0) {
				cout << "[Error] Wire \"" << wire_name << "\" from port must be a scalar.\n";
				exit(1);
			}

			// There can be multiple sinks for a Wire.
			auto to = it->second[1];
			vector<string> to_port_names;
			vector<comp_t> components;

			auto to_comp_node = to["to"];
			auto to_port_node = to["port"];
			bool output = false;

			if (to_comp_node.IsScalar() && to_port_node.IsScalar()) {
				components.push_back(comps[to_comp_node.as<string>()]);
				to_port_names.push_back(to_port_node.as<string>());
			} else if (to_comp_node.IsSequence() && to_port_node.IsSequence()) {
				for (auto comp : to_comp_node) {
					components.push_back(comps[comp.as<string>()]);
				}

				for (auto port : to_port_node) {
					to_port_names.push_back(port.as<string>());
				}

				if (components.size() != to_port_names.size()) {
					cout << "[Error] Wire \"" << wire_name << "\" output component and port sequences must be same length.\n";
					exit(1);
				}
			} else if (to_comp_node.IsScalar()) {
				if (to_comp_node.as<string>().compare("output") == 0) {
					output = true;
				}
			} else {
				cout << "[Error] Wire \"" << wire_name << "\" output must be a scalar or sequence.\n";
				exit(1);
			}

			if (from_name.compare("input") == 0) {
				if (to.size() == 2) {
					for (int i = 0; i < components.size(); ++i) {
						Connect(components[i], to_port_names[i], wire);
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
				if (comps.find(from_name) != comps.end()) {
					for (int i = 0; i < components.size(); ++i) {
						if (comps.find(components[i]->GetName()) != comps.end()) {
							auto from_comp = comps[from_name];
							auto to_comp = components[i];
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
			cout << "[Error] Wire \"" << wire_name << "\" declaration is incomplete.\n";
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
				cout << "[Error] stimulus value has to be one of the following: 0, 1, true, false.\n";
				exit(1);
			}

			auto wire = system.GetWire(wire_name);
			if (wire) {
				wire->SetValue(value);
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
