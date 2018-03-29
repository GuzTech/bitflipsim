library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

ENTITY OrC IS
PORT (
    A : in  STD_LOGIC;
    B : in  STD_LOGIC;
    O : out STD_LOGIC
);
END OrC;

ARCHITECTURE arch OF OrC IS
BEGIN
    O <= A or B;
END arch;
