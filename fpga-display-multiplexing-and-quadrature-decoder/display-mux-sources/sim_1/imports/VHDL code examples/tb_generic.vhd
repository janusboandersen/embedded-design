-----------------------------------------------------------------------------
-- Entity: tb_lab3 (testbench for the 7-segment hardware implementation)
-----------------------------------------------------------------------------
-- Filename      : tb_lab3.vhd
-- Creation date : 2019-03-05
-- Last changed  : 2019-03-25
-- Author(s)     : Janus Bo Andersen (Team 2.1)
-- Version       : 3.00
-- Description   : Testbench for full device: 
--               : Purpose: To test full design, integration of modules.
--
--               : Run 7 segment through full 000-FFF.
--               : Possible to set prescaler clock division, otherwise the test
--               : runs 16x 50 million clock cycles (100 MHz clk to 10 Hz count)
--
-- Test goals    : Verify:
--               : - That the reset button works
--               : - That the sseg advances at prescaled clock rate.
--               : - That the Mux stimulates the correct displayss.
-- Target device : Arty A7 35T board (Artix-7)
-----------------------------------------------------------------------------
-- File History:
-- Date        Version   Author    Comment
-- 2019-03-05  0.00      Janus     Create file.
-- 2019-03-05  1.00      Janus     Verify TB is working. Design passes OK.
-- 2019-03-08  2.00      Janus     Use generic mapping for prescaler in top.vhd
--                                 and refactor the test process.
--                                 DUT with generic prescaler passes test.
-- 2019-03-27  3.00      Janus     Update to Lab 3 (compulsory) specs
-----------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;

entity tb_lab3 is
end tb_lab3;

architecture arch of tb_lab3 is

-- Test settings
  constant clock_period      : time := 10 ns;     -- clk period for 100 MHz
  constant settling_time     : time := 100 ps;    -- time for signals to register
  constant PRESCALE_VAL_TEST : integer := 16;     -- prescaler tick on every 16 clocks
  constant PRESCALE_BIT_TEST : integer := 4;
  constant REFRESH_RATE_BITS_TEST : integer := 4; -- switch display every (X-2)^2 => 4 clocks ticks 

-- Dimension constants
  constant SSEG_BITS: integer := 8; --7 segs and 1 dp

-- Intermediate simulation signals
  signal clk    : std_logic; -- simulated clock signal
  signal rst    : std_logic; -- simulated reset
  signal sseg   : std_logic_vector(SSEG_BITS-1 downto 0); -- To pick up SSEG
  signal an     : std_logic_vector(3 downto 0);
  signal q_reg, q_next: unsigned(11 downto 0);  -- Emulate 12-bit system counter
  signal p_reg, p_next: unsigned(PRESCALE_BIT_TEST-1 downto 0); -- Emulate prescaler
  signal tick   : std_logic;

-- Specifications
  -- Array of required SSEG behaviors: output for different hex values (0-15)
  type sseg_array is array (0 to 15) of std_logic_vector(SSEG_BITS-1 downto 0);
  signal sseg_spec : sseg_array := (
  --dp gfedcba  <- segments on the 7 seg.
     "00111111",  -- 0 (3F)
     "00000110",  -- 1 (06) 
     "01011011",  -- 2 (5B)
     "01001111",  -- 3 (4F)
     "01100110",  -- 4 (66)
     "01101101",  -- 5 (6D)
     "01111101",  -- 6 (7D)
     "00000111",  -- 7 (07)
     "01111111",  -- 8 (7F)
     "01101111",  -- 9 (6F)
     "01110111",  -- A (77)
     "01111100",  -- B (7C)
     "00111001",  -- C (39)
     "01011110",  -- D (5E)
     "01111001",  -- E (79)
     "01110001"   -- F (71)
  );

