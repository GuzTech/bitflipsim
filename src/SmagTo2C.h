#ifndef SMAGTO2C_H
#define SMAGTO2C_H

#include "main.h"

class SmagTo2C : public Component {
public:
	SmagTo2C(string _name, size_t _num_bits);
	~SmagTo2C() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, const wire_t &wire, size_t index = 0) override;
	void Connect(PORTS port, const wb_t &wires, size_t port_idx = 0, size_t wire_idx = 0) override;

	const wire_t GetWire(PORTS port, size_t index = 0) const override;
	const PORT_DIR GetPortDirection(PORTS port) const override;

	void GenerateVHDLEntity(const string &path) const override;
	const string GenerateVHDLInstance() const override;

private:
	size_t num_bits = 0;

	vector<xor_t> xors;
    vector<ha_t>  adders;

	static bool entityGenerated; // Used for generating HDL
};

#endif // SMAGTO2C_H
