library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

ENTITY CarrySaveAdder IS
GENERIC (
    NUM_BITS : INTEGER
);
PORT (
    A    : in  STD_LOGIC_VECTOR(NUM_BITS - 1 DOWNTO 0);
    B    : in  STD_LOGIC_VECTOR(NUM_BITS - 1 DOWNTO 0);
    Cin  : in  STD_LOGIC_VECTOR(NUM_BITS - 1 DOWNTO 0);
    O    : out STD_LOGIC_VECTOR(NUM_BITS - 1 DOWNTO 0);
    Cout : out STD_LOGIC_VECTOR(NUM_BITS - 1 DOWNTO 0)
);
END CarrySaveAdder;

ARCHITECTURE arch OF CarrySaveAdder IS
BEGIN
    GEN_FULL_ADDERS: FOR I in 0 to NUM_BITS - 1 GENERATE
        FA_i : work.FullAdder
        PORT MAP (
            A => A(I),
            B => B(I),
            Cin => Cin(I),
            O => O(I),
            Cout => Cout(I)
        );
    END GENERATE GEN_FULL_ADDERS;
END arch;
