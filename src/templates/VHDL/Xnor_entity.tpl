library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

ENTITY Xnor IS
PORT (
    A : in  STD_LOGIC;
    B : in  STD_LOGIC;
    O : out STD_LOGIC
);
END Xnor;

ARCHITECTURE arch OF Xnor IS
BEGIN
    O <= A xnor B;
END arch;
