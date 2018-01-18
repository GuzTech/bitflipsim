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
	CONSTANT NUM_BITS_O       : INTEGER := N_BITS_A + N_BITS_B - 1;
	CONSTANT NUM_ADD_LVLS     : INTEGER := N_BITS_B - 1;
	CONSTANT NUM_AND_LVLS     : INTEGER := N_BITS_B;
	CONSTANT NUM_ADDS_PER_LVL : INTEGER := N_BITS_A;
	CONSTANT NUM_ANDS_PER_LVL : INTEGER := N_BITS_A;

	SIGNAL A_0_input : STD_LOGIC_VECTOR(NUM_ADDS_PER_LVL - 1 DOWNTO 0);
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

	gen_ands_i: FOR i IN 0 TO NUM_AND_LVLS - 1 GENERATE
		gen_ands_j: FOR j IN 0 TO NUM_ANDS_PER_LVL - 1 GENERATE
			and_o(i)(j) <= A(j) AND B(i);
		END GENERATE gen_ands_j;
	END GENERATE gen_ands_i;

	gen_adders: FOR i IN 0 TO (NUM_ADD_LVLS - 1) GENERATE
		A_0_input <= '0' & and_o(0)(and_o(0)'HIGH DOWNTO 1);
		rca0: IF (i = 0) GENERATE
			rca : ENTITY work.RippleCarryAdder(arch)
			GENERIC MAP (
				NUM_BITS => NUM_ADDS_PER_LVL
			)
			PORT MAP (
				A    => A_0_input,
				B    => and_o(1),
				Cin  => '0',
				O    => rc_O(i),
				Cout => rc_Cout(i)
			);
		END GENERATE rca0;

		rcan: IF (i > 0) GENERATE
			rca : ENTITY work.RippleCarryAdder(arch)
			GENERIC MAP (
				NUM_BITS => NUM_ADDS_PER_LVL
			)
			PORT MAP (
				A    => and_o(i + 1),
				B    => rc_O(i - 1),
				Cin  => '0',
				O    => rc_O(i),
				Cout => rc_Cout(i)
			);
		END GENERATE rcan;
	END GENERATE gen_adders;

END carry_propagate;

ARCHITECTURE carry_save OF Multiplier_Smag IS
	CONSTANT N_BITS_A         : INTEGER := NUM_BITS_A - 1;
	CONSTANT N_BITS_B         : INTEGER := NUM_BITS_B - 1;
	CONSTANT NUM_BITS_O       : INTEGER := N_BITS_A + N_BITS_B - 1;
	CONSTANT NUM_ADD_LVLS     : INTEGER := N_BITS_B - 1;
	CONSTANT NUM_AND_LVLS     : INTEGER := N_BITS_B;
	CONSTANT NUM_ADDS_PER_LVL : INTEGER := N_BITS_A;
	CONSTANT NUM_ANDS_PER_LVL : INTEGER := N_BITS_A;

	TYPE array_and IS ARRAY (0 TO NUM_AND_LVLS - 1) OF STD_LOGIC_VECTOR(NUM_ANDS_PER_LVL - 1 DOWNTO 0);
	SIGNAL and_o : array_and;
BEGIN
	O(0) <= and_o(0)(0);
	O(O'HIGH) <= A(A'HIGH) XOR B(B'HIGH);

	gen_ands_i: FOR i IN 0 TO NUM_AND_LVLS - 1 GENERATE
		gen_ands_j: FOR j IN 0 TO NUM_ANDS_PER_LVL - 1 GENERATE
			and_o(i)(j) <= A(j) AND B(i);
		END GENERATE gen_ands_j;
	END GENERATE gen_ands_i;

--	gen_adders: FOR i IN 0 TO (NUM_ADD_LVLS - 1) GENERATE
--		csai : ENTITY work.CarrySaveAdder(arch)
--		GENERIC MAP (
--			NUM_BITS => NUM_ADDS_PER_LVL
--		)
--		PORT MAP (
--			A    =>
--			B    =>
--			Cin  =>
--			O    =>
--			Cout =>
--		);
--	END GENERATE gen_adders;
END carry_save;
