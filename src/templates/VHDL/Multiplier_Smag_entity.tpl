LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.NUMERIC_STD.ALL;

ENTITY Multiplier_Smag IS
GENERIC (
	NUM_BITS_A : INTEGER := 8;
	NUM_BITS_B : INTEGER := 8
);
PORT (
	 A : IN  STD_LOGIC_VECTOR(NUM_BITS_A - 1 DOWNTO 0);
	 B : IN  STD_LOGIC_VECTOR(NUM_BITS_B - 1 DOWNTO 0);
	 O : OUT STD_LOGIC_VECTOR(NUM_BITS_A + NUM_BITS_B - 2 DOWNTO 0)
);
END Multiplier_Smag;

ARCHITECTURE carry_propagate OF Multiplier_Smag IS
	CONSTANT N_BITS_A         : INTEGER := NUM_BITS_A - 1;
	CONSTANT N_BITS_B         : INTEGER := NUM_BITS_B - 1;
	CONSTANT NUM_BITS_O       : INTEGER := N_BITS_A + N_BITS_B + 1;
	CONSTANT NUM_ADD_LVLS     : INTEGER := N_BITS_B - 1;
	CONSTANT NUM_AND_LVLS     : INTEGER := N_BITS_B;
	CONSTANT NUM_ADDS_PER_LVL : INTEGER := N_BITS_A;
	CONSTANT NUM_ANDS_PER_LVL : INTEGER := N_BITS_A;

	TYPE array_and IS ARRAY (0 TO NUM_AND_LVLS - 1) OF STD_LOGIC_VECTOR(NUM_ANDS_PER_LVL - 1 DOWNTO 0);
	SIGNAL and_o : array_and;

	TYPE array_rc IS ARRAY (0 TO NUM_ADD_LVLS - 1) OF STD_LOGIC_VECTOR(NUM_ADDS_PER_LVL - 1 DOWNTO 0);
	SIGNAL rc_A    : array_rc;
	SIGNAL rc_B    : array_rc;
	SIGNAL rc_O    : array_rc;
	SIGNAL rc_Cout : STD_LOGIC_VECTOR(0 TO NUM_ADD_LVLS - 1);
