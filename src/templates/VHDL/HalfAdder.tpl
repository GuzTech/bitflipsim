library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

ENTITY HalfAdder IS
PORT (
    A    : in  STD_LOGIC;
    B    : in  STD_LOGIC;
    O    : out STD_LOGIC;
    Cout : out STD_LOGIC
);
END HalfAdder;

ARCHITECTURE arch OF HalfAdder IS
BEGIN
    O <= A xor B;
    Cout <= A and B;
END arch;
