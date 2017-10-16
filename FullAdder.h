#ifndef FULLADDER_H
#define FULLADDER_H

#include "main.h"

class FullAdder : public Component {
public:
	FullAdder(string _name)
		: Component(_name) {}
	~FullAdder() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, wire_t wire, size_t index = 0) override;

	vector<wire_t> GetWires() override;
	vector<wire_t> GetInputWires() override;
	vector<wire_t> GetOutputWires() override;
	wire_t GetWire(PORTS port, size_t index = 0) override;

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
