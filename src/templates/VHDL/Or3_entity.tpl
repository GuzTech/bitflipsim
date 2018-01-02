library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

ENTITY Or3 IS
PORT (
    A : in  STD_LOGIC;
    B : in  STD_LOGIC;
    C : in  STD_LOGIC;
    O : out STD_LOGIC
);
END Or3;

ARCHITECTURE arch OF Or3 IS
BEGIN
    O <= A or B or C;
END arch;
