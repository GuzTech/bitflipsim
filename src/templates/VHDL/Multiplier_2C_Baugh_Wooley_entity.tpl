LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.NUMERIC_STD.ALL;

ENTITY Multiplier_2C_Baugh_Wooley IS
GENERIC (
	NUM_BITS_A : INTEGER := 8;
	NUM_BITS_B : INTEGER := 8
);
PORT (
	 A : IN  STD_LOGIC_VECTOR(NUM_BITS_A - 1 DOWNTO 0);
	 B : IN  STD_LOGIC_VECTOR(NUM_BITS_B - 1 DOWNTO 0);
	 O : OUT STD_LOGIC_VECTOR(NUM_BITS_A + NUM_BITS_B - 1 DOWNTO 0)
);
END Multiplier_2C_Baugh_Wooley;

ARCHITECTURE carry_save OF Multiplier_2C_Baugh_Wooley IS
	CONSTANT NUM_BITS_O       : INTEGER := NUM_BITS_A + NUM_BITS_B;
	CONSTANT NUM_ADD_LVLS     : INTEGER := NUM_BITS_B - 1;
	CONSTANT NUM_AND_LVLS     : INTEGER := NUM_BITS_B;
	CONSTANT NUM_ADDS_PER_LVL : INTEGER := NUM_BITS_A;
	CONSTANT NUM_ANDS_PER_LVL : INTEGER := NUM_BITS_A;

	TYPE array_and IS ARRAY (0 TO NUM_AND_LVLS - 1) OF STD_LOGIC_VECTOR(NUM_ANDS_PER_LVL - 1 DOWNTO 0);
	SIGNAL and_o : array_and;

	TYPE array_cs IS ARRAY (0 TO NUM_ADD_LVLS - 1) OF STD_LOGIC_VECTOR(NUM_ADDS_PER_LVL - 1 DOWNTO 0);
	SIGNAL cs_A    : array_cs;
	SIGNAL cs_B    : array_cs;
	SIGNAL cs_O    : array_cs;
	SIGNAL cs_Cin  : array_cs;
	SIGNAL cs_Cout : array_cs;
BEGIN
	O(0)          <= and_o(0)(0);
	O(O'HIGH)     <= NOT cs_Cout(cs_Cout'HIGH)(NUM_ANDS_PER_LVL - 1);

	gen_ands_i: FOR i IN 0 TO (NUM_AND_LVLS - 1) GENERATE
	    gen_row: IF (i < (NUM_AND_LVLS - 1)) GENERATE
		    gen_ands_j: FOR j IN 0 TO (NUM_ANDS_PER_LVL - 1) GENERATE
		        gen_col: IF (j < (NUM_ANDS_PER_LVL - 1)) GENERATE
		            and_o(i)(j) <= A(j) AND B(i);
		        END GENERATE gen_col;
		        
		        gen_col_last: IF (j = (NUM_ANDS_PER_LVL - 1)) GENERATE
		            and_o(i)(j) <= NOT (A(j) AND B(i));
		        END GENERATE gen_col_last;
		    END GENERATE gen_ands_j;
		END GENERATE gen_row;
		
		gen_row_last: IF (i = (NUM_AND_LVLS -1)) GENERATE
		    gen_ands_j: FOR j IN 0 TO (NUM_ANDS_PER_LVL - 1) GENERATE
		        gen_col: IF (j < (NUM_ANDS_PER_LVL - 1)) GENERATE
		            and_o(i)(j) <= NOT (A(j) AND B(i));
		        END GENERATE gen_col;
		        
		        gen_col_last: IF (j = (NUM_ANDS_PER_LVL - 1)) GENERATE
		            and_o(i)(j) <= A(j) AND B(j);
		        END GENERATE gen_col_last;
		    END GENERATE gen_ands_j;
		END GENERATE gen_row_last;
	END GENERATE gen_ands_i;

	gen_adders: FOR i IN 0 TO (NUM_ADD_LVLS - 1) GENERATE
		csa0: IF (i = 0) GENERATE
			cs_A(i)   <= '1' & and_o(0)(and_o(0)'HIGH DOWNTO 1);
			cs_B(i)   <= and_o(1);
			cs_Cin(i) <= and_o(2)(and_o(2)'HIGH - 1 DOWNTO 0) & '0';

			csa : ENTITY work.CarrySaveAdder(arch)
            GENERIC MAP (
                NUM_BITS => NUM_ADDS_PER_LVL
            )
            PORT MAP (
                A    => cs_A(i),
                B    => cs_B(i),
                Cin  => cs_Cin(i),
                O    => cs_O(i),
                Cout => cs_Cout(i)
            );
		END GENERATE csa0;

    	csan: IF (i > 0 AND i < (NUM_ADD_LVLS - 1)) GENERATE
			cs_A(i)   <= and_o(i + 1)(and_o(i + 1)'HIGH) & cs_O(i - 1)(cs_O(i - 1)'HIGH DOWNTO 1);
			cs_B(i)   <= cs_Cout(i - 1);
			cs_Cin(i) <= and_o(i + 2)(and_o(i + 2)'HIGH - 1 DOWNTO 0) & '0';

			csa : ENTITY work.CarrySaveAdder(arch)
            GENERIC MAP (
                NUM_BITS => NUM_ADDS_PER_LVL
            )
            PORT MAP (
                A    => cs_A(i),
                B    => cs_B(i),
                Cin  => cs_Cin(i),
                O    => cs_O(i),
                Cout => cs_Cout(i)
            );
		END GENERATE csan;
		
		csal: IF (i = (NUM_ADD_LVLS - 1)) GENERATE
		    cs_A(i) <= and_o(i + 1)(and_o(i + 1)'HIGH) & cs_O(i - 1)(cs_O(i - 1)'HIGH DOWNTO 1);
            cs_B(i) <= cs_Cout(i - 1);

		    rca : ENTITY work.RippleCarryAdder(arch)
		    GENERIC MAP (
		        NUM_BITS => NUM_ADDS_PER_LVL
		    )
		    PORT MAP (
		        A    => cs_A(i),
		        B    => cs_B(i),
		        Cin  => '0',
		        O    => cs_O(i),
		        Cout => cs_Cout(i)(cs_Cout(i)'HIGH)
		    );
		END GENERATE csal;		
	END GENERATE gen_adders;

	gen_outs: FOR i IN 1 TO (NUM_BITS_O - 1) GENERATE
		out_low: IF (i > 0 AND i < (NUM_BITS_O - NUM_ADDS_PER_LVL - 1)) GENERATE
			O(i) <= cs_O(i - 1)(0);
		END GENERATE out_low;

		out_high: IF (i >= (NUM_BITS_O - NUM_ADDS_PER_LVL - 1) AND i < (NUM_BITS_O - 1)) GENERATE
			O(i) <= cs_O(cs_O'HIGH)(i - NUM_ADDS_PER_LVL + 1);
		END GENERATE out_high;
	END GENERATE gen_outs;
END carry_save;