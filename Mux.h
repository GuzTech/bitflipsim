#ifndef MUX_H
#define MUX_H

#include "main.h"

class Wire;

class Mux : public Component {
public:
	Mux(std::string _name)
		: Component(_name) {}
	~Mux() = default;

	enum class PORTS {A, B, S, O};

	void Update() override;
	void Connect(PORTS port, wire_t wire);

	std::vector<wire_t> GetWires() override;

private:
	wire_t A;
	wire_t B;
	wire_t S;
	wire_t O;
};

#endif // MUX_H
