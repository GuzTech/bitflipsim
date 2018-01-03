LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.NUMERIC_STD.ALL;

ENTITY BoothEncoderRadix4 IS
PORT (
	X_2I    : IN  STD_LOGIC;
	X_2I_M1 : IN  STD_LOGIC;
	X_2I_P1 : IN  STD_LOGIC;
	Y_LSB   : IN  STD_LOGIC;
	Y_MSB   : IN  STD_LOGIC;
	ROW_LSB : OUT STD_LOGIC;
	X1_b    : OUT STD_LOGIC;
	X2_b    : OUT STD_LOGIC;
	SE      : OUT STD_LOGIC;
	Z       : OUT STD_LOGIC;
	NEG_CIN : OUT STD_LOGIC
);
END BoothEncoderRadix4;

ARCHITECTURE arch OF BoothEncoderRadix4 IS
	SIGNAL neg_cin_nor_1_O : STD_LOGIC;
	SIGNAL neg_cin_nor_2_O : STD_LOGIC;
	SIGNAL neg_cin_nor_3_O : STD_LOGIC;
	SIGNAL neg_cin_or3_O   : STD_LOGIC;
	SIGNAL se_xnor_O       : STD_LOGIC;
	SIGNAL se_nor3_O       : STD_LOGIC;
	SIGNAL se_and3_O       : STD_LOGIC;
	SIGNAL x2_b_O          : STD_LOGIC;
BEGIN
	neg_cin_nor_1_O <= X_2I NOR X_2I_P1;
	neg_cin_nor_2_O <= X_2I_P1 NOR Y_LSB;
	neg_cin_nor_3_O <= X_2I NOR Y_LSB;
	neg_cin_or3_O   <= neg_cin_nor_1_O OR neg_cin_nor_2_O OR neg_cin_nor_3_O;
	NEG_CIN <= neg_cin_or3_O AND X_2I_P1;

	se_xnor_O <= Y_MSB XNOR X_2I_P1;
	se_nor3_O <= NOT (X_2I OR X_2I_M1 OR X_2I_P1);
	se_and3_O <= X_2I AND X_2I_M1 AND X_2I_P1;
	SE <= se_xnor_O OR se_nor3_O OR se_and3_O;

	X1_b <= X_2I_M1 XNOR X_2I;

	x2_b_O <= X_2I_M1 XOR X_2I;
	X2_b <= x2_b_O;

	ROW_LSB <= x2_b_O AND Y_LSB;
END arch;
