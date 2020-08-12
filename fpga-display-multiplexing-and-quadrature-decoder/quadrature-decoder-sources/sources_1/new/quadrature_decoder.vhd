-----------------------------------------------------------------------------
-- Entity: quadrature_decoder (for the lab 3 extra assign. quadrature decoder)
-----------------------------------------------------------------------------
-- Filename      : top_lab3_extra.vhd
-- Creation date : 2019-03-30
-- Last changed  : 2019-03-30
-- Author(s)     : Janus Bo Andersen (Team 2.1)
-- Version       : 1.00
-- Description   : Decodes the quadrature from two phase-shifted square signals
--               : The signals are active low (go low on rotation events) 
-- Target device : Arty A7 35T board (Artix-7)
-----------------------------------------------------------------------------
-- File History:
-- Date        Version   Author    Comment
-- 2019-03-30  0.00      Janus     Creation of file
-- 2019-03-30  1.00      Janus     Implemented decoder
-----------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity quadrature_decoder is
  Port ( 
    clk    : in STD_LOGIC;       -- system clock
    rst    : in std_logic;       -- system reset
    rot_a  : in STD_LOGIC;       -- rotary signal A (CLK), already debounced
    rot_b  : in STD_LOGIC;       -- rotary signal B (DT), already debounced
    event_tick : out STD_LOGIC;   -- tick for rotation event
    event_dir  : out STD_LOGIC    -- indicate rot. dir.: CW: '1'; CCW: '0'.
  );  
end quadrature_decoder;

-- Method: Sample the rotary signal on each rising clock edge.
-- Register previous and current signals, usual synchronous method.
-- Total state is 4-bit sig. that relates to the report's transitions table.
-- Events can be determined fully from the 4-bit state signal.

architecture arch of quadrature_decoder is
signal state_reg, state_next : std_logic_vector(3 downto 0); -- total state (prev & current)

begin

-- Register
process (clk, rst)
begin
  if rst = '1' then
    --reset signal (active low)
    state_reg <= "1111";

  elsif rising_edge(clk) then
    --update state
    state_reg <= state_next;

  end if;
end process;

-- Next state logic
state_next <= state_reg(1 downto 0) & rot_a & rot_b;  -- shift in new values

-- output logic
process(state_reg)
begin
  case state_reg is
  when "0100" =>  -- CW on falling edge for signal B
                  -- Other possibilities "0010"|"1011"|"0100"|"1101"
    event_tick <= '1';
    event_dir <= '1';   -- CW rotation
  when "1000" =>  -- CCW on falling edge for signal A
                  -- Other possibilities "0001"|"0111"|"1000"|"1110"
    event_tick <= '1';
    event_dir <= '0';   -- CCW rotation
  when others =>        -- Ignored states, N/C states, indeterminate states
    event_tick <= '0';
    event_dir <= '0';
  end case;
end process;

end arch;