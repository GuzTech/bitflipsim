#ifndef MAIN_H
#define MAIN_H

#include <cstdint>
#include <iostream>
#include <memory>
#include <algorithm>
#include <vector>
#include <map>
#include <climits>

using namespace std;

class Component;
class HalfAdder;
class FullAdder;
class RippleCarryAdder;
class Multiplier_2C;
class Multiplier_Smag;
class And;
class Or;
class Xor;
class Nand;
class Nor;
class Xnor;
class Not;
class Mux;
class WireBundle;
class Wire;
class System;

enum class PORTS {A, B, Cin, Cout, I, O, S};
enum class NUMFMT {NONE, TWOS_COMPLEMENT, ONES_COMPLEMENT, SIGNED_MAGNITUDE, UNSIGNED};
enum class LAYOUT {NONE, CARRY_PROPAGATE, CARRY_SAVE, BOOTH_RADIX_2, BOOTH_RADIX_4};
enum class TYPE {NONE, INVERSION, SIGN_EXTEND};

using wire_t  = shared_ptr<Wire>;
using wb_t    = shared_ptr<WireBundle>;
using comp_t  = shared_ptr<Component>;
using comp_wt = weak_ptr<Component>;
using ha_t    = shared_ptr<HalfAdder>;
using fa_t    = shared_ptr<FullAdder>;
using and_t   = shared_ptr<And>;
using or_t    = shared_ptr<Or>;
using xor_t   = shared_ptr<Xor>;
using nand_t  = shared_ptr<Nand>;
using nor_t   = shared_ptr<Nor>;
using xnor_t  = shared_ptr<Xnor>;
using not_t   = shared_ptr<Not>;
using mux_t   = shared_ptr<Mux>;
using rca_t   = shared_ptr<RippleCarryAdder>;
using m2c_t   = shared_ptr<Multiplier_2C>;
using msmag_t = shared_ptr<Multiplier_Smag>;

#include "Component.h"
#include "HalfAdder.h"
#include "FullAdder.h"
#include "RippleCarryAdder.h"
#include "Multiplier_2C.h"
#include "Multiplier_Smag.h"
#include "And.h"
#include "Or.h"
#include "Xor.h"
#include "Nand.h"
#include "Nor.h"
#include "Xnor.h"
#include "Not.h"
#include "Mux.h"
#include "WireBundle.h"
#include "Wire.h"
#include "System.h"

#endif // MAIN_H
