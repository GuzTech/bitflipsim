#include "main.h"
#include <yaml-cpp/yaml.h>
#include <random>
#include <experimental/filesystem>
#include <stdnoreturn.h>

using namespace std;

inline
bool IsComponentDeclared(const comp_map_t &comps, const string &name) {
	return comps.find(name) != comps.end();
}

void Connect(const comp_t &component, const PORTS port, const wire_t &wire, size_t index = 0) {
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
	const auto &rcas_comp = dynamic_pointer_cast<RippleCarryAdderSubtracter>(component);
	const auto &rcs_comp  = dynamic_pointer_cast<RippleCarrySubtracter>(component);
	const auto &m2C_comp  = dynamic_pointer_cast<Multiplier_2C>(component);
	const auto &smag_comp = dynamic_pointer_cast<Multiplier_Smag>(component);
	const auto &m2C_booth_comp = dynamic_pointer_cast<Multiplier_2C_Booth>(component);
	const auto &smTo2C_comp    = dynamic_pointer_cast<SmagTo2C>(component);

	auto error_non_existent_port = [&]() {
		Error(string("Wire \"") + wire->GetName() + string("\" wants to connect to non-existent port \"")
			  + PortToPortNameMap[port] + string("\" of component \"") + component->GetName() + ("\".\n"));
	};

	if (fa_comp != nullptr) {
		switch (port) {
		case PORTS::A:
		case PORTS::B:
		case PORTS::Cin:
		case PORTS::O:
		case PORTS::Cout:
			fa_comp->Connect(port, wire);
			break;
		default:
			error_non_existent_port();
		}
	} else if (ha_comp != nullptr) {
		switch (port) {
		case PORTS::A:
		case PORTS::B:
		case PORTS::O:
		case PORTS::Cout:
			ha_comp->Connect(port, wire);
			break;
		default:
			error_non_existent_port();
		}
	} else if (and_comp != nullptr) {
		switch (port) {
		case PORTS::A:
		case PORTS::B:
		case PORTS::O:
			 and_comp->Connect(port, wire);
			 break;
		default:
			error_non_existent_port();
		}
	} else if (or_comp != nullptr) {
		switch (port) {
		case PORTS::A:
		case PORTS::B:
		case PORTS::O:
			or_comp->Connect(port, wire);
			break;
		default:
			error_non_existent_port();
		}
	} else if (or3_comp != nullptr) {
		switch (port) {
		case PORTS::A:
		case PORTS::B:
		case PORTS::C:
		case PORTS::O:
			or3_comp->Connect(port, wire);
			break;
		default:
			error_non_existent_port();
		}
	} else if (xor_comp != nullptr) {
		switch (port) {
		case PORTS::A:
		case PORTS::B:
		case PORTS::O:
			xor_comp->Connect(port, wire);
			break;
		default:
			error_non_existent_port();
		}
	} else if (nand_comp != nullptr) {
		switch (port) {
		case PORTS::A:
		case PORTS::B:
		case PORTS::O:
			nand_comp->Connect(port, wire);
			break;
		default:
			error_non_existent_port();
		}
	} else if (nor_comp != nullptr) {
		switch (port) {
		case PORTS::A:
		case PORTS::B:
		case PORTS::O:
			nor_comp->Connect(port, wire);
			break;
		default:
			error_non_existent_port();
		}
	} else if (xnor_comp != nullptr) {
		switch (port) {
		case PORTS::A:
		case PORTS::B:
		case PORTS::O:
			xnor_comp->Connect(port, wire);
			break;
		default:
			error_non_existent_port();
		}
	} else if (not_comp != nullptr) {
		switch (port) {
		case PORTS::I:
		case PORTS::O:
			not_comp->Connect(port, wire);
			break;
		default:
			error_non_existent_port();
		}
	} else if (mux_comp != nullptr) {
		switch (port) {
		case PORTS::A:
		case PORTS::B:
		case PORTS::S:
		case PORTS::O:
			mux_comp->Connect(port, wire);
			break;
		default:
			error_non_existent_port();
		}
	} else if (rca_comp != nullptr) {
		switch (port) {
		case PORTS::A:
		case PORTS::B:
		case PORTS::Cin:
		case PORTS::O:
		case PORTS::Cout:
			rca_comp->Connect(port, wire, index);
			break;
		default:
			error_non_existent_port();
		}
	} else if (rcas_comp != nullptr) {
		switch (port) {
		case PORTS::A:
		case PORTS::B:
		case PORTS::Cin:
		case PORTS::O:
		case PORTS::Cout:
			rcas_comp->Connect(port, wire, index);
			break;
		default:
			error_non_existent_port();
		}
	} else if (rcs_comp != nullptr) {
		switch (port) {
		case PORTS::A:
		case PORTS::B:
		case PORTS::O:
		case PORTS::Cout:
			rcs_comp->Connect(port, wire, index);
			break;
		default:
			error_non_existent_port();
		}
	} else if (m2C_comp != nullptr) {
		switch (port) {
		case PORTS::A:
		case PORTS::B:
		case PORTS::O:
			m2C_comp->Connect(port, wire, index);
			break;
		default:
			error_non_existent_port();
		}
	} else if (smag_comp != nullptr) {
		switch (port) {
		case PORTS::A:
		case PORTS::B:
		case PORTS::O:
			smag_comp->Connect(port, wire, index);
			break;
		default:
			error_non_existent_port();
		}
	} else if (m2C_booth_comp != nullptr) {
		switch (port) {
		case PORTS::A:
		case PORTS::B:
		case PORTS::O:
			m2C_booth_comp->Connect(port, wire, index);
			break;
		default:
			error_non_existent_port();
		}
	} else if (smTo2C_comp != nullptr) {
		switch (port) {
		case PORTS::A:
		case PORTS::B:
		case PORTS::O:
			smTo2C_comp->Connect(port, wire, index);
			break;
		default:
			error_non_existent_port();
		}
	}
}

//void ParseWireAndSize2(string wire_string, string &wire_name, size_t &size, WireBundle::REPR &repr) {
//	// Check if the wire name contains a space to indicate that it consists of the name and size of the wire bundle.
//	size_t pos = wire_string.find(" ");
//
//	try {
//		if (pos == npos) {
//			// The name is just the wire name and does not contain a size.
//			wire_name = wire_string;
//		} else {
//			// We assume the first value is the name.
//			wire_name = wire_string.substr(0, pos);
//
//			// We remove the name + " " part from the string, and search again.
//			// There should be a second part which should be a number to indicate the size.
//			wire_string.erase(0, pos + 1);
//
//			pos = wire_string.find(" ");
//		}
//	} catch (exception &e) {
//
//	}
//}

