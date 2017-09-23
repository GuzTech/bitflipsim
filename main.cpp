#include "main.h"
#include <yaml-cpp/yaml.h>
#include <map>

using namespace std;

void Connect(comp_t component, std::string port_name, wire_t wire) {
	auto fa = dynamic_pointer_cast<FullAdder>(component);
	auto ha = dynamic_pointer_cast<HalfAdder>(component);

	if (fa != nullptr) {
		if (port_name.compare("A") == 0) {
			fa->Connect(FullAdder::PORTS::A, wire);
		} else if (port_name.compare("B") == 0) {
			fa->Connect(FullAdder::PORTS::B, wire);
		} else if (port_name.compare("Cin") == 0) {
			fa->Connect(FullAdder::PORTS::Cin, wire);
		} else if (port_name.compare("S") == 0) {
			fa->Connect(FullAdder::PORTS::S, wire);
		} else if (port_name.compare("Cout") == 0) {
			fa->Connect(FullAdder::PORTS::Cout, wire);
		}
	} else if (ha != nullptr) {
		if (port_name.compare("A") == 0) {
			ha->Connect(HalfAdder::PORTS::A, wire);
		} else if (port_name.compare("B") == 0) {
			ha->Connect(HalfAdder::PORTS::B, wire);
		} else if (port_name.compare("S") == 0) {
			ha->Connect(HalfAdder::PORTS::S, wire);
		} else if (port_name.compare("C") == 0) {
			ha->Connect(HalfAdder::PORTS::C, wire);
		}
	}
}

int main(int argc, char **argv) {
	map<std::string, comp_t> comps;
	YAML::Node config;
	System system;
	
	// Check if a configuration file was supplied.
	if (argc == 2) {
		std::string config_file_name(argv[1]);

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
	}

	if (!config.IsNull()) {
		if (config["components"]) {
			auto components = config["components"];

			for (YAML::const_iterator it = components.begin(); it != components.end(); ++it) {
				std::string key = it->first.as<std::string>();
				std::string val;

				if (it->second) {
				    val = it->second.as<std::string>();
					cout << "Type: " << key << "\nName: " << val << "\n";
				} else {
					cout << "Type: " << key << "\n";
				}

				if (key.compare("FullAdder") == 0) {
					comps[val] = make_shared<FullAdder>();
				} else if (key.compare("HalfAdder") == 0) {
					comps[val] = make_shared<HalfAdder>();
				} else {
					cout << "Unrecognized component type: " << key << "\n";
				}
			}

			//auto fa1 = config["components"][0];
			//auto fa2 = config["components"][1];

			//cout << fa1["FullAdder"] << " " << fa2 << "\n";

			//for (YAML::const_iterator c : config["components"]) {
			//	cout << c->first.as<std::string>() << "\n";
			//}
		}

		cout << "Number of components created: " << comps.size() << "\n";

		cout << config["components"] << "\n";

		if (config["wires"]) {
			auto wires = config["wires"];

			for (YAML::const_iterator it = wires.begin(); it != wires.end(); ++it) {
				std::string from = it->second[0]["from"].as<std::string>();
				std::string from_port = it->second[0]["port"].as<std::string>();
				std::string to = it->second[1]["to"].as<std::string>();
				std::string to_port = it->second[1]["port"].as<std::string>();

				if (comps.find(from) != comps.end() && comps.find(to) != comps.end()) {
					auto wire = make_shared<Wire>();

					auto from_comp = comps[from];
					auto to_comp = comps[to];

					Connect(from_comp, from_port, wire);
					Connect(to_comp, to_port, wire);
				} else if (from.compare("input") == 0) {
					auto wire = make_shared<Wire>();

					auto to_comp = comps[to];

					Connect(to_comp, to_port, wire);
				} else if (to.compare("output") == 0) {
					auto wire = make_shared<Wire>();

					auto from_comp = comps[from];

					Connect(from_comp, from_port, wire);
				}
			}
			//for (auto w : config["wires"]) {
			//	cout << w << "\n";
			//}
		}

		for (auto c : comps) {
			system.AddComponent(c.second);
		}
	}
#if 0
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
