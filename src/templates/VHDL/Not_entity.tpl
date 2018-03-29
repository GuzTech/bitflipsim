library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

ENTITY NotC IS
PORT (
    I : in  STD_LOGIC;
    O : out STD_LOGIC
);
END NotC;

ARCHITECTURE arch OF NotC IS
BEGIN
    O <= not I;
END arch;
