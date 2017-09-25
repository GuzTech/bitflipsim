#ifndef AND_H
#define AND_H

#include "main.h"

class Wire;

class And : public Component {
public:
	And(std::string _name)
		: Component(_name) {}
	~And() = default;

	enum class PORTS {A, B, O};

	void Update() override;
	void Connect(PORTS port, wire_t wire);

	std::vector<wire_t> GetWires() override;

private:
	wire_t A;
	wire_t B;
	wire_t O;
};

#endif // AND_H
