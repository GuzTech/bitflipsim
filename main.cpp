#include "main.h"
#include <yaml-cpp/yaml.h>

using namespace std;

bool IsComponentDeclared(const map<string, comp_t> &comps, const string &name) {
	return comps.find(name) != comps.end();
}

void Connect(const comp_t &component, const string &port_name, const wire_t &wire, size_t index = 0) {
	const auto &fa_comp   = dynamic_pointer_cast<FullAdder>(component);
	const auto &ha_comp   = dynamic_pointer_cast<HalfAdder>(component);
	const auto &and_comp  = dynamic_pointer_cast<And>(component);
	const auto &or_comp   = dynamic_pointer_cast<Or>(component);
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

void ParseWireAndSize(string wire_string, string &wire_name, size_t &size) {
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
		if (wire_string.length() != 0 && pos == string::npos) {
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
			cout << "[Error] A numeric size should be given after wire name \"" << wire_name
				 << '\n';
			exit(1);
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
		ParseWireAndSize(wire_string, wire_name, bundle_size);

		if (it->second.size() == 2) {
			wire_t wire;
			wb_t wire_bundle = nullptr;
			size_t size = 1;

			if (bundle_size > 0) {
				wire_bundle = make_shared<WireBundle>(wire_name, bundle_size);
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
				} else if (output) {
					wire->SetAsOutputWire();
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

void ParseConstraint(const YAML::Node &node,
					 string &wire_name,
					 size_t &beg_idx,
					 size_t &end_idx,
					 string &type,
					 size_t &seed) {
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
	} else {
		cout << "[Error] \"begin_index\" is mandatory in a constraint, but none was given.\n";
		exit(1);
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
		type = node["type"].as<string>();
	} else {
		cout << "[Error] A constraint needs at least a \"type\", but none was given.\n";
		exit(1);
	}

	if (node["seed"]) {
		if (type.compare("rng") != 0) {
			cout << "[Warning] Contraint type is not \"rng\", so \"seed\" is ignored.\n";
		} else {
			try {
				seed = node["seed"].as<size_t>();
			} catch (YAML::TypedBadConversion<size_t> e) {
				cout << "[Error] \"seed\" is not a number: " << e.msg << '\n';
				exit(1);
			}
		}
	}
}

void ParseStimuli(System &system, YAML::Node config) {
	const auto &stimuli = config["stimuli"];

	auto error_invalid_value = [](const auto &val) {
		cout << "[Error] Value \"" << val << "\" in stimuli section "
			 << "is invalid. It should begin with either '0b'/'0B', '0x'/'0X', "
			 << "or '0d'/'0D' for binary, hexadecimal, and decimal "
			 << "representations respectively, then followed by a value.\n";
		exit(1);
	};

	auto error_constraint_map = []() {
		cout << "[Error] A \"constraint\" needs to be a map that has at least the "
			 << "\"wire\", \"begin_index\", and \"type\" keys.\n";
		exit(1);
	};

	size_t prev_toggles = 0;

	for (size_t i = 0; i < stimuli.size(); ++i) {
		cout << "\nStimulus " << i << '\n';
		for (const auto &step : stimuli[i]) {
			const auto &key_name = step.first.as<string>();
			const auto &value = step.second;

			if (key_name.compare("constraint") == 0) {
				if (value.IsMap()) {
					string wire_name;
					size_t beg_idx = UINT_MAX;
					size_t end_idx = UINT_MAX;
					string type;
					size_t seed = 0;

					ParseConstraint(value, wire_name, beg_idx, end_idx, type, seed);

					cout << "Wire: " << wire_name << '\n';
					cout << "Begin index: " << beg_idx << '\n';
					cout << "End index: " << end_idx << '\n';
					cout << "Type: " << type << '\n';
					cout << "Seed: " << seed << '\n';
				} else {
					error_constraint_map();
				}

				continue;
			} else {
				const auto &value_name = value.as<string>();
				const auto &wire = system.GetWire(key_name);
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
				} else {
					const auto &wb = system.GetWireBundle(key_name);
					if (wb) {
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
								 << value << " "
								 << ValueToHexString(value) << " "
								 << ValueToBinaryString(value, wb->GetSize()) << '\n';
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
		}

		system.Update();

		const size_t curr_toggles = system.GetNumToggles();
		cout << "#toggles: " << (curr_toggles - prev_toggles) << "\n";
		prev_toggles = curr_toggles;
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

		ParseStimuli(system, config);

		cout << "\nSimulation done!\n";
		cout << "Number of toggles: " << system.GetNumToggles() << '\n';

#if 0
		cout << "\nValue of all wires:\n";
		for (const auto &ow : system.GetWires()) {
			cout << "Wire \"" << ow->GetName()
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
#else
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
						 << value << " "
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
						 << value << " "
						 << ValueToHexString(value) << " "
						 << ValueToBinaryString(value, wb->GetSize()) << '\n';

					processed_wire_bundles.push_back(wb);
				}
			} else {
				cout << ow->GetName() << ": " << ow->GetValue() << '\n';
			}
		}
#endif
	}

	return 0;
}
