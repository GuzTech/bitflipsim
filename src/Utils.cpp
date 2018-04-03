#include "main.h"

void Error(const string &err) {
	cout << "[Error] " << err;
	exit(1);
}

map<string, PORTS> PortNameToPortMap = {{"A",              PORTS::A},
										{"B",              PORTS::B},
										{"C",              PORTS::C},
										{"Cin",            PORTS::Cin},
										{"Cout",           PORTS::Cout},
										{"I",              PORTS::I},
										{"O",              PORTS::O},
										{"S",              PORTS::S},
										{"X_2I",           PORTS::X_2I},
										{"X_2I_MINUS_ONE", PORTS::X_2I_MINUS_ONE},
										{"X_2I_PLUS_ONE",  PORTS::X_2I_PLUS_ONE},
										{"Y_LSB",          PORTS::Y_LSB},
										{"Y_MSB",          PORTS::Y_MSB},
										{"NEG",            PORTS::NEG},
										{"SE",             PORTS::SE},
										{"ROW_LSB",        PORTS::ROW_LSB},
										{"X1_b",           PORTS::X1_b},
										{"X2_b",           PORTS::X2_b},
										{"Z",              PORTS::Z},
										{"Yj",             PORTS::Yj},
										{"Yj_m1",          PORTS::Yj_m1},
										{"PPTj",           PORTS::PPTj},
										{"NEG_CIN",        PORTS::NEG_CIN}};

map<PORTS, string> PortToPortNameMap = {{PORTS::A,              "A"},
										{PORTS::B,              "B"},
										{PORTS::C,              "C"},
										{PORTS::Cin,            "Cin"},
										{PORTS::Cout,           "Cout"},
										{PORTS::I,              "I"},
										{PORTS::O,              "O"},
										{PORTS::S,              "S"},
										{PORTS::X_2I,           "X_2I"},
										{PORTS::X_2I_MINUS_ONE, "X_2I_MINUS_ONE"},
										{PORTS::X_2I_PLUS_ONE,  "X_2I_PLUS_ONE"},
										{PORTS::Y_LSB,          "Y_LSB"},
										{PORTS::Y_MSB,          "Y_MSB"},
										{PORTS::NEG,            "NEG"},
										{PORTS::SE,             "SE"},
										{PORTS::ROW_LSB,        "ROW_LSB"},
										{PORTS::X1_b,           "X1_b"},
										{PORTS::X2_b,           "X2_b"},
										{PORTS::Z,              "Z"},
										{PORTS::Yj,             "Yj"},
										{PORTS::Yj_m1,          "Yj_m1"},
										{PORTS::PPTj,           "PPTj"},
										{PORTS::NEG_CIN,        "NEG_CIN"}};
