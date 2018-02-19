LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.NUMERIC_STD.ALL;
USE STD.TEXTIO.ALL;
USE IEEE.STD_LOGIC_TEXTIO.ALL;

ENTITY {{TOP_NAME}}_tb IS
END {{TOP_NAME}}_tb;

ARCHITECTURE tvc OF {{TOP_NAME}}_tb IS
	SIGNAL i_clk : STD_LOGIC := '0';
	SIGNAL i_rst : STD_LOGIC := '1';
	{{SIGNALS}}

	CONSTANT clk_period : TIME := 1us;
	FILE stim_file : text;
	FILE exp_file : text;
BEGIN
----- SIGNAL ASSIGNMENTS -----
{{ASSIGNMENTS}}

----- ENTITY INSTANTIATIONS -----
{{INSTANCES}}

----- PROCESSES -----
    -- Clocked process
	clk_process: PROCESS
	BEGIN
		i_clk <= '0';
		WAIT FOR clk_period / 2;
		i_clk <= '1';
		WAIT FOR clk_period / 2;
	END PROCESS;

	-- Stimulus process
	stim_process: PROCESS
		VARIABLE rdline : LINE;
		VARIABLE exline : LINE;
		VARIABLE first : STD_LOGIC := '1';
		{{VARIABLES}}
	BEGIN
		-- Hold reset for 2 clock cycles.
		WAIT FOR 2 * clk_period;
		i_rst <= '0';

		-- Insert stimulus
		FILE_OPEN(stim_file, "stim_file.txt", READ_MODE);
		FILE_OPEN(exp_file, "expected_output.txt", READ_MODE);
		WHILE NOT ENDFILE(stim_file) LOOP
			READLINE(stim_file, rdline);
			{{READ_STIMULI}}
			{{ASSIGN_STIMULI}}
			WAIT FOR clk_period;

			IF (NOT ENDFILE(exp_file)) THEN
				READLINE(exp_file, exline);
				{{EXP_STIMULI}}

				IF (first = '1') THEN
					first := '0';
				ELSE
					{{ASSERT_STIMULI}}
				END IF;

				{{UPDATE_PREV_STIMULI}}
			END IF;
		END LOOP;

		WAIT FOR clk_period;
		ASSERT FALSE REPORT "Simulation ended" SEVERITY FAILURE;
	END PROCESS;
END tvc;
