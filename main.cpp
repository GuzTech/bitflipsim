#include "main.h"
#include <yaml-cpp/yaml.h>
#include <random>
#include <fstream>
#include <bitset>

using namespace std;

bool IsComponentDeclared(const map<string, comp_t> &comps, const string &name) {
	return comps.find(name) != comps.end();
}

void Connect(const comp_t &component, const string &port_name, const wire_t &wire, size_t index = 0) {
	const auto &fa_comp   = dynamic_pointer_cast<FullAdder>(component);
	const auto &ha_comp   = dynamic_pointer_cast<HalfAdder>(component);
	const auto &and_comp  = dynamic_pointer_cast<And>(component);
	const auto &or_comp   = dynamic_pointer_cast<Or>(component);
	const auto &or3_comp  = dynamic_pointer_cast<Or3>(component);
	const auto &xor_comp  = dynamic_pointer_cast<Xor>(component);
	const auto &nand_comp = dynamic_pointer_cast<Nand>(component);
	const auto &nor_comp  = dynamic_pointer_cast<Nor>(component);
	const auto &xnor_comp = dynamic_pointer_cast<Xnor>(component);
	const auto &not_comp  = dynamic_pointer_cast<Not>(component);
	const auto &mux_comp  = dynamic_pointer_cast<Mux>(component);
	const auto &rca_comp  = dynamic_pointer_cast<RippleCarryAdder>(component);
	const auto &m2C_comp  = dynamic_pointer_cast<Multiplier_2C>(component);
	const auto &smag_comp = dynamic_pointer_cast<Multiplier_Smag>(component);

	auto error_non_existent_port = [&]() {
		cout << "[Error] Wire \"" << wire->GetName() << "\" wants to connect to non-existent port \""
		 << port_name << "\" of component \"" << component->GetName() << "\".\n";
		exit(1);
	};

	if (fa_comp != nullptr) {
		if (port_name.compare("A") == 0)         fa_comp->Connect(PORTS::A, wire);
		else if (port_name.compare("B") == 0)    fa_comp->Connect(PORTS::B, wire);
		else if (port_name.compare("Cin") == 0)  fa_comp->Connect(PORTS::Cin, wire);
		else if (port_name.compare("O") == 0)    fa_comp->Connect(PORTS::O, wire);
		else if (port_name.compare("Cout") == 0) fa_comp->Connect(PORTS::Cout, wire);
		else error_non_existent_port();
	} else if (ha_comp != nullptr) {
		if (port_name.compare("A") == 0)         ha_comp->Connect(PORTS::A, wire);
		else if (port_name.compare("B") == 0)    ha_comp->Connect(PORTS::B, wire);
		else if (port_name.compare("O") == 0)    ha_comp->Connect(PORTS::O, wire);
		else if (port_name.compare("Cout") == 0) ha_comp->Connect(PORTS::Cout, wire);
		else error_non_existent_port();
	} else if (and_comp != nullptr) {
		if (port_name.compare("A") == 0)      and_comp->Connect(PORTS::A, wire);
		else if (port_name.compare("B") == 0) and_comp->Connect(PORTS::B, wire);
		else if (port_name.compare("O") == 0) and_comp->Connect(PORTS::O, wire);
		else error_non_existent_port();
	} else if (or_comp != nullptr) {
		if (port_name.compare("A") == 0)      or_comp->Connect(PORTS::A, wire);
		else if (port_name.compare("B") == 0) or_comp->Connect(PORTS::B, wire);
		else if (port_name.compare("O") == 0) or_comp->Connect(PORTS::O, wire);
		else error_non_existent_port();
	} else if (or3_comp != nullptr) {
		if (port_name.compare("A") == 0)      or3_comp->Connect(PORTS::A, wire);
		else if (port_name.compare("B") == 0) or3_comp->Connect(PORTS::B, wire);
		else if (port_name.compare("C") == 0) or3_comp->Connect(PORTS::C, wire);
		else if (port_name.compare("O") == 0) or3_comp->Connect(PORTS::O, wire);
		else error_non_existent_port();
	} else if (xor_comp != nullptr) {
		if (port_name.compare("A") == 0)      xor_comp->Connect(PORTS::A, wire);
		else if (port_name.compare("B") == 0) xor_comp->Connect(PORTS::B, wire);
		else if (port_name.compare("O") == 0) xor_comp->Connect(PORTS::O, wire);
		else error_non_existent_port();
	} else if (nand_comp != nullptr) {
		if (port_name.compare("A") == 0)      nand_comp->Connect(PORTS::A, wire);
		else if (port_name.compare("B") == 0) nand_comp->Connect(PORTS::B, wire);
		else if (port_name.compare("O") == 0) nand_comp->Connect(PORTS::O, wire);
		else error_non_existent_port();
	} else if (nor_comp != nullptr) {
		if (port_name.compare("A") == 0)      nor_comp->Connect(PORTS::A, wire);
		else if (port_name.compare("B") == 0) nor_comp->Connect(PORTS::B, wire);
		else if (port_name.compare("O") == 0) nor_comp->Connect(PORTS::O, wire);
		else error_non_existent_port();
	} else if (xnor_comp != nullptr) {
		if (port_name.compare("A") == 0)      xnor_comp->Connect(PORTS::A, wire);
		else if (port_name.compare("B") == 0) xnor_comp->Connect(PORTS::B, wire);
		else if (port_name.compare("O") == 0) xnor_comp->Connect(PORTS::O, wire);
		else error_non_existent_port();
	} else if (not_comp != nullptr) {
		if (port_name.compare("I") == 0)      not_comp->Connect(PORTS::I, wire);
		else if (port_name.compare("O") == 0) not_comp->Connect(PORTS::O, wire);
		else error_non_existent_port();
	} else if (mux_comp != nullptr) {
		if (port_name.compare("A") == 0)      mux_comp->Connect(PORTS::A, wire);
		else if (port_name.compare("B") == 0) mux_comp->Connect(PORTS::B, wire);
		else if (port_name.compare("S") == 0) mux_comp->Connect(PORTS::S, wire);
		else if (port_name.compare("O") == 0) mux_comp->Connect(PORTS::O, wire);
		else error_non_existent_port();
	} else if (rca_comp != nullptr) {
		if (port_name.compare("A") == 0)         rca_comp->Connect(PORTS::A, wire, index);
		else if (port_name.compare("B") == 0)    rca_comp->Connect(PORTS::B, wire, index);
		else if (port_name.compare("O") == 0)    rca_comp->Connect(PORTS::O, wire, index);
		else if (port_name.compare("Cin") == 0)  rca_comp->Connect(PORTS::Cin, wire, index);
		else if (port_name.compare("Cout") == 0) rca_comp->Connect(PORTS::Cout, wire, index);
		else error_non_existent_port();
	} else if (m2C_comp != nullptr) {
		if (port_name.compare("A") == 0)      m2C_comp->Connect(PORTS::A, wire, index);
		else if (port_name.compare("B") == 0) m2C_comp->Connect(PORTS::B, wire, index);
		else if (port_name.compare("O") == 0) m2C_comp->Connect(PORTS::O, wire, index);
		else error_non_existent_port();
	} else if (smag_comp != nullptr) {
		if (port_name.compare("A") == 0)      smag_comp->Connect(PORTS::A, wire, index);
		else if (port_name.compare("B") == 0) smag_comp->Connect(PORTS::B, wire, index);
		else if (port_name.compare("O") == 0) smag_comp->Connect(PORTS::O, wire, index);
		else error_non_existent_port();
	}
}

