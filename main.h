#ifndef MAIN_H
#define MAIN_H

#include <cstdint>
#include <iostream>
#include <memory>
#include <algorithm>
#include <vector>
#include <map>

class Component;
class HalfAdder;
class FullAdder;
class And;
class Or;
class Xor;
class Wire;
class System;

using wire_t = std::shared_ptr<Wire>;
using comp_t = std::shared_ptr<Component>;
using ha_t   = std::shared_ptr<HalfAdder>;
using fa_t   = std::shared_ptr<FullAdder>;
using and_t  = std::shared_ptr<And>;
using or_t   = std::shared_ptr<Or>;
using xor_t  = std::shared_ptr<Xor>;

#include "Component.h"
#include "HalfAdder.h"
#include "FullAdder.h"
#include "And.h"
#include "Or.h"
#include "Xor.h"
#include "Wire.h"
#include "System.h"

#endif // MAIN_H