void ParseWireAndSize(string wire_string, string &wire_name, size_t &size, WireBundle::REPR &repr) {
	auto error_inconsistent = [](const auto &wire) {
		Error("Inconsistent wire declaration for wire \"" + wire + "\".\nFormat for wires:\n"
			  + "<wire name>\n\nFormat for wire bundles:\n<wire name> <size> [1c|1C|2c|2C|smag]\n");
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
					Error("Size for wire \"" + wire_name + "\" is not a number.\n");
				} catch (out_of_range e ) {
					Error("Size for wire \"" + wire_name + "\" is too large.\n");
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
						    Error("Unrecognized number representation for wire bundle \""
								  + wire_name + "\".\nOnly \"2c\"/\"2C\" (default), \"1c\"/\"1C\""
								  + ", and \"smag\" are supported.\n");
						}
					} else {
						error_inconsistent(wire_name);
					}
				} catch (invalid_argument e) {
					Error("Size for wire \"" + wire_name + "\" is not a number.\n");
				} catch (out_of_range e ) {
				    Error("Size for wire \"" + wire_name + "\" is too large.\n");
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
					Error("Index for port \"" + port_name + "\" of wire \""
						  + wire_name + "\" is not a number.\n");
				} catch (out_of_range e) {
					Error("Index number for port \"" + port_name + "\" of wire \""
						  + wire_name + "\" is too large.\n");
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
						Error("At most two indices can be given after port name \""
							  + port_name + "\" for wire \"" + wire_name + "\".\n");

						// This only happens if the user enters a port name with at least one space after it
						// in double quotation marks like so "A ". Else the YAML parser removes whitespace.
					}
				} catch (invalid_argument e) {
					Error("Index for port \"" + port_name + "\" of wire \""
						  + wire_name + "\" is not a number.\n");
				} catch (out_of_range e) {
					Error("Index number for port \"" + port_name + "\" of wire \""
						  + wire_name + "\" is too large.\n");
				}
			}
		}
	} else {
		// The name is just the port and does not contain an index.
		port_name = port_string;
	}
}

void ParseMultiplierComponent(comp_map_t &comps, const YAML::Node &multiplier) {
	string name;
	size_t num_bits_A = 0;
	size_t num_bits_B = 0;
	NUMFMT format = NUMFMT::NONE;
	LAYOUT layout = LAYOUT::NONE;
	TYPE type = TYPE::NONE;

	auto error_unsupported_configuration = [&]() {
		Error("Multiplier configuration not supported.\n");
	};

	if (multiplier["name"]) {
		name = multiplier["name"].as<string>();
	} else {
		Error("Multiplier has no name.\n");
	}

	if (multiplier["A width"]) {
		try {
			num_bits_A = multiplier["A width"].as<size_t>();
		} catch (YAML::TypedBadConversion<size_t> e) {
			Error("Multiplier \"" + name + "\" port A width is not a number: " + e.msg + '\n');
		}
	} else {
		Error("Property \"A width\" for multiplier \"" + name + "\" is required, but was not found.\n");
	}

	if (multiplier["B width"]) {
		try {
			num_bits_B = multiplier["B width"].as<size_t>();
		} catch (YAML::TypedBadConversion<size_t> e) {
			Error("Multiplier \"" + name + "\" port B width is not a number: " + e.msg + '\n');
		}
	} else {
		Error("Property \"B width\" for multiplier \"" + name + "\" is required, but was not found.\n");
	}

	if (multiplier["number format"]) {
		const auto &fmt_string = multiplier["number format"].as<string>();

		if (fmt_string.compare("2C") == 0)			format = NUMFMT::TWOS_COMPLEMENT;
		else if (fmt_string.compare("1C") == 0) 	format = NUMFMT::ONES_COMPLEMENT;
		else if (fmt_string.compare("SMAG") == 0) 	format = NUMFMT::SIGNED_MAGNITUDE;
		else if (fmt_string.compare("UINT") == 0)	format = NUMFMT::UNSIGNED;
		else {
			Error("Property \"number format\" for multiplier \"" + name + "\" has an unsupported value. "
				  + "Supported values are \"2C\", \"1C\", \"SMAG\", and \"UINT\".\n");
		}
	} else {
		Error("Property \"number format\" for multiplier \"" + name + "\" is required, but was not found.\n");
	}

	if (multiplier["layout"]) {
		const auto &layout_string = multiplier["layout"].as<string>();

		if (layout_string.compare("carry propagate") == 0) 		layout = LAYOUT::CARRY_PROPAGATE;
		else if (layout_string.compare("carry save") == 0)		layout = LAYOUT::CARRY_SAVE;
		else if (layout_string.compare("booth radix-2") == 0) 	layout = LAYOUT::BOOTH_RADIX_2;
		else if (layout_string.compare("booth radix-4") == 0)	layout = LAYOUT::BOOTH_RADIX_4;
		else {
			Error("Property \"layout\" for multiplier \"" + name + "\" has an unsupported value. "
				  + "Supported values are \"carry propagate\", \"carry save\", \"booth radix-2\", and \"booth radix-4\".\n");
		}
	} else {
		Error("Property \"layout\" for multiplier \"" + name + "\" is required, but was not found.\n");
	}

	if (multiplier["type"]) {
		const auto &type_string = multiplier["type"].as<string>();

		if (type_string.compare("inversion") == 0)				type = TYPE::INVERSION;
		else if (type_string.compare("sign extension") == 0)	type = TYPE::SIGN_EXTEND;
		else if (type_string.compare("Baugh-Wooley") == 0)		type = TYPE::BAUGH_WOOLEY;
		else if (type_string.compare("none") == 0)				type = TYPE::NONE;
		else {
			Error("Property \"type\" for multiplier \"" + name + "\" has an unsupported value. "
				 + "Supported values are \"inversion\", \"sign extension\", and \"Baugh-Wooley\".\n");
		}
	}

	switch (format) {
	case NUMFMT::TWOS_COMPLEMENT:
		switch (layout) {
		case LAYOUT::CARRY_PROPAGATE:
			switch (type) {
			case TYPE::INVERSION:    comps[name] = make_shared<Multiplier_2C>(name, num_bits_A, num_bits_B, Multiplier_2C::MUL_TYPE::CARRY_PROPAGATE_INVERSION); break;
			case TYPE::SIGN_EXTEND:	 comps[name] = make_shared<Multiplier_2C>(name, num_bits_A, num_bits_B, Multiplier_2C::MUL_TYPE::CARRY_PROPAGATE_SIGN_EXTEND); break;
			case TYPE::BAUGH_WOOLEY: comps[name] = make_shared<Multiplier_2C>(name, num_bits_A, num_bits_B, Multiplier_2C::MUL_TYPE::CARRY_PROPAGATE_BAUGH_WOOLEY); break;
			case TYPE::NONE:
				Error("Twos-complement multiplier \"" + name + "\" cannot have \"none\" as a type.\n");
			default:
				error_unsupported_configuration();
				break;
			}
			break;
		case LAYOUT::CARRY_SAVE:
			switch (type) {
			case TYPE::INVERSION:     comps[name] = make_shared<Multiplier_2C>(name, num_bits_A, num_bits_B, Multiplier_2C::MUL_TYPE::CARRY_SAVE_INVERSION); break;
			case TYPE::SIGN_EXTEND:   comps[name] = make_shared<Multiplier_2C>(name, num_bits_A, num_bits_B, Multiplier_2C::MUL_TYPE::CARRY_SAVE_SIGN_EXTEND); break;
			case TYPE::BAUGH_WOOLEY:  comps[name] = make_shared<Multiplier_2C>(name, num_bits_A, num_bits_B, Multiplier_2C::MUL_TYPE::CARRY_SAVE_BAUGH_WOOLEY); break;
			case TYPE::NONE:
				Error("Twos-complement multiplier \"" + name + "\" cannot have \"none\" as a type.\n");
			default:
				error_unsupported_configuration();
				break;
			}
			break;
		case LAYOUT::BOOTH_RADIX_4: comps[name] = make_shared<Multiplier_2C_Booth>(name, num_bits_A, num_bits_B); break;
		default:
			error_unsupported_configuration();
			break;
		}
		break;
	case NUMFMT::SIGNED_MAGNITUDE:
		switch (layout) {
		case LAYOUT::CARRY_PROPAGATE: comps[name] = make_shared<Multiplier_Smag>(name, num_bits_A, num_bits_B, Multiplier_Smag::MUL_TYPE::CARRY_PROPAGATE); break;
		case LAYOUT::CARRY_SAVE:      comps[name] = make_shared<Multiplier_Smag>(name, num_bits_A, num_bits_B, Multiplier_Smag::MUL_TYPE::CARRY_SAVE); break;
			break;
		default:
			error_unsupported_configuration();
			break;
		}
		break;
	default:
		error_unsupported_configuration();
		break;
	}
}

