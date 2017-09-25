#ifndef XOR_H
#define XOR_H

#include "main.h"

class Wire;

class Xor : public Component {
public:
	Xor(std::string _name)
		: Component(_name) {}
	~Xor() = default;

	enum class PORTS {A, B, O};

	void Update() override;
	void Connect(PORTS port, wire_t wire);

	std::vector<wire_t> GetWires() override;

private:
	wire_t A;
	wire_t B;
	wire_t O;
};

#endif // XOR_H
