{{NAME}} : ENTITY work.Radix4BoothDecoder
GENERIC MAP (
	SIZE => {{SIZE}}
)
PORT MAP (
	Yj   => {{Yj_WIRE}},
	NEG  => {{NEG_WIRE}},
	X1_b => {{X1_b_WIRE}},
	X2_b => {{X2_b_WIRE}},
	Z    => {{Z_WIRE}},
	PPTj => {{PPTj_WIRE}}
);
