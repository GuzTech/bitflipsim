LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.NUMERIC_STD.ALL;

ENTITY Multiplier_2C_Booth IS
GENERIC (
	NUM_BITS_A : INTEGER := 8;
	NUM_BITS_B : INTEGER := 8
);
PORT (
	 A : IN  STD_LOGIC_VECTOR(NUM_BITS_A - 1 DOWNTO 0);
	 B : IN  STD_LOGIC_VECTOR(NUM_BITS_B - 1 DOWNTO 0);
	 O : OUT STD_LOGIC_VECTOR(NUM_BITS_A + NUM_BITS_B - 1 DOWNTO 0)
);
END Multiplier_2C_Booth;

ARCHITECTURE arch OF Multiplier_2C_Booth IS
	CONSTANT NUM_BITS_O     : INTEGER := NUM_BITS_A + NUM_BITS_B;
	CONSTANT NUM_ENCODERS   : INTEGER := (NUM_BITS_B + 1) / 2;
	CONSTANT NUM_PPT_ADDERS : INTEGER := NUM_ENCODERS - 1;
	CONSTANT ADD_SIZE_LVL_0 : INTEGER := NUM_BITS_A + 3;
	CONSTANT FINAL_ADD_SIZE : INTEGER := NUM_BITS_O - NUM_ENCODERS;

	SIGNAL dec_0_se_n : STD_LOGIC;
	SIGNAL row_lsb    : STD_LOGIC_VECTOR(NUM_ENCODERS - 1 DOWNTO 0);
	SIGNAL x1_b       : STD_LOGIC_VECTOR(NUM_ENCODERS - 1 DOWNTO 0);
	SIGNAL x2_b       : STD_LOGIC_VECTOR(NUM_ENCODERS - 1 DOWNTO 0);
	SIGNAL se         : STD_LOGIC_VECTOR(NUM_ENCODERS - 1 DOWNTO 0);
	SIGNAL z          : STD_LOGIC_VECTOR(NUM_ENCODERS - 1 DOWNTO 0);
	SIGNAL neg_cin    : STD_LOGIC_VECTOR(NUM_ENCODERS - 1 DOWNTO 0);

	TYPE array_cs IS ARRAY (0 TO NUM_PPT_ADDERS - 1) OF STD_LOGIC_VECTOR(FINAL_ADD_SIZE - 1 DOWNTO 0);
	SIGNAL cs_A       : array_cs;
	SIGNAL cs_B       : array_cs;
	SIGNAL cs_Cin     : array_cs;
	SIGNAL cs_O       : array_cs;
	SIGNAL cs_Cout    : array_cs;
	SIGNAL final_O    : STD_LOGIC_VECTOR(FINAL_ADD_SIZE - 1 DOWNTO 0);

	TYPE array_ppts IS ARRAY (0 TO NUM_ENCODERS - 1) OF STD_LOGIC_VECTOR(NUM_BITS_A - 2 DOWNTO 0);
	SIGNAL ppts : array_ppts;
