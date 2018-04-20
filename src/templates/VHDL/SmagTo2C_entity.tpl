LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.NUMERIC_STD.ALL;

ENTITY SmagTo2C IS
GENERIC (
    NUM_BITS : INTEGER := 4
);
PORT (
    A    : IN  STD_LOGIC_VECTOR(NUM_BITS - 1 DOWNTO 0);
    O    : OUT STD_LOGIC_VECTOR(NUM_BITS - 1 DOWNTO 0)
);
END SmagTo2C;

ARCHITECTURE arch OF SmagTo2C IS
    SIGNAL xor_out : STD_LOGIC_VECTOR(NUM_BITS - 2 DOWNTO 0);
    SIGNAL ha_cout : STD_LOGIC_VECTOR(NUM_BITS - 2 DOWNTO 0);
BEGIN
    gen_xors: FOR I IN 0 TO NUM_BITS - 2 GENERATE
        xor_out(I) <= A(A'HIGH) XOR A(I);
    END GENERATE gen_xors;

    gen_half_adders: FOR I IN 0 TO NUM_BITS - 1 GENERATE
        lsb: IF I = 0 GENERATE
            HA_i : ENTITY work.HalfAdder(arch)
            PORT MAP (
                A    => xor_out(I),
                B    => A(A'HIGH),
                O    => O(I),
                Cout => ha_cout(I)
            );
		END GENERATE lsb;

        HAs : IF (I > 0 AND I < (NUM_BITS - 1)) GENERATE
            HA_i : ENTITY work.HalfAdder(arch)
            PORT MAP (
                A    => xor_out(I),
                B    => ha_cout(I - 1),
                O    => O(I),
                Cout => ha_cout(I)
            );
        END GENERATE HAs;

        msb: IF I = (NUM_BITS - 1) GENERATE
            HA_i : ENTITY work.HalfAdder(arch)
            PORT MAP (
                A    => A(A'HIGH),
                B    => ha_cout(I - 1),
                O    => O(I),
                Cout => OPEN
            );
		END GENERATE msb;
    END GENERATE gen_half_adders;
END arch;