BEGIN
	O(0) <= and_o(0)(0);
	O(O'HIGH) <= A(A'HIGH) XOR B(B'HIGH);
	O(O'HIGH - 1) <= rc_Cout(rc_Cout'HIGH);

	gen_ands_i: FOR i IN 0 TO NUM_AND_LVLS - 1 GENERATE
		gen_ands_j: FOR j IN 0 TO NUM_ANDS_PER_LVL - 1 GENERATE
			and_o(i)(j) <= A(j) AND B(i);
		END GENERATE gen_ands_j;
	END GENERATE gen_ands_i;

	gen_adders: FOR i IN 0 TO (NUM_ADD_LVLS - 1) GENERATE
		rca0: IF (i = 0) GENERATE
			rc_A(i) <= '0' & and_o(0)(and_o(0)'HIGH DOWNTO 1);
			rc_B(i) <= and_o(1);
			O(i)    <= and_o(0)(0);
		END GENERATE rca0;

		rcan: IF (i > 0) GENERATE
			rc_A(i) <= rc_Cout(i - 1) & rc_O(i - 1)(rc_O(i - 1)'HIGH DOWNTO 1);
			rc_B(i) <= and_o(i + 1);
		END GENERATE rcan;

		rca : ENTITY work.RippleCarryAdder(arch)
        GENERIC MAP (
            NUM_BITS => NUM_ADDS_PER_LVL
        )
        PORT MAP (
            A    => rc_A(i),
            B    => rc_B(i),
            Cin  => '0',
            O    => rc_O(i),
            Cout => rc_Cout(i)
        );
	END GENERATE gen_adders;

	gen_outs: FOR i IN 1 TO (NUM_BITS_O - 1) GENERATE
		out_low: IF (i > 0 AND i < (NUM_BITS_O - NUM_ADDS_PER_LVL - 1)) GENERATE
			O(i) <= rc_O(i - 1)(0);
		END GENERATE out_low;

		out_high: IF (i > (NUM_BITS_O - NUM_ADDS_PER_LVL - 2) AND i < (NUM_BITS_O - 2)) GENERATE
			O(i) <= rc_O(rc_O'HIGH)(i - NUM_ADDS_PER_LVL + 1);
		END GENERATE out_high;
	END GENERATE gen_outs;
END carry_propagate;

ARCHITECTURE carry_save OF Multiplier_Smag IS
	CONSTANT N_BITS_A         : INTEGER := NUM_BITS_A - 1;
	CONSTANT N_BITS_B         : INTEGER := NUM_BITS_B - 1;
	CONSTANT NUM_BITS_O       : INTEGER := N_BITS_A + N_BITS_B + 1;
	CONSTANT NUM_ADD_LVLS     : INTEGER := N_BITS_B - 1;
	CONSTANT NUM_AND_LVLS     : INTEGER := N_BITS_B;
	CONSTANT NUM_ADDS_PER_LVL : INTEGER := N_BITS_A;
	CONSTANT NUM_ANDS_PER_LVL : INTEGER := N_BITS_A;

	TYPE array_and IS ARRAY (0 TO NUM_AND_LVLS - 1) OF STD_LOGIC_VECTOR(NUM_ANDS_PER_LVL - 1 DOWNTO 0);
	SIGNAL and_o : array_and;

	TYPE array_cs IS ARRAY (0 TO NUM_ADD_LVLS - 1) OF STD_LOGIC_VECTOR(NUM_ADDS_PER_LVL - 1 DOWNTO 0);
	SIGNAL cs_A    : array_cs;
	SIGNAL cs_B    : array_cs;
	SIGNAL cs_O    : array_cs;
	SIGNAL cs_Cin  : array_cs;
	SIGNAL cs_Cout : array_cs;
BEGIN
	O(0) <= and_o(0)(0);
	O(O'HIGH) <= A(A'HIGH) XOR B(B'HIGH);
--	O(O'HIGH - 1) <= rc_Cout(rc_Cout'HIGH);

	gen_ands_i: FOR i IN 0 TO NUM_AND_LVLS - 1 GENERATE
		gen_ands_j: FOR j IN 0 TO NUM_ANDS_PER_LVL - 1 GENERATE
			and_o(i)(j) <= A(j) AND B(i);
		END GENERATE gen_ands_j;
	END GENERATE gen_ands_i;

	gen_adders: FOR i IN 0 TO (NUM_ADD_LVLS - 1) GENERATE
		csa0: IF (i = 0) GENERATE
			cs_A(i) <= '0' & and_o(0)(and_o(0)'HIGH DOWNTO 1);
			cs_B(i) <= and_o(1);
			O(i)    <= and_o(0)(0);
		END GENERATE csa0;

		csan: IF (i > 0) GENERATE
			cs_A(i) <= cs_Cout(i - 1) & cs_O(i - 1)(cs_O(i - 1)'HIGH DOWNTO 1);
			cs_B(i) <= and_o(i + 1);
		END GENERATE csan;

		csa : ENTITY work.CarrySaveAdder(arch)
        GENERIC MAP (
            NUM_BITS => NUM_ADDS_PER_LVL
        )
        PORT MAP (
            A    => cs_A(i),
            B    => cs_B(i),
            Cin  => (OTHERS => '0'),
            O    => cs_O(i),
            Cout => cs_Cout(i)
        );
	END GENERATE gen_adders;

	gen_outs: FOR i IN 1 TO (NUM_BITS_O - 1) GENERATE
		out_low: IF (i > 0 AND i < (NUM_BITS_O - NUM_ADDS_PER_LVL - 1)) GENERATE
			O(i) <= cs_O(i - 1)(0);
		END GENERATE out_low;

		out_high: IF (i > (NUM_BITS_O - NUM_ADDS_PER_LVL - 2) AND i < (NUM_BITS_O - 2)) GENERATE
--			O(i) <= cs_O(rc_O'HIGH)(i - NUM_ADDS_PER_LVL + 1);
		END GENERATE out_high;
	END GENERATE gen_outs;
END carry_save;