void ParseComponents(comp_map_t &comps, const YAML::Node &config) {
	const auto &components = config["components"];

	if (components.size() == 0) {
		Error("No components found.\n");
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
					Error("Component \"" + comp_name + "\" can have at most two arguments.\n");
				}
			} else {
				Error("Component name must be a scalar or sequence.\n");
			}
		} else {
			Error("Component must have a name.\n");
		}

		if (comp_type.compare("FullAdder") == 0)                		comps[comp_name] = make_shared<FullAdder>(comp_name);
		else if (comp_type.compare("HalfAdder") == 0)           		comps[comp_name] = make_shared<HalfAdder>(comp_name);
		else if (comp_type.compare("And") == 0)                 		comps[comp_name] = make_shared<And>(comp_name);
		else if (comp_type.compare("Or") == 0)                  		comps[comp_name] = make_shared<Or>(comp_name);
		else if (comp_type.compare("Xor") == 0)                 		comps[comp_name] = make_shared<Xor>(comp_name);
		else if (comp_type.compare("Nand") == 0)                		comps[comp_name] = make_shared<Nand>(comp_name);
		else if (comp_type.compare("Nor") == 0)                 		comps[comp_name] = make_shared<Nor>(comp_name);
		else if (comp_type.compare("Xnor") == 0)                		comps[comp_name] = make_shared<Xnor>(comp_name);
		else if (comp_type.compare("Not") == 0)                 		comps[comp_name] = make_shared<Not>(comp_name);
		else if (comp_type.compare("Mux") == 0)                 		comps[comp_name] = make_shared<Mux>(comp_name);
		else if (comp_type.compare("RippleCarryAdder") == 0)    		comps[comp_name] = make_shared<RippleCarryAdder>(comp_name, num_bits_A);
		else if (comp_type.compare("RippleCarryAdderSubtracter") == 0)  comps[comp_name] = make_shared<RippleCarryAdderSubtracter>(comp_name, num_bits_A);
		else if (comp_type.compare("RippleCarrySubtracter") == 0)    	comps[comp_name] = make_shared<RippleCarrySubtracter>(comp_name, num_bits_A);
		else if (comp_type.compare("Multiplier") == 0) 		    		ParseMultiplierComponent(comps, it->second);
		else if (comp_type.compare("Multiplier_2C") == 0)       		comps[comp_name] = make_shared<Multiplier_2C>(comp_name, num_bits_A, num_bits_B);
		else if (comp_type.compare("Multiplier_Smag") == 0)     		comps[comp_name] = make_shared<Multiplier_Smag>(comp_name, num_bits_A, num_bits_B);
		else if (comp_type.compare("Multiplier_2C_Booth") == 0) 		comps[comp_name] = make_shared<Multiplier_2C_Booth>(comp_name, num_bits_A, num_bits_B);
		else if (comp_type.compare("SmagTo2C") == 0)            		comps[comp_name] = make_shared<SmagTo2C>(comp_name, num_bits_A);
		else {
			Error("Component type \"" + comp_type + "\" not recognized.\n");
		}
	}
}

