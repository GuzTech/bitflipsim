#ifndef NOT_H
#define NOT_H

#include "main.h"

class Wire;

class Not : public Component {
public:
	Not(std::string _name)
		: Component(_name) {}
	~Not() = default;

	enum class PORTS {I, O};

	void Update() override;
	void Connect(PORTS port, wire_t wire);

	std::vector<wire_t> GetWires() override;

private:
	wire_t I;
	wire_t O;
};

#endif // NOT_H
