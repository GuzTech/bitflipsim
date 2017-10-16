#ifndef XNOR_H
#define XNOR_H

#include "main.h"

class Xnor : public Component {
public:
	Xnor(string _name)
		: Component(_name) {}
	~Xnor() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, wire_t wire, size_t index = 0) override;

	vector<wire_t> GetWires() override;
	vector<wire_t> GetInputWires() override;
	vector<wire_t> GetOutputWires() override;

private:
	wire_t A;
	wire_t B;
	wire_t O;
};

#endif // XNOR_H
