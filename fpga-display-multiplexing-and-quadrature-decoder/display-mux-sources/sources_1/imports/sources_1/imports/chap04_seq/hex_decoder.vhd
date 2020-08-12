-----------------------------------------------------------------------------
-- Entity: mux_hex_decoder (for the 4x7-board)
-----------------------------------------------------------------------------
-- Filename      : mux_hex_decoder.vhd
-- Creation date : 2019-03-03
-- Last change   : 2019-03-26
-- Author(s)     : Janus Bo Andersen (Team 2.1)
-- Version       : 3.00
-- Description   : Hex-to-seven segment decoder (active high common cathode) 
--               : Multiplexing 3 displays with 33% duty cycle.
-- Target device : Arty A7 35T board (Artix-7)
-----------------------------------------------------------------------------
-- File History:
-- Date        Version   Author    Comment
-- 2019-03-03  0.00      Janus     Creation of file
-- 2019-03-03  0.80      Janus     Concat. dp & sseg signal to avoid a latch
-- 2019-03-03  0.90      Janus     Successfully implemented dec. on Arty board
-- 2019-03-05  1.00      Janus     Design passes testbench (integration) OK.
-- 2019-03-08  2.00      Janus     Refactored. Clarified comments. TB OK!
-- 2019-03-26  3.00      Janus     Added time-multiplexing
-----------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity mux_hex_decoder is
  generic (
    REFRESH_RATE_BITS : integer := 18 -- use only the 2 MSBs, updating at 100MHz/2^16
  );
  port(
    clk, reset  : in  std_logic;
    mux_in      : in  std_logic_vector(11 downto 0);  -- 3 x 4-bit digits (12 bit total), either hex or BCD
    dp_in       : in  std_logic_vector(3 downto 0);   -- decimal point
    sseg        : out std_logic_vector(7 downto 0);   -- 7seg pattern signals (incl. dp)
    an          : out std_logic_vector(3 downto 0)
  );
end mux_hex_decoder;

architecture mux_arch of mux_hex_decoder is
-- For 7-seg decoding:
signal hex : std_logic_vector(3 downto 0);  -- 4 bits to be decoded to patterns
signal dp  : std_logic;                     -- single bit decimal point signal

-- For multiplexing:
constant N    : integer := REFRESH_RATE_BITS;
signal q_reg  : unsigned(N-1 downto 0);       -- 18-bit counter
signal q_next : unsigned(N-1 downto 0);       -- 18-bit next-state
signal sel    : std_logic_vector(1 downto 0); -- 2 MSBs are placed for disp sel

begin

  -- Register (18-bit)
  process(clk, reset)
  begin
    if reset = '1' then
      q_reg <= (others => '0');
    elsif rising_edge(clk) then
      q_reg <= q_next;
    end if;
  end process;

  -- Next-state logic (18-bit)
  q_next <= q_reg + 1;

  -- Selector for mux (casting 2 MSBs into sel)
  sel <= std_logic_vector(q_reg(N-1 downto N-2));

  -- Mux process - 3 h/w multiplexers in one, like in report figures
  process(sel, mux_in, dp_in)
  begin
    case sel is
    when "00" =>
      an <= "0001";
      hex <= mux_in(3 downto 0);
      dp <= dp_in(0);
    when "01" =>
      an <= "0010";
      hex <= mux_in(7 downto 4);
      dp <= dp_in(1);
    when "10" =>
      an <= "0100";
      hex <= mux_in(11 downto 8);
      dp <= dp_in(2);
    when others =>
      an <= "0000";  -- all displays off
      hex <= "0000";
      dp <= '0';
    end case;
  end process;

  -- Output logic (decoder routing circuit)	
  -- Note: CC disp on board are active high. Avoids a latch by full assignment.
  with hex select 
    sseg <=
    -- dp    gfedcba segm., binary counter value
       dp & "0111111" when "0000",  -- 0
       dp & "0000110" when "0001",  -- 1
       dp & "1011011" when "0010",  -- 2
       dp & "1001111" when "0011",  -- 3
       dp & "1100110" when "0100",  -- 4
       dp & "1101101" when "0101",  -- 5
       dp & "1111101" when "0110",  -- 6
       dp & "0000111" when "0111",  -- 7
       dp & "1111111" when "1000",  -- 8
       dp & "1101111" when "1001",  -- 9
       dp & "1110111" when "1010",  -- A
       dp & "1111100" when "1011",  -- B
       dp & "0111001" when "1100",  -- C
       dp & "1011110" when "1101",  -- D
       dp & "1111001" when "1110",  -- E
       dp & "1110001" when others;  -- F

end mux_arch;