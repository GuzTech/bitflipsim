#include "main.h"
#include <yaml-cpp/yaml.h>

using namespace std;

int main(int argc, char **argv) {
	YAML::Node config;
	
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
			for (auto c : config["components"]) {
				cout << c << "\n";
			}
		}

		if (config["wires"]) {
			for (auto w : config["wires"]) {
				cout << w << "\n";
			}
		}
	}

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

	return 0;
}
