#ifndef OR_H
#define OR_H

#include "main.h"

class Or : public Component {
public:
	Or(string _name)
		: Component(_name) {}
	~Or() = default;

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

#endif // OR_H
