#ifndef MAIN_H
#define MAIN_H

#include <cstdint>
#include <iostream>
#include <memory>
#include <algorithm>
#include <vector>
#include <map>
#include <climits>
#include <fstream>
#include <ctemplate/template.h>

using namespace std;
using namespace ctemplate;

class Component;
class HalfAdder;
class FullAdder;
class RippleCarryAdder;
class CarrySaveAdder;
class Multiplier_2C;
class Multiplier_Smag;
class BoothEncoderRadix4;
class Radix4BoothDecoder;
class Multiplier_2C_Booth;
class And;
class And3;
class Or;
class Or3;
class Xor;
class Nand;
class Nor;
class Nor3;
class Xnor;
class Not;
class Mux;
class WireBundle;
class Wire;
class System;

enum class PORTS {A, B, C, Cin, Cout, I, O, S, X_2I, X_2I_MINUS_ONE, X_2I_PLUS_ONE, Y_LSB, Y_MSB, NEG, SE, ROW_LSB, X1_b, X2_b, Z, Yj, Yj_m1, PPTj, NEG_CIN};
enum class NUMFMT {NONE, TWOS_COMPLEMENT, ONES_COMPLEMENT, SIGNED_MAGNITUDE, UNSIGNED};
enum class LAYOUT {NONE, CARRY_PROPAGATE, CARRY_SAVE, BOOTH_RADIX_2, BOOTH_RADIX_4};
enum class TYPE {NONE, INVERSION, SIGN_EXTEND, BAUGH_WOOLEY};

using wire_t  = shared_ptr<Wire>;
using wb_t    = shared_ptr<WireBundle>;
using comp_t  = shared_ptr<Component>;
using comp_wt = weak_ptr<Component>;
using ha_t    = shared_ptr<HalfAdder>;
using fa_t    = shared_ptr<FullAdder>;
using and_t   = shared_ptr<And>;
using and3_t  = shared_ptr<And3>;
using or_t    = shared_ptr<Or>;
using or3_t   = shared_ptr<Or3>;
using xor_t   = shared_ptr<Xor>;
using nand_t  = shared_ptr<Nand>;
using nor_t   = shared_ptr<Nor>;
using nor3_t  = shared_ptr<Nor3>;
using xnor_t  = shared_ptr<Xnor>;
using not_t   = shared_ptr<Not>;
using mux_t   = shared_ptr<Mux>;
using rca_t   = shared_ptr<RippleCarryAdder>;
using csa_t   = shared_ptr<CarrySaveAdder>;
using m2c_t   = shared_ptr<Multiplier_2C>;
using msmag_t = shared_ptr<Multiplier_Smag>;
using b_enc_t = shared_ptr<BoothEncoderRadix4>;
using b_r4d_t = shared_ptr<Radix4BoothDecoder>;
using m2cb_t  = shared_ptr<Multiplier_2C_Booth>;

#include "Component.h"
#include "HalfAdder.h"
#include "FullAdder.h"
#include "RippleCarryAdder.h"
#include "CarrySaveAdder.h"
#include "Multiplier_2C.h"
#include "Multiplier_Smag.h"
#include "BoothEncoderRadix4.h"
#include "Radix4BoothDecoder.h"
#include "Multiplier_2C_Booth.h"
#include "And.h"
#include "And3.h"
#include "Or.h"
#include "Or3.h"
#include "Xor.h"
#include "Nand.h"
#include "Nor.h"
#include "Nor3.h"
#include "Xnor.h"
#include "Not.h"
#include "Mux.h"
#include "WireBundle.h"
#include "Wire.h"
#include "System.h"

#endif // MAIN_H
