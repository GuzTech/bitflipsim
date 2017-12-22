#ifndef BOOTH_ENCODER_RADIX_4
#define BOOTH_ENCODER_RADIX_4

#include "main.h"

//#define METHOD_BEWICK

class BoothEncoderRadix4 : public Component {
public:
	BoothEncoderRadix4(string _name);
	~BoothEncoderRadix4() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, const wire_t &wire, size_t index = 0) override;
	void Connect(PORTS port, const wb_t &wires, size_t port_idx = 0, size_t wire_idx = 0) override;

	const wire_t GetWire(PORTS port, size_t index = 0) const override;

	void PrintDebug() const override;
private:
	xor_t X2_b;		// x2 / x-2
	xnor_t X1_b;	// x1 / x-1
	xnor_t Z;		// Zero
	and_t Row_LSB;	// Value of LSB of partial product
	nor_t Neg_cin_nor_1;	// Neg_cin
	nor_t Neg_cin_nor_2;
	nor_t Neg_cin_nor_3;
	or3_t Neg_cin_or3;
	and_t Neg_cin_and;

#ifdef METHOD_BEWICK
	xnor_t SE_xnor;	// Sign-extension
	nor3_t SE_nor3;
	and3_t SE_and3;
	or_t SE_or;
	and_t SE_and;
	xor_t SE_xor;
#else
	xnor_t SE_xnor;
	nor3_t SE_nor3;
	and3_t SE_and3;
	or3_t SE_or3;
#endif

	wire_t x2_neg;	// Wire that connects X_2i+1 to Neg.
};

#endif // BOOTH_ENCODER_RADIX_4
