{{NAME}} : ENTITY work.Multiplier_Smag({{ARCH}})
GENERIC MAP (
	NUM_BITS_A => {{NUM_BITS_A}},
	NUM_BITS_B => {{NUM_BITS_B}}
)
PORT MAP (
	A => {{A_WIRE}},
	B => {{B_WIRE}},
	O => {{O_WIRE}}
);