BEGIN
	dec_0_se_n <= NOT se(0);

	gen_encoders: FOR i IN 0 TO (NUM_ENCODERS - 1) GENERATE
		enc0: IF i = 0 GENERATE
			enc : ENTITY work.BoothEncoderRadix4(arch)
			PORT MAP (
				X_2I    => B(0),
				X_2I_M1 => '0',
				X_2I_P1 => B(1),
				Y_LSB   => A(0),
				Y_MSB   => A(NUM_BITS_A - 1),
				ROW_LSB => row_lsb(0),
				X1_b    => x1_b(0),
				X2_b    => x2_b(0),
				SE      => se(0),
				Z       => z(0),
				NEG_CIN => neg_cin(0)
			);
		END GENERATE enc0;

		encs: IF i > 0 GENERATE
			enc : ENTITY work.BoothEncoderRadix4(arch)
			PORT MAP (
				X_2I    => B(i * 2),
				X_2I_M1 => B((i * 2) - 1),
				X_2I_P1 => B((i * 2) + 1),
				Y_LSB   => A(0),
				Y_MSB   => A(NUM_BITS_A - 1),
				ROW_LSB => row_lsb(i),
				X1_b    => x1_b(i),
				X2_b    => x2_b(i),
				SE      => se(i),
				Z       => z(i),
				NEG_CIN => neg_cin(i)
			);
		END GENERATE encs;
	END GENERATE gen_encoders;

	gen_decoders: FOR i IN 0 TO (NUM_ENCODERS - 1) GENERATE
		dec : ENTITY work.Radix4BoothDecoder(arch)
		GENERIC MAP (
			SIZE => NUM_BITS_A
		)
		PORT MAP (
			Yj   => A,
			NEG  => B((i * 2) + 1),
			X1_b => x1_b(i),
			X2_b => x2_b(i),
			Z    => z(i),
			PPTj => ppts(i)
		);
	END GENERATE gen_decoders;

	gen_cs_adders: FOR i IN 0 TO (NUM_PPT_ADDERS - 1) GENERATE
		csa0: IF i = 0 GENERATE
			-- Assign '0' to all unused bits
			cs_A(0)(cs_A(0)'HIGH DOWNTO ADD_SIZE_LVL_0 - 1) <= (others => '0');
			cs_B(0)(cs_B(0)'HIGH DOWNTO ADD_SIZE_LVL_0)     <= (others => '0');
			cs_Cin(0)(cs_Cin(0)'HIGH DOWNTO ADD_SIZE_LVL_0) <= (others => '0');
			cs_Cin(0)(1 DOWNTO 0)                           <= (others => '0');

			-- Assign lower bits
			cs_A(0)  (ADD_SIZE_LVL_0 - 2 DOWNTO 0) <= se(0) & dec_0_se_n & dec_0_se_n & ppts(0);       -- PPT_0
			cs_B(0)  (ADD_SIZE_LVL_0 - 1 DOWNTO 0) <= '1' & se(1) & ppts(1) & row_lsb(1) & neg_cin(0); -- PPT_1
			cs_Cin(0)(ADD_SIZE_LVL_0 - 1 DOWNTO 2) <= ppts(2) & row_lsb(2) & neg_cin(1);               -- PPT_2

			csa : ENTITY work.CarrySaveAdder(arch)
			GENERIC MAP (
				NUM_BITS => FINAL_ADD_SIZE
			)
			PORT MAP (
				A    => cs_A(i),
				B    => cs_B(i),
				Cin  => cs_Cin(i),
				O    => cs_O(i),
				Cout => cs_Cout(i)
			);
		END GENERATE csa0;

		csas: IF (i > 0 AND i < (NUM_PPT_ADDERS - 1)) GENERATE
			-- Assign '0' to all unused bits
            cs_A(i)(cs_A(i)'HIGH DOWNTO ADD_SIZE_LVL_0 + i)     <= (others => '0');
            cs_B(i)(cs_B(i)'HIGH DOWNTO ADD_SIZE_LVL_0 + 1 - 1) <= (others => '0');
            cs_Cin(i)(cs_Cin(i)'HIGH DOWNTO ADD_SIZE_LVL_0 + i) <= (others => '0');
            cs_Cin(i)(i + 1 DOWNTO 0)                           <= (others => '0');

			-- Assign lower bits
			cs_A(i)(ADD_SIZE_LVL_0 + i - 1 DOWNTO 0) <= '1' & se(i + 1) & cs_O(i - 1)(ADD_SIZE_LVL_0 + i - 2 DOWNTO 1); -- cs_O(i - 1)
			cs_B(i)(ADD_SIZE_LVL_0 + i - 2 DOWNTO 0) <= cs_Cout(i - 1)(ADD_SIZE_LVL_0 + i - 2 DOWNTO 0); -- cs_Cout(i - 1)
			cs_Cin(i)(ADD_SIZE_LVL_0 + i - 1 DOWNTO i + 2) <= ppts(i + 2) & row_lsb(i + 2) & neg_cin(i + 1); -- PPT_(i + 2)

			csa : ENTITY work.CarrySaveAdder(arch)
			GENERIC MAP (
				NUM_BITS => FINAL_ADD_SIZE
			)
			PORT MAP (
				A    => cs_A(i),
				B    => cs_B(i),
				Cin  => cs_Cin(i),
				O    => cs_O(i),
				Cout => cs_Cout(i)
			);
		END GENERATE csas;

		final_csa: IF i = (NUM_PPT_ADDERS - 1) GENERATE
			-- Assign '0' to all unused bits
			cs_A(i)(cs_A(i)'HIGH DOWNTO ADD_SIZE_LVL_0 + i)     <= (others => '0');
			cs_Cin(i)(cs_Cin(i)'HIGH DOWNTO i + 3)              <= (others => '0');
			cs_Cin(i)(i + 1 DOWNTO 0)                           <= (others => '0');

			-- Assign lower bits
			cs_A(i)(FINAL_ADD_SIZE - 1 DOWNTO 0) <= se(i + 1) & cs_O(i - 1)(FINAL_ADD_SIZE - 1 DOWNTO 1);
			cs_B(i)(FINAL_ADD_SIZE - 1 DOWNTO 0) <= cs_Cout(i - 1)(FINAL_ADD_SIZE - 1 DOWNTO 0);
			cs_Cin(i)(i + 2) <= neg_cin(i + 1);

			csa : ENTITY work.CarrySaveAdder(arch)
			GENERIC MAP (
				NUM_BITS => FINAL_ADD_SIZE
			)
			PORT MAP (
				A    => cs_A(i),
				B    => cs_B(i),
				Cin  => cs_Cin(i),
				O    => cs_O(i),
				Cout => cs_Cout(i)
			);
		END GENERATE final_csa;
	END GENERATE gen_cs_adders;

	rca : ENTITY work.RippleCarryAdder(arch)
	GENERIC MAP (
		NUM_BITS => FINAL_ADD_SIZE
	)
	PORT MAP (
		A    => cs_O(NUM_PPT_ADDERS - 1),
		B    => cs_Cout(NUM_PPT_ADDERS - 1),
		Cin  => '0',
		O    => final_O,
		Cout => open
	);

	gen_output: FOR i IN 0 TO (NUM_BITS_O - 1) GENERATE
		out_0: IF i = 0 GENERATE
			O(0) <= row_lsb(0);
		END GENERATE out_0;
		out_b: IF (i > 0 AND i < (NUM_BITS_O - FINAL_ADD_SIZE)) GENERATE
			O(i) <= cs_O(i - 1)(0);
		END GENERATE out_b;
		out_e: IF i >= (NUM_BITS_O - FINAL_ADD_SIZE) GENERATE
			O(i) <= final_O(i - (NUM_BITS_O - FINAL_ADD_SIZE));
		END GENERATE out_e;
	END GENERATE gen_output;
END arch;