vector<wi_t> ParseWires(comp_map_t &comps, YAML::Node config) {
	const auto &wires = config["wires"];

	vector<wi_t> wire_information;

	if (wires.size() == 0) {
		Error("\"wires\" section is empty.\n");
	}

	for (YAML::const_iterator it = wires.begin(); it != wires.end(); ++it) {
		const auto &wire_string = it->first.as<string>();

		wi_t wire_info = make_shared<WireInformation>();

		string wire_name;
		size_t bundle_size = 0;
		WireBundle::REPR repr = WireBundle::REPR::TWOS_COMPLEMENT;
		ParseWireAndSize(wire_string, wire_name, bundle_size, repr);

		wire_info->name = wire_name;

		if (it->second.size() == 2) {
			wire_t wire;
			wb_t wire_bundle = nullptr;
			size_t size = 1;

			if (bundle_size > 0) {
				wire_bundle = make_shared<WireBundle>(wire_name, bundle_size, repr);
				wire_bundle->Init();
				size = bundle_size;

				wire_info->bundle_size = bundle_size;
				wire_info->is_bundle = true;
				wire_info->wires = wire_bundle;
			}

			auto check_from = [&](const auto &from) {
				// Check for consistency of the "from:" section.
				if (from.IsNull())     Error("No \"from:\" section found for wire \"" + wire_name + "\".\n");
				if (from.IsScalar())   Error("\"from:\" section of wire \"" + wire_name + "\" is a scalar, but it must be a map.\n");
				if (from.IsSequence()) Error("\"from:\" section of wire \"" + wire_name + "\" is a sequence, but it must be a map.\n");
				if (from.IsMap()) {
					auto from_node = from["from"];
					if (!from_node.IsDefined()) {
						Error("No \"from:\" section found for wire \"" + wire_name + "\".\n");
					} else if (from_node.IsNull()) {
						Error("\"from:\" section of wire \"" + wire_name + "\" is empty.\n");
					} else if (!from_node.IsScalar()) {
						Error("\"from:\" section of wire \"" + wire_name + "\" must be a scalar.\n");
					}
				}
			};

			auto check_from_port_node = [&](const auto &from_is_input, const auto &from_port_node) {
				// Check for consistency of the "port:" section if the wire input is not "input".
				if (!from_is_input) {
					if (!from_port_node.IsDefined()) Error("No \"port:\" section found for wire \"" + wire_name + "\".\n");
					if (from_port_node.IsNull())     Error("\"port:\" section of wire \"" + wire_name + "\" is empty.\n");
					if (!from_port_node.IsScalar())  Error("\"port:\" section of wire \"" + wire_name + "\" must be a scalar.\n");
				}
			};

			auto check_to = [&](const auto &to) {
				// Check for consistency of the "to:" section.
				if (to.IsNull())     Error("No \"to:\" section found for wire \"" + wire_name + "\".\n");
				if (to.IsScalar())   Error("\"to:\" section for wire \"" + wire_name + "\" is a scalar, but it must be a map.\n");
				if (to.IsSequence()) Error("\"to:\" section for wire \"" + wire_name + "\" is a sequence, but it must be a map.\n");
				if (to.IsMap()) {
					const auto &to_node = to["to"];
					if (!to_node.IsDefined()) {
						Error("No \"to:\" section found for wire \"" + wire_name + "\".\n");
					} else if (to_node.IsNull()) {
						Error("\"to:\" section is empty for wire \"" + wire_name + "\".\n");
					} else if (to_node.IsSequence() && to_node.begin() == to_node.end()) {
						Error("\"to:\" section contains empty sequence for wire \"" + wire_name + "\".\n");
					} else if (to_node.IsMap() && to_node.begin() == to_node.end()) {
						Error("\"to:\" section contains empty map for wire \"" + wire_name + "\".\n");
					}
				}
			};

			for (size_t b_idx = 0; b_idx < size; ++b_idx) {
				if (bundle_size > 0) {
					wire = (*wire_bundle)[b_idx];
				} else {
					wire = make_shared<Wire>(wire_name);
					wire_info->wire = wire;
				}

				// There is always a single source for a Wire.
				const auto &from = it->second[0];
				check_from(from);

				const auto &from_name = from["from"].as<string>();
				const bool from_is_input = from_name.compare("input") == 0;

				// Check if a component with this name exists.
				if (!from_is_input && !IsComponentDeclared(comps, from_name)) {
					Error("\"from:\" section of wire \"" + wire_name + "\" points to component \"" + from_name + "\" which does not exist.\n");
				}

				const auto &from_port_node = from["port"];
				check_from_port_node(from_is_input, from_port_node);

				// There can be multiple sinks for a Wire.
				const auto &to = it->second[1];
				check_to(to);

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
						Error("Wire \"" + wire_name + "\" is an \"output\" wire, but has a port declared.\n"
							  + "Either remove the \"port:\" key, or connect the port to an existing component.\n");
					}

					if (IsComponentDeclared(comps, comp_name)) {
						to_components.push_back(comps[comp_name]);
						to_port_names.push_back(to_port_node.as<string>());
					} else {
						Error("Wire \"" + wire_name + "\" refers to component \"" + comp_name + "\" which does not exist.\n");
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
								Error("\"to:\" section of wire \"" + wire_name + "\" has multiple \"output\" items configured.\n");
							}
						} else {
							Error("Wire \"" + wire_name + "\" refers to component \"" + comp_name + "\" which does not exist.\n");
						}
					}

					for (YAML::const_iterator it = to_port_node.begin(); it != to_port_node.end(); ++it) {
						to_port_names.push_back((*it).as<string>());
					}

					if (to_components.size() != to_port_names.size()) {
						Error("Wire \"" + wire_name + "\" output component and port sequences must be same length.\n");
					}
				}
				// If only the "to:" key exists.
				else if (!port_node_def && comp_node_def && to_comp_node.IsScalar()) {
					const auto &comp_name = to_comp_node.as<string>();

					// Node value must be "output".
					if (comp_name.compare("output") == 0) {
						output = true;
					} else {
						Error("Wire \"" + wire_name + "\" has only a \"to:\" section without \"port:\".\n"
							  + "In that case, the only valid value for \"to:\" is \"output\", but is now \""
							  + comp_name + "\".\n");
					}
				}
				// Something is configured incorrectly if we are here.
				else {
					if ((comp_node_def && port_node_def) &&
						(to_comp_node.Type() != to_port_node.Type())) {
						Error("Wire \"" + wire_name + "\" output \"to:\" and \"port:\" must both be scalars or sequences.\n");
					}
				}

				// If a Wire or WireBundle is driven by the global input...
				if (from_name.compare("input") == 0) {
					if (to.size() == 2) {
						for (std::size_t i = 0; i < to_components.size(); ++i) {
							string port_name;
							size_t begin_idx = 0;
							size_t end_idx = 0;

							ParsePortAndIndex(wire_name, to_port_names[i], port_name, begin_idx, end_idx);
							wire_info->AddTo(to_components[i]->GetName(), PortNameToPortMap[port_name], begin_idx, end_idx);

							if (b_idx + begin_idx <= end_idx) {
								Connect(to_components[i], PortNameToPortMap[port_name], wire, b_idx + begin_idx);
							}
						}
					} else {
						Error("Input wire \"" + wire_name + "\" has incomplete declared output.\n");
					}
				} else {
					if (from.size() == 2) {
						const auto &from_comp = comps[from_name];
						const auto &from_port = from["port"].as<string>();

						string from_port_name;
						size_t from_begin_idx = 0;
						size_t from_end_idx = 0;

						ParsePortAndIndex(wire_name, from_port, from_port_name, from_begin_idx, from_end_idx);
						wire_info->AddFrom(from_name, PortNameToPortMap[from_port_name], from_begin_idx, from_end_idx);
						if (wire_bundle) {
							wire_bundle->SetFromMinIdx(from_begin_idx);
							wire_bundle->SetFromMaxIdx(from_end_idx);
						}

						if (b_idx + from_begin_idx <= from_end_idx) {
							Connect(from_comp, PortNameToPortMap[from_port_name], wire, b_idx + from_begin_idx);
						}

						if (IsComponentDeclared(comps, from_name)) {
							for (std::size_t i = 0; i < to_components.size(); ++i) {
								if (comps.find(to_components[i]->GetName()) != comps.end()) {
									const auto &to_comp = to_components[i];
									const auto &to_port = to_port_names[i];

									string to_port_name;
									size_t to_begin_idx = 0;
									size_t to_end_idx = 0;

									ParsePortAndIndex(wire_name, to_port, to_port_name, to_begin_idx, to_end_idx);
									wire_info->AddTo(to_comp->GetName(), PortNameToPortMap[to_port_name], to_begin_idx, to_end_idx);

									if (b_idx + to_begin_idx <= to_end_idx) {
										Connect(to_comp, PortNameToPortMap[to_port_name], wire, b_idx + to_begin_idx);
									}
								} else {
									Error("Wire \"" + wire_name + "\" input component does not exist.\n");
								}
							}
						} else {
							Error("Output component of wire \"" + wire_name + "\" does not exist.\n");
						}
					} else {
						Error("Output wire \"" + wire_name + "\" input declaration needs one \"from:\" and one \"to:\" section.\n");
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
			Error("Wire \"" + wire_name + "\" declaration needs exactly 1 \"from\" section and 1 \"to\" section.\n");
		}

		wire_information.emplace_back(wire_info);
	}

	cout << "Wire information:\n";
	for (const auto &wi : wire_information) {
		if (wi->is_bundle) {
			cout << "\nWireBundle: " << wi->name;
		} else {
			cout << "Wire: " << wi->name;
		}

		cout << "\nfrom:\n";
		for (const auto &from : wi->from) {
			cout << "comp: " << get<0>(from) << " port: " << PortToPortNameMap[get<1>(from)] << " bIdx: " << get<2>(from) << " eIdx: " << get<3>(from) << '\n';
		}

		cout << "\nto:\n";
		for (const auto &to : wi->to) {
			cout << "comp: " << get<0>(to) << " port: " << PortToPortNameMap[get<1>(to)] << " bIdx: " << get<2>(to) << " eIdx: " << get<3>(to) << '\n';
		}
	}

	return wire_information;
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
	enum class TYPE {NONE, RNG, UNIFORM, COUNT_UP, COUNT_DOWN, SHIFT_UP, SHIFT_DOWN};

	Constraint(const string _wire_name,
			   const size_t _begin_index,
			   const size_t _end_index,
			   const TYPE _type,
			   const size_t _seed,
			   const float _sigma,
			   const int32_t _ub,
			   const int32_t _lb,
			   const size_t _times)
		: wire_name(_wire_name)
		, begin_index(_begin_index)
		, end_index(_end_index)
		, type(_type)
		, seed(_seed)
		, sigma(_sigma)
		, ub(_ub)
		, lb(_lb)
		, times(_times)
	{
		generator = default_random_engine(seed);
		norm_distr = normal_distribution<float>(0.0f, sigma);
		int_gen = uniform_int_distribution<int32_t>(lb, ub);
		bit_gen = uniform_int_distribution<uint8_t>(0, 1);
	}

	float GenerateRandomNorm() {
		return norm_distr(generator);
	}

	int32_t GenerateUniformInteger() {
		return int_gen(generator);
	}

	bool GenerateRandomBit() {
		const auto rnd_val = bit_gen(generator);
		return (rnd_val < 0.5f);
	}

	string wire_name;
	size_t begin_index;
	size_t end_index;
	TYPE type;
	size_t seed;
	float  sigma;
	int32_t ub;
	int32_t lb;
	default_random_engine generator;
	normal_distribution<float> norm_distr;
	uniform_int_distribution<int32_t> int_gen;
	uniform_int_distribution<uint8_t> bit_gen;
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
	int32_t ub = 1;
	int32_t lb = -1;

	if (node["wire"]) {
		wire_name = node["wire"].as<string>();
	} else {
	    Error("A constraint needs at least a \"wire\", but none was given.\n");
	}

	if (node["begin_index"]) {
		try {
			beg_idx = node["begin_index"].as<size_t>();
		} catch (YAML::TypedBadConversion<size_t> e) {
			Error("\"begin_index\" is not a number: " + e.msg + '\n');
		}
	}

	if (node["end_index"]) {
		try {
			end_idx = node["end_index"].as<size_t>();
		} catch (YAML::TypedBadConversion<size_t> e) {
			Error("\"end_index\" is not a number: " + e.msg + '\n');
		}
	}

	if (node["type"]) {
		const auto &type_string = node["type"].as<string>();

		if (type_string.compare("rng") == 0) {
			type = Constraint::TYPE::RNG;
		} else if (type_string.compare("uniform") == 0) {
			type = Constraint::TYPE::UNIFORM;
		} else if (type_string.compare("count up") == 0) {
			type = Constraint::TYPE::COUNT_UP;
		} else if (type_string.compare("count down") == 0) {
			type = Constraint::TYPE::COUNT_DOWN;
		} else if (type_string.compare("shift up") == 0) {
			type = Constraint::TYPE::SHIFT_UP;
		} else if (type_string.compare("shift down") == 0) {
			type = Constraint::TYPE::SHIFT_DOWN;
		} else {
			type = Constraint::TYPE::NONE;
		}
	} else {
		Error("A constraint needs at least a \"type\", but none was given.\n");
	}

	if (node["seed"]) {
		if ((type != Constraint::TYPE::RNG) && (type != Constraint::TYPE::UNIFORM)) {
			cout << "[Warning] Constraint type is not \"rng\" or \"uniform\", so \"seed\" is ignored.\n";
		} else {
			try {
				seed = node["seed"].as<size_t>();
			} catch (YAML::TypedBadConversion<size_t> e) {
				Error("\"seed\" is not a number: " + e.msg + '\n');
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
					Error("\"sigma\" must be larger than 0.0.\n");
				}
			} catch (YAML::TypedBadConversion<float> e) {
				Error("\"sigma\" is not a number: " + e.msg + '\n');
			}
		}
	}

	if (node["ub"]) {
		if (type != Constraint::TYPE::UNIFORM) {
			cout << "[Warning] Constraint type is not \"uniform\", so \"ub\" is ignored.\n";
		} else {
			try {
				ub = node["ub"].as<int32_t>();
			} catch (YAML::TypedBadConversion<size_t> e) {
				Error("\"ub\" is not a number: " + e.msg + '\n');
			}
		}
	}

	if (node["lb"]) {
		if (type != Constraint::TYPE::UNIFORM) {
			cout << "[Warning] Constraint type is not \"uniform\", so \"lb\" is ignored.\n";
		} else {
			try {
				lb = node["lb"].as<int32_t>();
				if (lb > ub) {
					Error("\"lb\" is larger than \"ub\".\n");
				}
			} catch (YAML::TypedBadConversion<size_t> e) {
				Error("\"lb\" is not a number: " + e.msg + '\n');
			}
		}
	}

	if (node["times"]) {
		try {
			times = node["times"].as<size_t>();
		} catch (YAML::TypedBadConversion<size_t> e) {
			Error("\"times\" is not a number: " + e.msg + '\n');
		}
	}

	return make_shared<Constraint>(wire_name, beg_idx, end_idx, type, seed, sigma, ub, lb, times);
}

