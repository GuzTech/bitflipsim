#ifndef UTILS_H
#define UTILS_H

void Error(const string &err) __attribute__ ((noreturn));

enum class PORTS {A, B, C, Cin, Cout, I, O, S, X_2I, X_2I_MINUS_ONE, X_2I_PLUS_ONE, Y_LSB, Y_MSB, NEG, SE, ROW_LSB, X1_b, X2_b, Z, Yj, Yj_m1, PPTj, NEG_CIN};
enum class PORT_DIR {INPUT, OUTPUT};
enum class NUMFMT {NONE, TWOS_COMPLEMENT, ONES_COMPLEMENT, SIGNED_MAGNITUDE, UNSIGNED};
enum class LAYOUT {NONE, CARRY_PROPAGATE, CARRY_SAVE, BOOTH_RADIX_2, BOOTH_RADIX_4};
enum class TYPE {NONE, INVERSION, SIGN_EXTEND, BAUGH_WOOLEY};

extern map<string, PORTS> PortNameToPortMap;
extern map<PORTS, string> PortToPortNameMap;

#endif // UTILS_H
