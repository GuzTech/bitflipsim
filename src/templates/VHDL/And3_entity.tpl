library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

ENTITY Add3 IS
PORT (
    A : in  STD_LOGIC;
    B : in  STD_LOGIC;
    C : in  STD_LOGIC;
    O : out STD_LOGIC
);
END Add3;

ARCHITECTURE arch OF Add3 IS
BEGIN
    O <= A and B and C;
END arch;
