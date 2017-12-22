library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

ENTITY Not IS
PORT (
    I : in  STD_LOGIC;
    O : out STD_LOGIC
);
END Not;

ARCHITECTURE arch OF Not IS
BEGIN
    O <= not I;
END arch;