void ParseWireAndSize(string wire_string, string &wire_name, size_t &size, WireBundle::REPR &repr) {
	auto error_inconsistent = [](const auto &wire) {
		cout << "[Error] Inconsistent wire declaration for wire \"" << wire << "\".\n"
			 << "Format for wires:\n"
			 << "<wire name>\n\n"
			 << "Format for wire bundles:\n"
			 << "<wire name> <size> [1c|1C|2c|2C|smag]\n";
		exit(1);
	};

	// Check if the wire name contains a space to indicate that it
	// consists of the name and size of the wire bundle.
	size_t pos = wire_string.find(" ");
	if (pos != string::npos) {
		// We assume the first value is the name.
		wire_name = wire_string.substr(0, pos);

		// We remove the name + " " part from the string, and search again.
		// There should be a second part which should be a number to indicate the size.
		wire_string.erase(0, pos + 1);

		pos = wire_string.find(" ");
		if (wire_string.length() != 0) {
			if (pos == string::npos) {
				// Only a size is given.
				try {
					size = stoul(wire_string.substr(0, pos));
				} catch (invalid_argument e) {
					cout << "[Error] Size for wire \"" << wire_name << "\" is not a number.\n";
					exit(1);
				} catch (out_of_range e ) {
					cout << "[Error] Size for wire \"" << wire_name << "\" is too large.\n";
					exit(1);
				}
			} else {
				// A size and number representation is given.
				try {
					size = stoul(wire_string.substr(0, pos));

					// We again remove the name + " " part from the string, and search again.
					wire_string.erase(0, pos + 1);

					pos = wire_string.find(" ");
					if (wire_string.length() != 0 && pos == string::npos) {
						if (wire_string.compare("2c") == 0 ||
							wire_string.compare("2C") == 0) {
							repr = WireBundle::REPR::TWOS_COMPLEMENT;
						} else if (wire_string.compare("1c") == 0 ||
								   wire_string.compare("1C") == 0) {
							repr = WireBundle::REPR::ONES_COMPLEMENT;
						} else if (wire_string.compare("smag") == 0) {
							repr = WireBundle::REPR::SIGNED_MAGNITUDE;
						} else {
							cout << "[Error] Unrecognized number representation for wire bundle \""
								 << wire_name << "\".\nOnly \"2c\"/\"2C\" (default), \"1c\"/\"1C\""
								 << ", and \"s_mag\" are supported.\n";
							exit(1);
						}
					} else {
						error_inconsistent(wire_name);
					}
				} catch (invalid_argument e) {
					cout << "[Error] Size for wire \"" << wire_name << "\" is not a number.\n";
					exit(1);
				} catch (out_of_range e ) {
					cout << "[Error] Size for wire \"" << wire_name << "\" is too large.\n";
					exit(1);
				}
			}
		} else {
			error_inconsistent(wire_name);
		}
	} else {
		// The name is just the wire name and does not contain a size.
		wire_name = wire_string;
	}
}

