#ifndef NOT_H
#define NOT_H

#include "main.h"

class Not : public Component {
public:
	Not(std::string _name)
		: Component(_name) {
		needs_update = true;
	}
	~Not() = default;

	enum class PORTS {I, O};

	void Update(bool propagating) override;
	void Connect(PORTS port, wire_t wire);

	std::vector<wire_t> GetWires() override;
	std::vector<wire_t> GetInputWires() override;
	std::vector<wire_t> GetOutputWires() override;

private:
	wire_t I;
	wire_t O;
};

#endif // NOT_H
