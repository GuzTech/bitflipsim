library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

ENTITY AndC IS
PORT (
    A : in  STD_LOGIC;
    B : in  STD_LOGIC;
    O : out STD_LOGIC
);
END AndC;

ARCHITECTURE arch OF AndC IS
BEGIN
    O <= A and B;
END arch;