void ParsePortAndIndex(const string &wire_name,
					   string port_string,
					   string &port_name,
					   size_t &begin_idx,
					   size_t &end_idx) {
	// Check if the name contains a space to indicate that it consists of
	// the name of the port and the beginning and end index of that port.
	size_t pos = port_string.find(" ");
	if (pos != string::npos) {
		// We assume the first value is the name.
		port_name = port_string.substr(0, pos);

		// We remove the name + " " part from the string, and search again.
		// There should be a second part which should be a number to indicate the index.
		port_string.erase(0, pos + 1);

		pos = port_string.find(" ");

		if (port_string.length() != 0) {
			if (pos == string::npos) {
				// There is one argument after the name.
				try {
					begin_idx = stoul(port_string.substr(0, pos));
					port_string.erase(0, pos + 1);
					pos = port_string.find(" ");
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
				// There are at least two arguments after the name.
				try {
					begin_idx = stoul(port_string.substr(0, pos));
					port_string.erase(0, pos + 1);
					pos = port_string.find(" ");

					if (port_string.length() != 0 && pos == string::npos) {
						// There is a second argument, which is the end index.
						end_idx = stoul(port_string.substr(0, pos));
					} else {
						cout << "[Error] At most two indices can be given after port name \""
							 << port_name << "\" for wire \"" << wire_name << "\".\n";
						exit(1);

						// This only happens if the user enters a port name with at least one space after it
						// in double quotation marks like so "A ". Else the YAML parser removes whitespace.
					}
				} catch (invalid_argument e) {
					cout << "[Error] Index for port \"" << port_name << "\" of wire \""
						 << wire_name << "\" is not a number.\n";
					exit(1);
				} catch (out_of_range e) {
					cout << "[Error] Index number for port \"" << port_name << "\" of wire \""
						 << wire_name << "\" is too large.\n";
					exit(1);
				}
			}
		}
	} else {
		// The name is just the port and does not contain an index.
		port_name = port_string;
	}
}

void ParseMultiplierComponent(map<string, comp_t> &comps, const YAML::Node &multiplier) {
	string name;
	size_t num_bits_A = 0;
	size_t num_bits_B = 0;
	NUMFMT format = NUMFMT::NONE;
	LAYOUT layout = LAYOUT::NONE;
	TYPE type = TYPE::NONE;

	if (multiplier["name"]) {
		name = multiplier["name"].as<string>();
	} else {
		cout << "[Error] Multiplier has no name.\n";
		exit(1);
	}

	if (multiplier["A width"]) {
		try {
			num_bits_A = multiplier["A width"].as<size_t>();
		} catch (YAML::TypedBadConversion<size_t> e) {
			cout << "[Error] Multiplier \"" << name << "\" port A width is not a number: " << e.msg << '\n';
			exit(1);
		}
	} else {
		cout << "[Error] Property \"A width\" for multiplier \"" << name << "\" is required, but was not found.\n";
		exit(1);
	}

	if (multiplier["B width"]) {
		try {
			num_bits_B = multiplier["B width"].as<size_t>();
		} catch (YAML::TypedBadConversion<size_t> e) {
			cout << "[Error] Multiplier \"" << name << "\" port B width is not a number: " << e.msg << '\n';
			exit(1);
		}
	} else {
		cout << "[Error] Property \"B width\" for multiplier \"" << name << "\" is required, but was not found.\n";
		exit(1);
	}

	if (multiplier["number format"]) {
		const auto &fmt_string = multiplier["number format"].as<string>();

		if (fmt_string.compare("2C") == 0)			format = NUMFMT::TWOS_COMPLEMENT;
		else if (fmt_string.compare("1C") == 0) 	format = NUMFMT::ONES_COMPLEMENT;
		else if (fmt_string.compare("SMAG") == 0) 	format = NUMFMT::SIGNED_MAGNITUDE;
		else if (fmt_string.compare("UINT") == 0)	format = NUMFMT::UNSIGNED;
		else {
			cout << "[Error] Property \"number format\" for multiplier \"" << name << "\" has an unsupported value. "
				 << "Supported values are \"2C\", \"1C\", \"SMAG\", and \"UINT\".\n";
			exit(1);
		}
	} else {
		cout << "[Error] Property \"number format\" for multiplier \"" << name << "\" is required, but was not found.\n";
		exit(1);
	}

	if (multiplier["layout"]) {
		const auto &layout_string = multiplier["layout"].as<string>();

		if (layout_string.compare("carry propagate") == 0) 		layout = LAYOUT::CARRY_PROPAGATE;
		else if (layout_string.compare("carry save") == 0)		layout = LAYOUT::CARRY_SAVE;
		else if (layout_string.compare("booth radix-2") == 0) 	layout = LAYOUT::BOOTH_RADIX_2;
		else if (layout_string.compare("booth radix-4") == 0)	layout = LAYOUT::BOOTH_RADIX_4;
		else {
			cout << "[Error] Property \"layout\" for multiplier \"" << name << "\" has an unsupported value. "
				 << "Supported values are \"carry propagate\", \"carry save\", \"booth radix-2\", and \"booth radix-4\".\n";
			exit(1);
		}
	} else {
		cout << "[Error] Property \"layout\" for multiplier \"" << name << "\" is required, but was not found.\n";
		exit(1);
	}

	if (multiplier["type"]) {
		const auto &type_string = multiplier["type"].as<string>();

		if (type_string.compare("inversion") == 0)				type = TYPE::INVERSION;
		else if (type_string.compare("sign extension") == 0)	type = TYPE::SIGN_EXTEND;
		else if (type_string.compare("Baugh-Wooley") == 0)		type = TYPE::BAUGH_WOOLEY;
		else if (type_string.compare("none") == 0)				type = TYPE::NONE;
		else {
			cout << "[Error] Property \"type\" for multiplier \"" << name << "\" has an unsupported value. "
				 << "Supported values are \"inversion\", and \"sign extension\".\n";
			exit(1);
		}
	}
	//else {
	//	cout << "[Error] Property \"type\" for multiplier \"" << name << "\" is required, but was not found.\n";
	//	exit(1);
	//}

	switch (format) {
	case NUMFMT::TWOS_COMPLEMENT:
		if (layout == LAYOUT::CARRY_PROPAGATE) {
			if (type == TYPE::INVERSION) {
				comps[name] = make_shared<Multiplier_2C>(name, num_bits_A, num_bits_B, Multiplier_2C::MUL_TYPE::CARRY_PROPAGATE_INVERSION);
			} else if (type == TYPE::SIGN_EXTEND) {
				comps[name] = make_shared<Multiplier_2C>(name, num_bits_A, num_bits_B, Multiplier_2C::MUL_TYPE::CARRY_PROPAGATE_SIGN_EXTEND);
			} else if (type == TYPE::BAUGH_WOOLEY) {
				comps[name] = make_shared<Multiplier_2C>(name, num_bits_A, num_bits_B, Multiplier_2C::MUL_TYPE::CARRY_PROPAGATE_BAUGH_WOOLEY);
			} else if (type == TYPE::NONE) {
				cout << "[Error] Twos-complement multiplier \"" << name << "\" cannot have \"none\" as a type.\n";
				exit(1);
			} else {
				cout << "[Error] Unsupported combination of properties for multiplier \"" << name << "\".\n";
				exit(1);
			}
		} else if (layout == LAYOUT::CARRY_SAVE) {
			if (type == TYPE::INVERSION) {
				comps[name] = make_shared<Multiplier_2C>(name, num_bits_A, num_bits_B, Multiplier_2C::MUL_TYPE::CARRY_SAVE_INVERSION);
			} else if (type == TYPE::SIGN_EXTEND) {
				comps[name] = make_shared<Multiplier_2C>(name, num_bits_A, num_bits_B, Multiplier_2C::MUL_TYPE::CARRY_SAVE_SIGN_EXTEND);
			} else if (type == TYPE::BAUGH_WOOLEY) {
				comps[name] = make_shared<Multiplier_2C>(name, num_bits_A, num_bits_B, Multiplier_2C::MUL_TYPE::CARRY_SAVE_BAUGH_WOOLEY);
			} else if (type == TYPE::NONE) {
				cout << "[Error] Twos-complement multiplier \"" << name << "\" cannot have \"none\" as a type.\n";
				exit(1);
			} else {
				cout << "[Error] Unsupported combination of properties for multiplier \"" << name << "\".\n";
				exit(1);
			}
		}
		break;
	case NUMFMT::SIGNED_MAGNITUDE:
		if (layout == LAYOUT::CARRY_PROPAGATE) {
			comps[name] = make_shared<Multiplier_Smag>(name, num_bits_A, num_bits_B, Multiplier_Smag::MUL_TYPE::CARRY_PROPAGATE);
		} else if (layout == LAYOUT::CARRY_SAVE) {
			comps[name] = make_shared<Multiplier_Smag>(name, num_bits_A, num_bits_B, Multiplier_Smag::MUL_TYPE::CARRY_SAVE);
		} else {
			cout << "[Error] Unsupported combination of properties for multiplier \"" << name << "\".\n";
			exit(1);
		}
		break;
	default:
		break;
	}
}

void ParseComponents(map<string, comp_t> &comps, const YAML::Node &config) {
	const auto &components = config["components"];

	if (components.size() == 0) {
		cout << "[Error] No components found.\n";
		exit(1);
	}

	for (YAML::const_iterator it = components.begin(); it != components.end(); ++it) {
		const string &comp_type = it->first.as<string>();
		string comp_name;

		size_t num_bits_A = 0;
		size_t num_bits_B = 0;
		const auto &value = it->second;
		if (value) {
			if (value.IsScalar()) {
				comp_name = value.as<string>();
			} else if (value.IsMap()) {

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

		if (comp_type.compare("FullAdder") == 0)             comps[comp_name] = make_shared<FullAdder>(comp_name);
		else if (comp_type.compare("HalfAdder") == 0)        comps[comp_name] = make_shared<HalfAdder>(comp_name);
		else if (comp_type.compare("And") == 0)              comps[comp_name] = make_shared<And>(comp_name);
		else if (comp_type.compare("Or") == 0)               comps[comp_name] = make_shared<Or>(comp_name);
		else if (comp_type.compare("Xor") == 0)              comps[comp_name] = make_shared<Xor>(comp_name);
		else if (comp_type.compare("Nand") == 0)             comps[comp_name] = make_shared<Nand>(comp_name);
		else if (comp_type.compare("Nor") == 0)              comps[comp_name] = make_shared<Nor>(comp_name);
		else if (comp_type.compare("Xnor") == 0)             comps[comp_name] = make_shared<Xnor>(comp_name);
		else if (comp_type.compare("Not") == 0)              comps[comp_name] = make_shared<Not>(comp_name);
		else if (comp_type.compare("Mux") == 0)              comps[comp_name] = make_shared<Mux>(comp_name);
		else if (comp_type.compare("RippleCarryAdder") == 0) comps[comp_name] = make_shared<RippleCarryAdder>(comp_name, num_bits_A);
		else if (comp_type.compare("Multiplier") == 0) 		 ParseMultiplierComponent(comps, it->second);
		else if (comp_type.compare("Multiplier_2C") == 0)    comps[comp_name] = make_shared<Multiplier_2C>(comp_name, num_bits_A, num_bits_B);
		else if (comp_type.compare("Multiplier_Smag") == 0)  comps[comp_name] = make_shared<Multiplier_Smag>(comp_name, num_bits_A, num_bits_B);
		else {
			cout << "[Error] Component type \"" << comp_type << "\" not recognized.\n";
			exit(1);
		}
	}
}

void ParseWires(map<string, comp_t> &comps, YAML::Node config) {
	const auto &wires = config["wires"];

	if (wires.size() == 0) {
		cout << "[Error] \"wires\" section is empty.\n";
		exit(1);
	}

	for (YAML::const_iterator it = wires.begin(); it != wires.end(); ++it) {
		const auto &wire_string = it->first.as<string>();

		string wire_name;
		size_t bundle_size = 0;
		WireBundle::REPR repr = WireBundle::REPR::TWOS_COMPLEMENT;
		ParseWireAndSize(wire_string, wire_name, bundle_size, repr);

		if (it->second.size() == 2) {
			wire_t wire;
			wb_t wire_bundle = nullptr;
			size_t size = 1;

			if (bundle_size > 0) {
				wire_bundle = make_shared<WireBundle>(wire_name, bundle_size, repr);
				wire_bundle->Init();
				size = bundle_size;
			}

			for (size_t b_idx = 0; b_idx < size; ++b_idx) {
				if (bundle_size > 0) {
					wire = (*wire_bundle)[b_idx];
				} else {
					wire = make_shared<Wire>(wire_name);
				}

				// There is always a single source for a Wire.
				const auto &from = it->second[0];

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

				const auto &from_name = from["from"].as<string>();
				const bool from_is_input = from_name.compare("input") == 0;
			
				// Check if a component with this name exists.
				if (!from_is_input && !IsComponentDeclared(comps, from_name)) {
					cout << "[Error] \"from:\" section of wire \"" << wire_name << "\" points to component \""
						 << from_name << "\" which does not exist.\n";
					exit(1);
				}

				const auto &from_port_node = from["port"];

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
				const auto &to = it->second[1];

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
					const auto &to_node = to["to"];
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

				const auto &to_comp_node = to["to"];
				const auto &to_port_node = to["port"];
				bool output = false;

				const bool comp_node_def = to_comp_node.IsDefined();
				const bool port_node_def = to_port_node.IsDefined();

				// If both "to:" and "port:" keys exist and have a scalar value.
				if (comp_node_def && to_comp_node.IsScalar() &&
					port_node_def && to_port_node.IsScalar()) {
					const auto &comp_name = to_comp_node.as<string>();

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
					for (YAML::const_iterator it = to_comp_node.begin(); it != to_comp_node.end(); ++it) {
						const auto &comp_name = (*it).as<string>();

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
					const auto &comp_name = to_comp_node.as<string>();

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
							size_t begin_idx = 0;
							size_t end_idx = 0;
							ParsePortAndIndex(wire_name, to_port_names[i], port_name, begin_idx, end_idx);

							Connect(to_components[i], port_name, wire, b_idx + begin_idx);
						}
					} else {
						cout << "[Error] Input wire \"" << wire_name << "\" has incomplete declared output.\n";
						exit(1);
					}
				} else {
					if (from.size() == 2) {
						const auto &from_comp = comps[from_name];
						const auto &from_port = from["port"].as<string>();

						string from_port_name;
						size_t from_begin_idx = 0;
						size_t from_end_idx = 0;
						ParsePortAndIndex(wire_name, from_port, from_port_name, from_begin_idx, from_end_idx);
						Connect(from_comp, from_port_name, wire, b_idx + from_begin_idx);

						if (IsComponentDeclared(comps, from_name)) {
							for (std::size_t i = 0; i < to_components.size(); ++i) {
								if (comps.find(to_components[i]->GetName()) != comps.end()) {
									const auto &to_comp = to_components[i];
									const auto &to_port = to_port_names[i];
								
									string to_port_name;
									size_t to_begin_idx = 0;
									size_t to_end_idx = 0;
								
									ParsePortAndIndex(wire_name, to_port, to_port_name, to_begin_idx, to_end_idx);
									Connect(to_comp, to_port_name, wire, b_idx + to_begin_idx);
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
					if (wire_bundle) {
						wire_bundle->SetAsInputBundle();
					}
				} else if (output) {
					wire->SetAsOutputWire();
					if (wire_bundle) {
						wire_bundle->SetAsOutputBundle();
					}
				}
			}
		} else {
			cout << "[Error] Wire \"" << wire_name << "\" declaration needs exactly 1 \"from\" section and 1 \"to\" section.\n";
			exit(1);
		}
	}
}

const string ValueToBinaryString(int64_t value, size_t size) {
	stringstream stream;
	stream << "0b";

	for (size_t i = 0; i < size; ++i) {
		const size_t sv = size - i - 1;

		stream << (((1ul << sv) & value) > 0);
		if (sv % 4 == 0 && sv != 0) {
			stream << '_';
		}
	}

	return stream.str();
}

const string ValueToHexString(int64_t value) {
	stringstream stream;
	stream << "0x" << hex << value;
	return stream.str();
}

struct Constraint {
	enum class TYPE {NONE, RNG};

	Constraint(const string _wire_name,
			   const size_t _begin_index,
			   const size_t _end_index,
			   const TYPE _type,
			   const size_t _seed,
			   const float _sigma,
			   const size_t _times)
		: wire_name(_wire_name)
		, begin_index(_begin_index)
		, end_index(_end_index)
		, type(_type)
		, seed(_seed)
		, sigma(_sigma)
		, times(_times)
	{
		generator = default_random_engine(seed);
		norm_distr = normal_distribution<float>(0.0f, sigma);
		uniform_int_distr = uniform_int_distribution<int32_t>(0, 1);
	}

	string wire_name;
	size_t begin_index;
	size_t end_index;
	TYPE type;
	size_t seed;
	float  sigma;
	default_random_engine generator;
	normal_distribution<float> norm_distr;
	uniform_int_distribution<int32_t> uniform_int_distr;
	size_t times;
};

using constr_t = shared_ptr<Constraint>;

const constr_t ParseConstraint(const YAML::Node &node) {
	string wire_name;
	size_t beg_idx = UINT_MAX;
	size_t end_idx = UINT_MAX;
	Constraint::TYPE type;
	size_t seed = 0;
	float sigma = 1.0f;
	size_t times = 1;

	if (node["wire"]) {
		wire_name = node["wire"].as<string>();
	} else {
	    cout << "[Error] A constraint needs at least a \"wire\", but none was given.\n";
		exit(1);
	}

	if (node["begin_index"]) {
		try {
			beg_idx = node["begin_index"].as<size_t>();
		} catch (YAML::TypedBadConversion<size_t> e) {
			cout << "[Error] \"begin_index\" is not a number: " << e.msg << '\n';
			exit(1);
		}
	}

	if (node["end_index"]) {
		try {
			end_idx = node["end_index"].as<size_t>();
		} catch (YAML::TypedBadConversion<size_t> e) {
			cout << "[Error] \"end_index\" is not a number: " << e.msg << '\n';
			exit(1);
		}
	}

	if (node["type"]) {
		const auto &type_string = node["type"].as<string>();

		if (type_string.compare("rng") == 0) {
			type = Constraint::TYPE::RNG;
		} else {
			type = Constraint::TYPE::NONE;
		}
	} else {
		cout << "[Error] A constraint needs at least a \"type\", but none was given.\n";
		exit(1);
	}

	if (node["seed"]) {
		if (type != Constraint::TYPE::RNG) {
			cout << "[Warning] Constraint type is not \"rng\", so \"seed\" is ignored.\n";
		} else {
			try {
				seed = node["seed"].as<size_t>();
			} catch (YAML::TypedBadConversion<size_t> e) {
				cout << "[Error] \"seed\" is not a number: " << e.msg << '\n';
				exit(1);
			}
		}
	}

	if (node["sigma"]) {
		if (type != Constraint::TYPE::RNG) {
			cout << "[Warning] Constraint type is not \"rng\", so \"sigma\" is ignored.\n";
		} else {
			try {
				sigma = node["sigma"].as<float>();
				if (sigma <= 0.0f) {
					cout << "[Error] \"sigma\" must be larger than 0.0.\n";
					exit(1);
				}
			} catch (YAML::TypedBadConversion<float> e) {
				cout << "[Error] \"sigma\" is not a number: " << e.msg << '\n';
				exit(1);
			}
		}
	}

	if (node["times"]) {
		try {
			times = node["times"].as<size_t>();
		} catch (YAML::TypedBadConversion<size_t> e) {
			cout << "[Error] \"times\" is not a number: " << e.msg << '\n';
			exit(1);
		}
	}

	return make_shared<Constraint>(wire_name, beg_idx, end_idx, type, seed, sigma, times);
}

void ParseStimuli(System &system, YAML::Node config, const string &config_file_name) {
	const auto &stimuli = config["stimuli"];

	auto error_invalid_value = [](const auto &val) {
		cout << "[Error] Value \"" << val << "\" in stimuli section "
			 << "is invalid. It should begin with either '0b'/'0B', '0x'/'0X', "
			 << "or '0d'/'0D' for binary, hexadecimal, and decimal "
			 << "representations respectively, then followed by a value.\n";
		exit(1);
	};

	auto error_constraint_map = []() {
		cout << "[Error] A \"constraint\" needs to be either a map, or a sequence "
			 << " of maps. In either case it needs to have at least the "
			 << "\"wire\", \"begin_index\", and \"type\" keys.\n";
		exit(1);
	};

	auto error_non_existent_wire = [](const auto &name) {
		cout << "[Error] No wire or wire bundle \"" << name << "\" found.\n";
		exit(1);
	};

	auto process_wire_rng = [](const auto &wire, const auto &constraint, auto &system) {
		for (size_t i = 0; i < constraint->times; ++i) {
			auto rnd_val = constraint->uniform_int_distr(constraint->generator);
			if (rnd_val < 0.0f) {
				wire->SetValue(false, false);
			} else {
				wire->SetValue(true, false);
			}
			system.Update();
		}
	};

	map<string, vector<int64_t>> io_values;
	for (const auto &[name, bundle] : system.GetWireBundles()) {
		io_values[name] = vector<int64_t>();
	}
	vector<size_t> toggles = {};
	vector<float> sigmas = {};

	auto process_wire_bundle_rng = [&](const auto &wb, const auto &constraint, auto &system) {
		const float scale_factor = (float)((1l << wb->GetSize()) - 1);
		for (size_t i = 0; i < constraint->times; ++i) {
			auto rnd_val = constraint->norm_distr(constraint->generator) * scale_factor;
			if (rnd_val > scale_factor) {
				rnd_val = scale_factor;
			}

			wb->SetValue((int64_t)rnd_val, false);
			io_values[wb->GetName()].emplace_back((int64_t)rnd_val);
			system.Update();
		}
	};

	size_t prev_toggles = 0;

	vector<constr_t> constraints;

	for (size_t i = 0; i < stimuli.size(); ++i) {
		cout << "\nStimulus " << i << '\n';
		for (const auto &step : stimuli[i]) {
			const auto &key_name = step.first.as<string>();
			const auto &value_node = step.second;

			if (key_name.compare("constraint") == 0) {
				// The key is a constraint.
				if (value_node.IsSequence()) {
					// The constraint applies to multiple wires or wire bundles.
					size_t max_repetitions = 1;

					for (YAML::const_iterator it = value_node.begin(); it != value_node.end(); ++it) {
						const auto &c = ParseConstraint(*it);

						if (c->times > max_repetitions) {
							max_repetitions = c->times;
						}

						constraints.emplace_back(c);
						sigmas.emplace_back(c->sigma);
					}

					size_t prev = system.GetNumToggles();
					for (size_t i = 0; i < max_repetitions; ++i) {
						for (const auto &c : constraints) {
							if (c->times) {
								c->times--;
								const auto &w = system.GetWire(c->wire_name);
								const auto &wb = system.GetWireBundle(c->wire_name);

								if (w) {
									switch (c->type) {
									case Constraint::TYPE::RNG: {
										auto rnd_val = c->uniform_int_distr(c->generator);
										if (rnd_val < 0.0f) {
											w->SetValue(false, false);
										} else {
											w->SetValue(true, false);
										}
										break;
									}
									case Constraint::TYPE::NONE: break;
									}
								} else if (wb) {
									switch (c->type) {
									case Constraint::TYPE::RNG: {
										const float scale_factor = (float)((1l << wb->GetSize()) - 1);
										auto rnd_val = c->norm_distr(c->generator) * scale_factor;

										if (rnd_val > scale_factor) {
											rnd_val = scale_factor;
										}

										wb->SetValue((int64_t)rnd_val, false);

										io_values[wb->GetName()].emplace_back((int64_t)rnd_val);
										break;
									}
									case Constraint::TYPE::NONE: break;
									}
								}
							}
						}

						system.Update();
						for (const auto &o : system.GetOutputWireBundles()) {
							io_values[o->GetName()].emplace_back(o->Get2CValue());
						}

						const size_t curr_toggles = system.GetNumToggles();
						toggles.emplace_back(curr_toggles - prev);
						prev = curr_toggles;
					}
				} else if (value_node.IsMap()) {
					// The constraint applies to one wire or wire bundle.
					const auto &c = ParseConstraint(value_node);
					const auto &w = system.GetWire(c->wire_name);
					const auto &wb = system.GetWireBundle(c->wire_name);

					if (w) {
						switch (c->type) {
						case Constraint::TYPE::RNG: process_wire_rng(w, c, system); break;
						case Constraint::TYPE::NONE: break;
						}
					} else if (wb) {
						switch (c->type) {
						case Constraint::TYPE::RNG: process_wire_bundle_rng(wb, c, system); break;
						case Constraint::TYPE::NONE: break;
						}
					} else {
						error_non_existent_wire(c->wire_name);
					}
				} else {
					error_constraint_map();
				}

				continue;
			} else {
				// The key is a wire or wire bundle.
				const auto &value_name = value_node.as<string>();
				const auto &wire = system.GetWire(key_name);
				const auto &wb = system.GetWireBundle(key_name);

				if (wire) {
					auto value = false;

					if (value_name.compare("1") == 0 || value_name.compare("true") == 0) {
						value = true;
					} else if (value_name.compare("0") == 0 || value_name.compare("false") == 0) {
						value = false;
					} else {
						cout << "[Error] stimulus value of wire \"" << key_name
							 <<	"\" has to be one of the following: 0, 1, true, false.\n";
						exit(1);
					}
					wire->SetValue(value, false);

					// Print the wire name and value.
					cout << key_name << ": " << value << '\n';
				} else if (wb) {
					auto value_string = value_name;
					auto base = 2;

					// A wire bundle value begins with either:
					// * "0b" for binary representation
					// * "0x" for hexadecimal representation
					// * "0d" for decimal representation
					if (value_string.length() > 2) {
						const auto &prefix = value_string.substr(0, 2);
						if (prefix.compare("0b") == 0 || prefix.compare("0B") == 0) {
							base = 2;
						} else if (prefix.compare("0x") == 0 || prefix.compare("0X") == 0) {
							base = 16;
						} else if (prefix.compare("0d") == 0 || prefix.compare("0D") == 0) {
							base = 10;
						} else {
							error_invalid_value(value_string);
						}
					} else {
						error_invalid_value(value_string);
					}

					// Remove the prefix
					value_string.erase(0, 2);

					try {
						const int64_t value = stol(value_string, 0, base);
						wb->SetValue(value, false);

						// Print the bundle name and value in hex and binary.
						cout << wb->GetName() << ": "
							 << wb->Get2CValue() << " "
							 << ValueToHexString(wb->GetValue()) << " "
							 << ValueToBinaryString(wb->GetValue(), wb->GetSize()) << '\n';
					} catch (invalid_argument e) {
						error_invalid_value(value_string);
					} catch (out_of_range e) {
						cout << "[Error] Value \"" << value_string << "\" is too large.\n";
						exit(1);
					}
				} else {
					cout << "[Error] Non-existent wire or wire bundle \"" << key_name << "\" found in stimuli section.\n";
					exit(1);
				}
			}
		}

		system.Update();

		const size_t curr_toggles = system.GetNumToggles();
		cout << "#toggles: " << (curr_toggles - prev_toggles) << "\n";
		prev_toggles = curr_toggles;
	}

	auto outfile_name = config_file_name.substr(0, config_file_name.find_last_of("."));
	auto outfile = ofstream(outfile_name + ".txt");
	size_t i = 0;
	for (const auto &[name, vals] : io_values) {
		outfile << name << ' ' << system.GetWireBundle(name)->GetSize();

		if (sigmas.size()) {
			outfile << ' ' << sigmas[i++] << '\n';
		} else {
			outfile << '\n';
		}

		for (const auto &val : vals) {
			outfile << val << ',';
		}
		outfile << '\n';
	}
	outfile << "toggles\n";
	for (const auto &val : toggles) {
		outfile << val << ',';
	}
	outfile.close();
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
		const auto &components = config["components"];
		const auto &wires = config["wires"];
		const auto &stimuli = config["stimuli"];

		if (!components) {
			cout << "[Error] No \"components\" section found in \"" << config_file_name << "\"\n";
			exit(1);
		}
		if (components.size() == 0) {
			cout << "[Error] \"components\" section in \"" << config_file_name << "\" is empty.\n";
			exit(1);
		}
		if (!wires) {
			cout << "[Error] No \"wires\" section found in \"" << config_file_name << "\"\n";
			exit(1);
		}
		if (wires.size() == 0) {
			cout << "[Error] \"wires\" section in \"" << config_file_name << "\" is empty.\n";
			exit(1);
		}
		if (!stimuli) {
			cout << "[Error] No \"stimuli\" section found in \"" << config_file_name << "\"\n";
			exit(1);
		}
		if (stimuli.size() == 0) {
			cout << "[Error] \"stimuli\" section in \"" << config_file_name << "\" is empty.\n";
			exit(1);
		}

		ParseComponents(comps, config);
		ParseWires(comps, config);

		for (const auto &c : comps) {
			system.AddComponent(c.second);
		}

		system.FindLongestPathInSystem();
		system.FindInitialState();

		cout << "Longest path in the system: " << system.GetLongestPath() << '\n';
		cout << "Number of components: " << system.GetNumComponents() <<
			"\nNumber of wires: " << system.GetNumWires() << '\n';

		ParseStimuli(system, config, config_file_name);

		cout << "\nSimulation done!\n";
		cout << "Number of toggles: " << system.GetNumToggles() << '\n';

#if 1
		cout << "\nValue of all wires:\n";
		for (const auto &[ow_name, ow] : system.GetWires()) {
			cout << "Wire \"" << ow_name
				 << "\": " << ow->GetValue()
				 << "\t#outputs: " << ow->GetNumOutputs()
				 << "\t#toggles: " << ow->GetNumToggles()
				 << '\n';
		}

		cout << "\nValue of all components:\n";
		for (const auto &c : system.GetComponents()) {
			cout << "Component \"" << c->GetName()
				 << "\" #toggles: " << c->GetNumToggles()
				 << '\n';
		}
#endif
		cout << "\nInputs:\n";
		vector<wb_t> processed_wire_bundles;

		for (const auto &iw : system.GetInputWires()) {
			const auto &wb = iw->GetWireBundle();
			if (wb) {
				if (find(processed_wire_bundles.begin(),
						 processed_wire_bundles.end(),
						 wb) == processed_wire_bundles.end())
				{
					const auto value = wb->GetValue();
					cout << wb->GetName() << ": "
						 << wb->Get2CValue() << " "
						 << ValueToHexString(value) << " "
						 << ValueToBinaryString(value, wb->GetSize()) << '\n';

					processed_wire_bundles.push_back(wb);
				}
			} else {
				cout << iw->GetName() << ": " << iw->GetValue() << '\n';
			}
		}

		cout << "\nOutputs:\n";
		processed_wire_bundles.clear();

		for (const auto &ow : system.GetOutputWires()) {
			const auto &wb = ow->GetWireBundle();
			if (wb) {
				if (find(processed_wire_bundles.begin(),
						 processed_wire_bundles.end(),
						 wb) == processed_wire_bundles.end())
				{
					const auto value = wb->GetValue();
					cout << wb->GetName() << ": "
						 << wb->Get2CValue() << " "
						 << ValueToHexString(value) << " "
						 << ValueToBinaryString(value, wb->GetSize()) << '\n';

					processed_wire_bundles.push_back(wb);
				}
			} else {
				cout << ow->GetName() << ": " << ow->GetValue() << '\n';
			}
		}
	}
#if 0
	// Booth encoder radix-4
	const auto be = make_shared<BoothEncoderRadix4>("be_0");
	const auto bd1 = make_shared<BoothDecoderRadix4>("bd_1");
	const auto bd2 = make_shared<BoothDecoderRadix4>("bd_2");
	const auto bd3 = make_shared<BoothDecoderRadix4>("bd_3");

	// Input wires
	const auto w2x_i = make_shared<Wire>("2x_i");
	const auto w2x_i_p1 = make_shared<Wire>("2x_i_p1");
	const auto w2x_i_m1 = make_shared<Wire>("2x_i_m1");
	const auto y_lsb = make_shared<Wire>("y_lsb");
	const auto y1 = make_shared<Wire>("y1");
	const auto y2 = make_shared<Wire>("y2");
	const auto y_msb = make_shared<Wire>("y_msb");
	be->Connect(PORTS::X_2I, w2x_i);
	be->Connect(PORTS::X_2I_PLUS_ONE, w2x_i_p1);
	be->Connect(PORTS::X_2I_MINUS_ONE, w2x_i_m1);
	be->Connect(PORTS::Y_LSB, y_lsb);
	be->Connect(PORTS::Y_MSB, y_msb);

	// Output wires
	const auto neg = make_shared<Wire>("neg");
	const auto row_lsb = make_shared<Wire>("row_lsb");
	const auto x1b = make_shared<Wire>("x1b");
	const auto x2b = make_shared<Wire>("x2b");
	const auto se = make_shared<Wire>("se");
	const auto z = make_shared<Wire>("z");
	const auto neg_cin = make_shared<Wire>("Neg_cin");
	const auto ppt1 = make_shared<Wire>("ppt1");
	const auto ppt2 = make_shared<Wire>("ppt2");
	const auto ppt3 = make_shared<Wire>("ppt3");

	// Connections
	// Encoder
	be->Connect(PORTS::NEG, neg);
	be->Connect(PORTS::ROW_LSB, row_lsb);
	be->Connect(PORTS::X1_b, x1b);
	be->Connect(PORTS::X2_b, x2b);
	be->Connect(PORTS::SE, se);
	be->Connect(PORTS::Z, z);
	be->Connect(PORTS::NEG_CIN, neg_cin);

	// Decoder 1 input
	bd1->Connect(PORTS::Yj, y1);
	bd1->Connect(PORTS::Yj_m1, y_lsb);
	bd1->Connect(PORTS::NEG, w2x_i_p1);
	bd1->Connect(PORTS::X1_b, x1b);
	bd1->Connect(PORTS::X2_b, x2b);
	bd1->Connect(PORTS::Z, z);

	// Decoder 1 output
	bd1->Connect(PORTS::PPTj, ppt1);
	
	// Decoder 2 input
	bd2->Connect(PORTS::Yj, y2);
	bd2->Connect(PORTS::Yj_m1, y1);
	bd2->Connect(PORTS::NEG, w2x_i_p1);
	bd2->Connect(PORTS::X1_b, x1b);
	bd2->Connect(PORTS::X2_b, x2b);
	bd2->Connect(PORTS::Z, z);

	// Decoder 2 output
	bd2->Connect(PORTS::PPTj, ppt2);

	// Decoder 3 input
	bd3->Connect(PORTS::Yj, y_msb);
	bd3->Connect(PORTS::Yj_m1, y2);
	bd3->Connect(PORTS::NEG, w2x_i_p1);
	bd3->Connect(PORTS::X1_b, x1b);
	bd3->Connect(PORTS::X2_b, x2b);
	bd3->Connect(PORTS::Z, z);

	// Decoder 3 output
	bd3->Connect(PORTS::PPTj, ppt3);

	// Stimuli

	// y = 1100
	// x = 10
	// 1100 * 10 = -4 * -2 = 8 = 001000
	w2x_i_m1->SetValue(false); // x[1]
	w2x_i->SetValue(false);    // x[0]
	w2x_i_p1->SetValue(true);  // x[-1] == 0

	y_lsb->SetValue(false); // y[0]
	y1->SetValue(false);    // y[1]
	y2->SetValue(true);     // y[2]
	y_msb->SetValue(true);  // y[3]

	// y = 1000
	// x = 11
	// 1000 * 11 = -8 * -1 = 8 = 001000
//	w2x_i_m1->SetValue(false); // x[1]
//	w2x_i->SetValue(true);     // x[0]
//	w2x_i_p1->SetValue(true);  // x[-1] == 0
//
//	y_lsb->SetValue(false); // y[0]
//	y1->SetValue(false);    // y[1]
//	y2->SetValue(false);    // y[2]
//	y_msb->SetValue(true);  // y[3]
	
	// y = 1000
	// x = 01
	// 1000 * 01 = -8 * 1 = -8 = 111000
//	w2x_i_m1->SetValue(false); // x[1]
//	w2x_i->SetValue(true);     // x[0]
//	w2x_i_p1->SetValue(false); // x[-1] == 0
//
//	y_lsb->SetValue(false); // y[0]
//	y1->SetValue(false);    // y[1]
//	y2->SetValue(false);    // y[2]
//	y_msb->SetValue(true);  // y[3]

	// y = 1100
	// x = 11
	// 1100 * 11 = -4 * -1 = 4 = 000100
//	w2x_i_m1->SetValue(false); // x[1]
//	w2x_i->SetValue(true);     // x[0]
//	w2x_i_p1->SetValue(true);  // x[-1] == 0
//
//	y_lsb->SetValue(false); // y[0]
//	y1->SetValue(false);    // y[1]
//	y2->SetValue(true);     // y[2]
//	y_msb->SetValue(true);  // y[3]

	// y = 0000
	// x = 11
	// 0000 * 11 = 0 * -1 = 4 = 000000
//	w2x_i_m1->SetValue(false); // x[1]
//	w2x_i->SetValue(true);     // x[0]
//	w2x_i_p1->SetValue(true);  // x[-1] == 0
//
//	y_lsb->SetValue(false); // y[0]
//	y1->SetValue(false);    // y[1]
//	y2->SetValue(false);    // y[2]
//	y_msb->SetValue(false); // y[3]

	// Update
	be->Update(false);
	bd1->Update(false);
	bd2->Update(false);
	bd3->Update(false);

	// Print values
	cout << "\nEncoder:";
	cout << "\n2x_i_m1:\t" << w2x_i_m1->GetValue()
		 << "\n2x_i:\t\t" << w2x_i->GetValue()
		 << "\n2x_i_p1:\t" << w2x_i_p1->GetValue()
		 << "\ny_lsb:\t\t" << y_lsb->GetValue()
		 << "\ny_msb:\t\t" << y_msb->GetValue() << '\n';
	cout << "\nx1b:\t\t" << x1b->GetValue()
		 << "\nx2b:\t\t" << x2b->GetValue()
		 << "\nneg:\t\t" << w2x_i_p1->GetValue()//neg->GetValue()
		 << "\nz:\t\t" << z->GetValue()
		 << "\nrow_lsb:\t" << row_lsb->GetValue()
		 << "\nse:\t\t" << se->GetValue()
		 << "\nneg_cin:\t" << neg_cin->GetValue() << '\n';

	cout << "\nDecoder:";
	cout << "\ny0:\t\t" << y_lsb->GetValue()
		 << "\ny1:\t\t" << y1->GetValue()
		 << "\ny2:\t\t" << y2->GetValue()
		 << "\ny3:\t\t" << y_msb->GetValue()
		 << "\nppt1:\t\t" << ppt1->GetValue()
		 << "\nppt2:\t\t" << ppt2->GetValue()
		 << "\nppt3:\t\t" << ppt3->GetValue();

	const size_t ppt = ((*se)() << 6)
		| (!(*se)() << 5)
		| (!(*se)() << 4)
		| ((*ppt3)() << 3)
		| ((*ppt2)() << 2)
		| ((*ppt1)() << 1)
		| (*row_lsb)();
	const size_t full_ppt = ppt +  size_t((*neg_cin)() << 1);
	bitset<7> full_ppt_bin(full_ppt);
	bitset<6> final_result(full_ppt & 0x3F);

	cout << "\n\nFull partial product: " << full_ppt_bin << "\nFinal result: " << final_result << '\n';

	//const auto bm = Multiplier_2C_Booth("Booth", 4, 2);

	const auto ppt1_r4d = make_shared<Wire>("ppt1_r4d");
	const auto ppt2_r4d = make_shared<Wire>("ppt2_r4d");
	const auto ppt3_r4d = make_shared<Wire>("ppt3_r4d");
	
	auto r4d = Radix4BoothDecoder("r4d", 4);
	r4d.Connect(PORTS::Yj, y_lsb, 0);
	r4d.Connect(PORTS::Yj, y1, 1);
	r4d.Connect(PORTS::Yj, y2, 2);
	r4d.Connect(PORTS::Yj, y_msb, 3);
	r4d.Connect(PORTS::NEG, w2x_i_p1);
	r4d.Connect(PORTS::X1_b, x1b);
	r4d.Connect(PORTS::X2_b, x2b);
	r4d.Connect(PORTS::Z, z);
	r4d.Connect(PORTS::O, ppt1_r4d, 0);
	r4d.Connect(PORTS::O, ppt2_r4d, 1);
	r4d.Connect(PORTS::O, ppt3_r4d, 2);

	r4d.Update(false);

	const size_t ppt_r4d = ((*se)() << 6)
		| (!(*se)() << 5)
		| (!(*se)() << 4)
		| ((*ppt3_r4d)() << 3)
		| ((*ppt2_r4d)() << 2)
		| ((*ppt1_r4d)() << 1)
		| (*row_lsb)();
	const size_t full_ppt_r4d = ppt_r4d +  size_t((*neg_cin)() << 1);
	bitset<7> full_ppt_bin_r4d(full_ppt_r4d);
	bitset<6> final_result_r4d(full_ppt_r4d & 0x3F);

	cout << "\nFull partial product: " << full_ppt_bin_r4d << "\nFinal result: " << final_result_r4d << '\n';
#endif
	return 0;
}
