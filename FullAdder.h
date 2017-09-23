#ifndef FULLADDER_H
#define FULLADDER_H

#include "main.h"

class Wire;

class FullAdder : public Component {
public:
	FullAdder() = default;
	FullAdder(std::string _name)
		: Component(_name) {}
	~FullAdder() = default;

	enum class PORTS {A, B, Cin, S, Cout};

	void Update() override;
	void Connect(PORTS port, wire_t wire);

	std::vector<wire_t> GetWires() override;

private:
	wire_t A;
	wire_t B;
	wire_t Cin;
	wire_t S;
	wire_t Cout;

	// Internal wires
	bool iw_1_curr = false;
	bool iw_2_curr = false;
	bool iw_3_curr = false;
	bool iw_1_prev = false;
	bool iw_2_prev = false;
	bool iw_3_prev = false;
};

#endif // FULLADDER_H
