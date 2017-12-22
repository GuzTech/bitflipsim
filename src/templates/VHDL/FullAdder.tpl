library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

ENTITY FullAdder IS
PORT (
    A    : in  STD_LOGIC;
    B    : in  STD_LOGIC;
    Cin  : in  STD_LOGIC;
    O    : out STD_LOGIC;
    Cout : out STD_LOGIC
);
END FullAdder;

ARCHITECTURE arch OF FullAdder IS
    SIGNAL iw_1 : STD_LOGIC;
    SIGNAL iw_2 : STD_LOGIC;
    SIGNAL iw_3 : STD_LOGIC;
BEGIN
    iw_1 <= A xor B;
    iw_2 <= iw_1 and Cin;
    iw_3 <= A and B;

    O <= iw_1 xor Cin;
    Cout <= iw_2 or iw_3;
END arch;