void ParseStimuli(System &system, YAML::Node config, const string &config_file_name, bool print_debug) {
	const auto &stimuli = config["stimuli"];

	auto error_invalid_value = [](const auto &val) {
		Error("Value \"" + val + "\" in stimuli section "
			  + "is invalid. It should begin with either '0b'/'0B', '0x'/'0X', "
			  + "or '0d'/'0D' for binary, hexadecimal, and decimal "
			  + "representations respectively, then followed by a value.\n");
	};

	auto error_constraint_map = []() {
		Error(string("A \"constraint\" needs to be either a map, or a sequence ")
			  + " of maps. In either case it needs to have at least the "
			  + "\"wire\", \"begin_index\", and \"type\" keys.\n");
	};

	auto error_non_existent_wire = [](const auto &name) {
		Error("No wire or wire bundle \"" + name + "\" found.\n");
	};

	// Helper struct used for storing input and output stimuli.
	struct io_bundle {
		io_bundle(const wire_t &_wire)
			: wire(_wire),
			  wb(nullptr)
			{};
		io_bundle(const wb_t &_wb)
			: wire(nullptr),
			  wb(_wb)
			{};

	    vector<int64_t> values;
		vector<int64_t> values_2C;
	    wire_t wire;
	    wb_t wb;
	};

	ordered_map<string, shared_ptr<io_bundle>> in_values;
	ordered_map<string, shared_ptr<io_bundle>> out_values;

	// Create enough space for wire bundles.
	for (const auto &[name, bundle] : system.GetWireBundles()) {
	    if (bundle->IsInputBundle()) {
			in_values[name] = make_shared<io_bundle>(bundle);
		} else if (bundle->IsOutputBundle()) {
			out_values[name] = make_shared<io_bundle>(bundle);
		}
	}

	// Do the same for input wires.
	for (const auto &iw : system.GetInputWires()) {
		in_values[iw->GetName()] = make_shared<io_bundle>(iw);
	}

	// Do the same for output wires.
	for (const auto &ow : system.GetOutputWires()) {
		out_values[ow->GetName()] = make_shared<io_bundle>(ow);
	}

	vector<size_t> toggles = {};
	vector<float> sigmas = {};

	auto process_wire_rng = [&](const auto &wire, const auto &constraint, auto &system, const size_t num_times) {
		for (size_t i = 0; i < num_times; ++i) {
			const bool rnd_val = constraint->GenerateRandomBit() < 0.0f;
			const int64_t val = rnd_val ? 1 : 0;

			wire->SetValue(rnd_val, false);
			in_values[wire->GetName()]->values.emplace_back(val);
		}
	};

	auto process_wire_bundle_rng = [&](const auto &wb, const auto &constraint, auto &system, const size_t num_times) {
		const float scale_factor = (float)((1l << wb->GetSize()) - 1);
		for (size_t i = 0; i < num_times; ++i) {
			auto rnd_val = constraint->GenerateRandomNorm() * scale_factor;
			if (rnd_val > scale_factor) {
				rnd_val = scale_factor;
			}

			wb->SetValue((int64_t)rnd_val, false);
			const auto &map_val = in_values[wb->GetName()];
			map_val->values.emplace_back(wb->GetValue());
			map_val->values_2C.emplace_back(wb->Get2CValue());
		}
	};

	auto process_wire_bundle_uniform = [&](const auto &wb, const auto &constraint, auto &system, const size_t num_times) {
		for (size_t i = 0; i < num_times; ++i) {
			auto rnd_val = constraint->GenerateUniformInteger();
			wb->SetValue((int64_t)rnd_val, false);

			const auto &map_val = in_values[wb->GetName()];
			map_val->values.emplace_back(wb->GetValue());
			map_val->values_2C.emplace_back(wb->Get2CValue());
		}
	};

	auto process_wire_bundle_count = [&](const auto &wb, const auto &constraint, auto &system, const size_t num_times, DIRECTION dir) {
		if (dir == DIRECTION::UP) {
			for (size_t i = 0; i < num_times; ++i) {
				const auto curr_val = wb->GetValue();
				const auto new_val = curr_val + 1;
				wb->SetValue(new_val, false);

				const auto &map_val = in_values[wb->GetName()];
				map_val->values.emplace_back(wb->GetValue());
				map_val->values_2C.emplace_back(wb->Get2CValue());
			}
		} else {
			for (size_t i = 0; i < num_times; ++i) {
				const auto curr_val = wb->GetValue();
				const auto new_val = curr_val - 1;
				wb->SetValue(new_val, false);

				const auto &map_val = in_values[wb->GetName()];
				map_val->values.emplace_back(wb->GetValue());
				map_val->values_2C.emplace_back(wb->Get2CValue());
			}
		}
	};

	auto process_wire_bundle_shift = [&](const auto &wb, const auto &constraint, auto &system, const size_t num_times, DIRECTION dir) {
		if (dir == DIRECTION::UP) {
			for (size_t i = 0; i < num_times; ++i) {
				const auto curr_val = wb->GetValue();
				const auto new_val = curr_val << 1;
				wb->SetValue(new_val, false);

				const auto &map_val = in_values[wb->GetName()];
				map_val->values.emplace_back(wb->GetValue());
				map_val->values_2C.emplace_back(wb->Get2CValue());
			}
		} else {
			for (size_t i = 0; i < num_times; ++i) {
				const auto curr_val = wb->GetValue();
				const auto new_val = curr_val >> 1;
				wb->SetValue(new_val, false);

				const auto &map_val = in_values[wb->GetName()];
				map_val->values.emplace_back(wb->GetValue());
				map_val->values_2C.emplace_back(wb->Get2CValue());
			}
		}
	};

	size_t prev_toggles = 0;

	vector<constr_t> constraints;

	for (size_t i = 0; i < stimuli.size(); ++i) {
		if (print_debug) {
			cout << "\nStimulus " << i << '\n';
		}
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
									case Constraint::TYPE::RNG:
									case Constraint::TYPE::UNIFORM: {
										process_wire_rng(w, c, system, 1);
										break;
									}
									case Constraint::TYPE::NONE: break;
									default: break;
									}
								} else if (wb) {
									switch (c->type) {
									case Constraint::TYPE::RNG: {
										process_wire_bundle_rng(wb, c, system, 1);
										break;
									}
									case Constraint::TYPE::UNIFORM: {
										process_wire_bundle_uniform(wb, c, system, 1);
										break;
									}
									case Constraint::TYPE::SHIFT_UP: {
										process_wire_bundle_shift(wb, c, system, 1, DIRECTION::UP);
										break;
									}
									case Constraint::TYPE::SHIFT_DOWN: {
										process_wire_bundle_shift(wb, c, system, 1, DIRECTION::DOWN);
										break;
									}
									case Constraint::TYPE::COUNT_UP: {
										process_wire_bundle_count(wb, c, system, 1, DIRECTION::UP);
										break;
									}
									case Constraint::TYPE::COUNT_DOWN: {
										process_wire_bundle_count(wb, c, system, 1, DIRECTION::DOWN);
										break;
									}
									case Constraint::TYPE::NONE: break;
									default: break;
									}
								}
							}
						}

						system.Update();
						for (const auto &o : system.GetOutputWireBundles()) {
							out_values[o->GetName()]->values.emplace_back(o->GetValue());
							out_values[o->GetName()]->values_2C.emplace_back(o->Get2CValue());
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
						case Constraint::TYPE::RNG:
						case Constraint::TYPE::UNIFORM: {
							process_wire_rng(w, c, system, c->times);
							system.Update();
							break;
						}
						case Constraint::TYPE::NONE: break;
						default: break;
						}
					} else if (wb) {
						switch (c->type) {
						case Constraint::TYPE::RNG: {
							process_wire_bundle_rng(wb, c, system, c->times);
							system.Update();
							break;
						}
						case Constraint::TYPE::UNIFORM: {
							process_wire_bundle_uniform(wb, c, system, c->times);
							break;
						}
						case Constraint::TYPE::SHIFT_UP: {
							process_wire_bundle_shift(wb, c, system, c->times, DIRECTION::UP);
							break;
						}
						case Constraint::TYPE::SHIFT_DOWN: {
							process_wire_bundle_shift(wb, c, system, c->times, DIRECTION::DOWN);
							break;
						}
						case Constraint::TYPE::COUNT_UP: {
							process_wire_bundle_count(wb, c, system, c->times, DIRECTION::UP);
							break;
						}
						case Constraint::TYPE::COUNT_DOWN: {
							process_wire_bundle_count(wb, c, system, c->times, DIRECTION::DOWN);
							break;
						}
						case Constraint::TYPE::NONE: break;
						default: break;
						}
					} else {
						error_non_existent_wire(c->wire_name);
					}
				} else {
					error_constraint_map();
				}

				//continue;
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
						Error("Stimulus value of wire \"" + key_name
							 +	"\" has to be one of the following: 0, 1, true, false.\n");
					}
					wire->SetValue(value, false);

					const auto &io_val = in_values[wire->GetName()];
					io_val->values.emplace_back((int64_t)value);
					if (wb) {
						io_val->values_2C.emplace_back(wb->Get2CValue());
					}
					if (io_val->wire == nullptr) {
						io_val->wire = wire;
					}

					if (print_debug) {
						// Print the wire name and value.
						cout << key_name << ": " << value << '\n';
					}
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

						const auto &io_val = in_values[wb->GetName()];
						io_val->values.emplace_back((int64_t)value);
						io_val->values_2C.emplace_back(wb->Get2CValue());
						if (io_val->wb == nullptr) {
							io_val->wb = wb;
						}

						if (print_debug) {
							// Print the bundle name and value in hex and binary.
							cout << wb->GetName() << ": "
								 << wb->Get2CValue() << " "
								 << ValueToHexString(wb->GetValue()) << " "
								 << ValueToBinaryString(wb->GetValue(), wb->GetSize()) << '\n';
						}
					} catch (invalid_argument e) {
						error_invalid_value(value_string);
					} catch (out_of_range e) {
						Error("Value \"" + value_string + "\" is too large.\n");
					}
				} else {
					Error(string("Non-existent wire or wire bundle \"") + key_name + "\" found in stimuli section.\n");
				}
			}
		}

		system.Update();
		for (const auto &o : system.GetOutputWireBundles()) {
			out_values[o->GetName()]->values.emplace_back(o->GetValue());
			out_values[o->GetName()]->values_2C.emplace_back(o->Get2CValue());
		}

		const size_t curr_toggles = system.GetNumToggles();
		if (print_debug) {
			cout << "#toggles: " << (curr_toggles - prev_toggles) << "\n";
		}
		prev_toggles = curr_toggles;
	}

	// Output file that has the values given to the inputs,
	// is produced on the outputs, and how many toggles each input caused.
	auto outfile_name = config_file_name.substr(0, config_file_name.find_last_of("."));
	auto outfile = ofstream(outfile_name + ".txt");
	size_t i = 0;

	// Write all input values.
	for (const auto &[name, bundle] : in_values) {
		if (bundle->wb) {
			outfile << name << ' ' << bundle->wb->GetSize();
		}

		if (sigmas.size()) {
			outfile << ' ' << sigmas[i++] << '\n';
		} else {
			outfile << '\n';
		}

		for (const auto &val : bundle->values_2C) {
			outfile << val << ',';
		}
		outfile << '\n';
	}

	// Write all output values.
	for (const auto &[name, bundle] : out_values) {
		if (bundle->wb) {
			outfile << name << ' ' << bundle->wb->GetSize() << '\n';
		}

		for (const auto &val : bundle->values_2C) {
			outfile << val << ',';
		}
		outfile << '\n';
	}

	outfile << "toggles\n";
	for (const auto &val : toggles) {
		outfile << val << ',';
	}
	outfile.close();

	// Write a stimulus file for the testbench.
	size_t max_iterations = 0;
	for (const auto &[name, bundle] : in_values) {
		const auto &vals = bundle->values;
		if (vals.size() > max_iterations) {
			max_iterations = vals.size();
		}
	}

	size_t found = config_file_name.find_last_of(".");
	string stimfile_path = "";
	if (found != string::npos) {
		stimfile_path = config_file_name.substr(0, config_file_name.find_last_of("."));
	}

	auto stim_file = ofstream(stimfile_path + "/stim_file.txt");
	for (size_t i = 0; i < max_iterations; ++i) {
		for (const auto &[name, bundle] : in_values) {
			if (bundle->wb) {
				const auto &vals = bundle->values;
				if (i < vals.size()) {
					const auto &val = vals[i];
					for (int j = bundle->wb->GetSize() - 1; j >= 0; --j) {
						stim_file << ((val >> j) & 1);
					}
					stim_file << ' ';
				} else {
					for (size_t j = 0; j < bundle->wb->GetSize(); ++j) {
						stim_file << '0';
					}
					stim_file << ' ';
				}
			}
		}
		stim_file << '\n';
	}
	stim_file.close();

	auto expected_output = ofstream(stimfile_path + "/expected_output.txt");
	for (size_t i = 0; i < max_iterations; ++i) {
		for (const auto &[name, ob] : out_values) {
			if (ob->wb) {
				const auto &vals = ob->values;
				if (i < vals.size()) {
					const auto &val = vals[i];
					for (int j = ob->wb->GetSize() - 1; j >= 0; --j) {
						expected_output << ((val >> j) & 1);
					}
					expected_output << ' ';
				} else {
					for (size_t j = 0; j < ob->wb->GetSize(); ++j) {
						expected_output << '0';
					}
					expected_output << ' ';
				}
			}
		}
		expected_output << '\n';
	}
	expected_output.close();
}

