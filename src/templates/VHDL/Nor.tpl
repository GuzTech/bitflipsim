library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

ENTITY Nor IS
PORT (
    A : in  STD_LOGIC;
    B : in  STD_LOGIC;
    O : out STD_LOGIC
);
END Nor;

ARCHITECTURE arch OF Nor IS
BEGIN
    O <= A nor B;
END arch;