begin
  --Instantiate a full device. Use the whole top structure with gen. prescaler
  dut1 : entity work.top
    generic map(
      SET_PRESCALE_BITS => PRESCALE_BIT_TEST,
      SET_PRESCALE_VAL  => PRESCALE_VAL_TEST,
      SET_REFRESH_RATE_BITS => REFRESH_RATE_BITS_TEST
    )
    port map(
      sseg => sseg,   --output, 7-seg. pattern
      an   => an,     --output, muxing transistors
      clk  => clk,    --input, simulated clock
      rst  => rst     --input, simulated reset
    );

--Generate the clock
  clock: process
  begin
    clk <= '0';
    wait for clock_period / 2;
    clk <= '1';
    wait for clock_period / 2;
  end process clock;

--Emulate the prescaler
  -- Register (4-bit)
  process(clk, rst)
  begin
    if (rst = '1') then 
      p_reg <= (others => '0');
    elsif rising_edge(clk) then
      p_reg <= p_next;
    end if;
  end process;

  -- Next-state logic (4-bit) like mod-M
  p_next <= (others => '0') when p_reg = (PRESCALE_VAL_TEST - 1) else p_reg + 1;

  -- Output logic
  tick <= '1' when p_reg = (PRESCALE_VAL_TEST - 1) else '0';


--Emulate the system counter
  -- Register (12-bit)
  process(clk, rst, tick)
  begin
    if rst = '1' then
      q_reg <= (others => '0');
    elsif rising_edge(clk) and (tick = '1') then  -- Prescaled with EN signal
      q_reg <= q_next;
    end if;
  end process;

  -- Next-state logic (12-bit)
  q_next <= q_reg + 1;


--Test module behavior versus specifications
  test: process
  begin

    --Test the reset button for a full round of the prescaler
    rst <= '1';
    for j in 1 to PRESCALE_VAL_TEST loop
      wait until rising_edge(clk);
    end loop;
    rst <= '0';

    wait for settling_time;

    assert sseg = sseg_spec(0) and an = "0001"
      report "Reset is not resetting internal counters and mux"
      severity failure;

    -- run for all 3 digits of the 3-hex digit counter (the 3x 7-seg displays)
    for digit3 in 0 to 15 loop
      for digit2 in 0 to 15 loop
        for digit1 in 0 to 15 loop

          -- check display #1 (right-most on 4x7-board)
          assert sseg = sseg_spec(digit1) and an = "0001"
            report "Failing test: Display 1. Loop #" & integer'image(digit1) & "." 
            severity failure;

          -- wait for next Mux period (4 clocks + settling time)
          for c in 1 to 4 loop
            wait until rising_edge(clk);
          end loop;
          wait for settling_time;

          -- check display #2
          assert sseg = sseg_spec(digit2) and an = "0010"
            report "Failing test: Display 2. Loop #" & integer'image(digit2) & "." 
            severity failure;

          -- wait for next Mux period (4 clocks + settling time)
          for c in 1 to 4 loop
            wait until rising_edge(clk);
          end loop;
          wait for settling_time;

          -- check display #3
          assert sseg = sseg_spec(digit3)  and an = "0100"
            report "Failing test: Display 3. Loop #" & integer'image(digit3) & "." 
            severity failure;

          -- wait for next Mux period (4 clocks + settling time)
          for c in 1 to 4 loop
            wait until rising_edge(clk);
          end loop;
          wait for settling_time;

          -- check display #4 (left-most on 4x7-board) -> must always be zero and off
          assert sseg = sseg_spec(0)  and an = "0000"
            report "Failing test: Display 4. Loop #" & integer'image(0) & "." 
            severity failure;

          -- wait for next Mux period (4 clocks + settling time) -> to restart loop
          for c in 1 to 4 loop
            wait until rising_edge(clk);
          end loop;
          wait for settling_time;

        end loop;
      end loop;
    end loop;

    --Terminate simulation OK if we reached this point 
    assert false
      report "Simulation OK!"
      severity failure;

  end process test;

end arch;