YAML::Node LoadConfigurationFile(const string &config_file_name) {
	YAML::Node config;

	try {
		config = YAML::LoadFile(config_file_name.c_str());
	} catch (YAML::BadFile e) {
		Error(string("Could not load file \"") + config_file_name.c_str() + "\": " + e.msg + '\n');
	} catch (YAML::ParserException e) {
		Error(string("Could not parse file \"") + config_file_name.c_str() + "\": " + e.msg + '\n');
	}

	return config;
}

int main(int argc, char **argv) {
	comp_map_t comps;
	YAML::Node config;
	System system;
	string config_file_name;
	bool generate_vhdl = false;

	auto error_usage = []() {
		cout << "Usage: ./bitflipsim [--vhdl] <configuration file>\n";
		exit(0);
	};

	// Check if a configuration file was supplied.
	if (argc == 2) {
		config_file_name = string(argv[1]);
	} else if (argc == 3) {
		string cmdline_option(argv[1]);

		if (cmdline_option.compare("--vhdl") == 0) {
			config_file_name = string(argv[2]);
			generate_vhdl = true;
		} else {
			error_usage();
		}
	} else {
		error_usage();
	}

	config = LoadConfigurationFile(config_file_name);
	string output_file_path = config_file_name.substr(0, config_file_name.find_last_of(".")) + '/';

	if (!config.IsNull()) {
		// First check for obvious errors.
		const auto &components = config["components"];
		const auto &wires = config["wires"];
		const auto &stimuli = config["stimuli"];

		if (!components) {
			Error("No \"components\" section found in \"" + config_file_name + "\"\n");
		}
		if (components.size() == 0) {
			Error("\"components\" section in \"" + config_file_name + "\" is empty.\n");
		}
		if (!wires) {
			Error("No \"wires\" section found in \"" + config_file_name + "\"\n");
		}
		if (wires.size() == 0) {
			Error("\"wires\" section in \"" + config_file_name + "\" is empty.\n");
		}
		if (!stimuli) {
			Error("No \"stimuli\" section found in \"" + config_file_name + "\"\n");
		}
		if (stimuli.size() == 0) {
			Error("\"stimuli\" section in \"" + config_file_name + "\" is empty.\n");
		}

		ParseComponents(comps, config);
		vector<wi_t> wire_information = ParseWires(comps, config);
		system.SetWireInformation(wire_information);

		for (const auto &c : comps) {
			system.AddComponent(c.second);
		}

		for (const auto &wi : wire_information) {
			if (wi->is_bundle && wi->wires) {
				system.AddWireBundle(wi->wires);
			} else if (wi->wire) {
				system.AddWire(wi->wire);
			}
		}

		system.FindLongestPathInSystem();
		system.FindInitialState();

		if (generate_vhdl) {
			// Recursively create the folders of the path.
			experimental::filesystem::create_directory(output_file_path);
		}

		cout << "Longest path in the system: " << system.GetLongestPath() << '\n';
		cout << "Number of components: " << system.GetNumComponents() <<
			"\nNumber of wires: " << system.GetNumWires() << '\n';

		ParseStimuli(system, config, config_file_name, false);

		cout << "\nSimulation done!\n";
		cout << "Number of toggles: " << system.GetNumToggles() << '\n';

#if 0
		cout << "\nValue of all wires:\n";
		for (const auto &[ow_name, ow] : system.GetWires()) {
			cout << "Wire \"" << ow_name
				 << "\": " << ow->GetValue()
				 << "\t#outputs: " << ow->GetNumOutputs()
				 << "\t#toggles: " << ow->GetNumToggles()
				 << '\n';
			cout << "Outputs:\n";
			for (const auto &c : ow->GetComponentOutputs()) {
				cout << '\t' << c.lock()->GetName() << '\n';
			}
			for (const auto &w : ow->GetWireOutputs()) {
				cout << '\t' << w.lock()->GetName() << '\n';
			}
		}
#endif
#if 0
		cout << "\nInputs:\n";
		vector<wb_t> processed_wire_bundles;

		for (const auto &iw : system.GetAllInputWires()) {
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

		for (const auto &ow : system.GetAllOutputWires()) {
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
#endif
	}

	if (generate_vhdl) {
		size_t found = config_file_name.find_last_of("/\\");
		string file_name = config_file_name.substr(found + 1);
		string top_level_name;
		found = file_name.find_last_of(".");

		if (found != string::npos) {
			top_level_name = file_name.substr(0, found);
		} else {
			top_level_name = file_name;
		}

		system.GenerateVHDL("top_" + top_level_name, output_file_path);
	}

// TEST
/*	wire_t a, b, c, d, e;
	a = make_shared<Wire>("A");
	b = a;
	c = a;
	d = a;
	e = a;

	cout << a.use_count() << ' ' << a->GetName() << ' ' << e->GetName() << '\n';
	wire_t f = make_shared<Wire>("F");
	*a = *f;
	f.reset();
	cout << a.use_count() << ' ' << a->GetName() << ' ' << e->GetName() << '\n';*/

	return 0;
}
