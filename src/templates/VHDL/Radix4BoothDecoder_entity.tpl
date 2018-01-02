LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.NUMERIC_STD.ALL;

ENTITY Radix4BoothDecoder IS
GENERIC (
	SIZE : INTEGER
);
PORT (
	 Yj   : IN  STD_LOGIC_VECTOR(SIZE - 1 DOWNTO 0);
	 NEG  : IN  STD_LOGIC;
	 X1_b : IN  STD_LOGIC;
	 X2_b : IN  STD_LOGIC;
	 Z    : IN  STD_LOGIC;
	 PPTj : OUT STD_LOGIC_VECTOR(SIZE - 2 DOWNTO 0)
);
END Radix4BoothDecoder;

ARCHITECTURE arch OF Radix4BoothDecoder IS
	SIGNAL xj_neg_O : STD_LOGIC_VECTOR(SIZE - 1 DOWNTO 0);
	SIGNAL xj_or_O  : STD_LOGIC_VECTOR(SIZE - 1 DOWNTO 1);
	SIGNAL xj_or3_O : STD_LOGIC_VECTOR(SIZE - 2 DOWNTO 0);
BEGIN
	gen_xnors: FOR i IN 0 TO SIZE - 1 GENERATE
		xj_neg_O(i) <= Yj(i) XNOR NEG;
	END GENERATE gen_xnors;

	gen_ors: FOR i IN 1 TO SIZE - 1 GENERATE
		xj_or_O(i) <= xj_neg_O(i) OR X1_b;
	END GENERATE gen_ors;

	gen_or3s: FOR i IN 0 TO SIZE - 2 GENERATE
		xj_or3_O(i) <= xj_neg_O(i) OR Z OR X2_b;
	END GENERATE gen_or3s;

	gen_ppt: FOR i IN 0 TO SIZE - 2 GENERATE
		PPTj(i) <= xj_or_O(i + 1) NAND xj_or3_O(i);
	END GENERATE gen_ppt;
END arch;
