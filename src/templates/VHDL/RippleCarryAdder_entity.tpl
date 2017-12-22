library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

ENTITY RippleCarryAdder IS
GENERIC (
    NUM_BITS : INTEGER
);
PORT (
    A    : in  STD_LOGIC_VECTOR(NUM_BITS - 1 DOWNTO 0);
    B    : in  STD_LOGIC_VECTOR(NUM_BITS - 1 DOWNTO 0);
    Cin  : in  STD_LOGIC;
    O    : out STD_LOGIC_VECTOR(NUM_BITS - 1 DOWNTO 0);
    Cout : out STD_LOGIC
);
END RippleCarryAdder;

ARCHITECTURE arch OF RippleCarryAdder IS
    SIGNAL fa_cout : STD_LOGIC_VECTOR(NUM_BITS - 1 DOWNTO 0);
BEGIN
    GEN_FULL_ADDERS: FOR I in 0 to NUM_BITS - 1 GENERATE
        LSB: IF I = 0 GENERATE
            FA_i : work.FullAdder
            PORT MAP (
                A => A(I),
                B => B(I),
                Cin => Cin,
                O => O(I),
                Cout => fa_cout(I)
            );
        END GENERATE LSB;
        
        FAs : IF I > 0 GENERATE
            FA_i : work.FullAdder
            PORT MAP (
                A => A(I),
                B => B(I),
                Cin => fa_cout(I - 1),
                O => O(I),
                Cout => fa_cout(I)
            );
        END GENERATE FAs;
    END GENERATE GEN_FULL_ADDERS;
    
    Cout <= fa_cout(NUM_BITS - 1);
END arch;
