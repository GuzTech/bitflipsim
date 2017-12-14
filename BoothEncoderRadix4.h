#ifndef BOOTH_ENCODER_RADIX_4
#define BOOTH_ENCODER_RADIX_4

#include "main.h"

class BoothEncoderRadix4 : public Component {
public:
	BoothEncoderRadix4(string _name);
	~BoothEncoderRadix4() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, const wire_t &wire, size_t index = 0) override;
	void Connect(PORTS port, const wb_t &wires, size_t port_idx = 0, size_t wire_idx = 0) override;

	const size_t GetNumToggles() final;
	const wire_t GetWire(PORTS port, size_t index) const override;

private:
	xor_t X2_b;		// x2 / x-2
	xnor_t X1_b;	// x1 / x-1
	xnor_t Z;		// Zero
	xnor_t SE;		// Sign-extension
	and_t Row_LSB;	// Value of LSB of partial product
	nor_t Neg_cin_nor_1;
	nor_t Neg_cin_nor_2;
	nor_t Neg_cin_nor_3;
	or3_t Neg_cin_or3;
	and_t Neg_cin_and;

	wire_t nor_1_o;
	wire_t nor_2_o;
	wire_t nor_3_o;
	wire_t or3_o;

	wire_t x2_neg;	// Wire that connects X_2i+1 to Neg.
};

#endif // BOOTH_ENCODER_RADIX_4
