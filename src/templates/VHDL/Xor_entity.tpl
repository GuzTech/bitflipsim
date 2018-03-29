library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

ENTITY XorC IS
PORT (
    A : in  STD_LOGIC;
    B : in  STD_LOGIC;
    O : out STD_LOGIC
);
END XorC;

ARCHITECTURE arch OF XorC IS
BEGIN
    O <= A xor B;
END arch;
