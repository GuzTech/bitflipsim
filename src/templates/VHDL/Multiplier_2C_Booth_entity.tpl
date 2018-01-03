LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.NUMERIC_STD.ALL;

ENTITY Multiplier_2C_Booth IS
GENERIC (
	NUM_BITS_A : INTEGER := 4;
	NUM_BITS_B : INTEGER := 4
);
PORT (
	 A : IN  STD_LOGIC_VECTOR(NUM_BITS_A - 1 DOWNTO 0);
	 B : IN  STD_LOGIC_VECTOR(NUM_BITS_B - 1 DOWNTO 0);
	 O : OUT STD_LOGIC_VECTOR(NUM_BITS_A + NUM_BITS_B - 1 DOWNTO 0)
);
END Multiplier_2C_Booth;

ARCHITECTURE arch OF Multiplier_2C_Booth IS
	CONSTANT NUM_ENCODERS   : INTEGER := (NUM_BITS_B + 1) / 2;
	CONSTANT NUM_PPT_ADDERS : INTEGER := NUM_ENCODERS - 1;

	SIGNAL dec_0_se_n : STD_LOGIC;
	SIGNAL row_lsb    : STD_LOGIC_VECTOR(NUM_ENCODERS - 1 DOWNTO 0);
	SIGNAL x1_b       : STD_LOGIC_VECTOR(NUM_ENCODERS - 1 DOWNTO 0);
	SIGNAL x2_b       : STD_LOGIC_VECTOR(NUM_ENCODERS - 1 DOWNTO 0);
	SIGNAL se         : STD_LOGIC_VECTOR(NUM_ENCODERS - 1 DOWNTO 0);
	SIGNAL z          : STD_LOGIC_VECTOR(NUM_ENCODERS - 1 DOWNTO 0);
	SIGNAL neg_cin    : STD_LOGIC_VECTOR(NUM_ENCODERS - 1 DOWNTO 0);

	TYPE array_ppts IS ARRAY (0 TO NUM_ENCODERS - 1) OF STD_LOGIC_VECTOR(NUM_ENCODERS - 1 DOWNTO 0);
	SIGNAL ppts : array_ppts;
BEGIN
	gen_encoders: FOR i IN 0 TO NUM_ENCODERS GENERATE
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

	gen_decoders: FOR i IN 0 TO NUM_ENCODERS GENERATE
		dec : ENTITY work.Radix4BoothDecoder(arch)
		PORT MAP (
			Yj   => A,
			NEG  => B((i * 2) + 1),
			X1_b => x1_b(i),
			X2_b => x2_b(i),
			Z    => z(i),
			PPTj => ppts(i)
		);
	END GENERATE gen_decoders;
END arch;
