{{NAME}} : work.RippleCarryAdder
GENERIC MAP (
    NUM_BITS => {{SIZE}}
);
PORT MAP (
    A => i_{{NAME}}_A,
    B => i_{{NAME}}_B,
    Cin => i_{{NAME}}_Cin,
    O => o_{{NAME}}_O,
    Cout => o_{{NAME}}_O
);
