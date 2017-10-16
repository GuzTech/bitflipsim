#ifndef NOT_H
#define NOT_H

#include "main.h"

class Not : public Component {
public:
	Not(string _name)
		: Component(_name) {
		needs_update = true;
	}
	~Not() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, wire_t wire, size_t index = 0) override;

	vector<wire_t> GetWires() override;
	vector<wire_t> GetInputWires() override;
	vector<wire_t> GetOutputWires() override;
	wire_t GetWire(PORTS port, size_t index = 0) override;

private:
	wire_t I;
	wire_t O;
};

#endif // NOT_H
