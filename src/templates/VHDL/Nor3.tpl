library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

ENTITY Nor3 IS
PORT (
    A : in  STD_LOGIC;
    B : in  STD_LOGIC;
    C : in  STD_LOGIC;
    O : out STD_LOGIC
);
END Nor3;

ARCHITECTURE arch OF Nor3 IS
BEGIN
    O <= A nor B nor C;
END arch;
