LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.NUMERIC_STD.ALL;

ENTITY RippleCarrySubtracter IS
GENERIC (
    NUM_BITS : INTEGER := 4
);
PORT (
    A    : IN  STD_LOGIC_VECTOR(NUM_BITS - 1 DOWNTO 0);
    B    : IN  STD_LOGIC_VECTOR(NUM_BITS - 1 DOWNTO 0);
    O    : OUT STD_LOGIC_VECTOR(NUM_BITS - 1 DOWNTO 0);
    Cout : OUT STD_LOGIC
);
END RippleCarrySubtracter;

ARCHITECTURE arch OF RippleCarrySubtracter IS
    SIGNAL not_out : STD_LOGIC_VECTOR(NUM_BITS - 1 DOWNTO 0);
    SIGNAL fa_cout : STD_LOGIC_VECTOR(NUM_BITS - 1 DOWNTO 0);
BEGIN
    gen_full_adders: FOR I IN 0 TO NUM_BITS - 1 GENERATE
        not_out(I) <= not B(I);
    
        lsb: IF I = 0 GENERATE
            FA_i : ENTITY work.FullAdder(arch)
            PORT MAP (
                A    => A(I),
                B    => not_out(I),
                Cin  => '1',
                O    => O(I),
                Cout => fa_cout(I)
            );
		END GENERATE lsb;
    
        FAs : IF I > 0 GENERATE
            FA_i : ENTITY work.FullAdder(arch)
            PORT MAP (
                A    => A(I),
                B    => not_out(I),
                Cin  => fa_cout(I - 1),
                O    => O(I),
                Cout => fa_cout(I)
            );
        END GENERATE FAs;
    END GENERATE gen_full_adders;
    
    Cout <= fa_cout(NUM_BITS - 1);
END arch;
