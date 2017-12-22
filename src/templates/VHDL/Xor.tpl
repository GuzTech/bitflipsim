library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

ENTITY Xor IS
PORT (
    A : in  STD_LOGIC;
    B : in  STD_LOGIC;
    O : out STD_LOGIC
);
END Xor;

ARCHITECTURE arch OF Xor IS
BEGIN
    O <= A xor B;
END arch;
