-----------------------------------------------------------------------------
-- Entity: top_lab3 (for the 4x7-segment hardware implementation)
-----------------------------------------------------------------------------
-- Filename      : top.vhd
-- Creation date : 2019-03-03
-- Last changed  : 2019-03-26
-- Author(s)     : Janus Bo Andersen (Team 2.1)
-- Version       : 3.00
-- Description   : Top level instantiatior and wrapper for  
--               : 7-segment decoders and related counters
-- Target device : Arty A7 35T board (Artix-7)
-----------------------------------------------------------------------------
-- File History:
-- Date        Version   Author    Comment
-- 2019-03-03  0.00      Janus     Creation of file
-- 2019-03-03  0.90      Janus     Design successfully implemented on board
-- 2019-03-05  0.91      Janus     Changed hardcoded values to constants
-- 2019-03-05  1.00      Janus     Design passes testbench (integration) OK.
-- 2019-03-08  1.10      Janus     Assign max_tick -> enable directly
--                                 without waiting for a falling_edge.
-- 2019-03-08  1.20      Janus     Implemented generic prescaling in top
-- 2019-03-08  2.00      Janus     Refactored code. Passes TB.
-- 2019-03-26  3.00      Janus     Updated to Lab 3 specs: 3 display mux
-----------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

--create ports that map to hardware pins on the A7, or can be controlled from testbench
entity top is
  generic(
    SET_PRESCALE_BITS : integer := 24;        -- bits to count to 10*10**6
    SET_PRESCALE_VAL  : integer := 10*10**6;  -- mod M: to divide 100 MHz to 10Hz
    DP                : std_logic_vector(3 downto 0) := "0000"; -- no decimal points
    SET_REFRESH_RATE_BITS : integer := 18;    -- use only the 2 MSBs, updating at 100MHz/2^16
    SET_COUNTER_BITS 		: integer := 12				-- bitwidth of the central system counter
  );

  port (
    sseg: out std_logic_vector(7 downto 0);   -- 8 bits: 7 segments and 1 dp
    an  : out std_logic_vector(3 downto 0);   -- 4 sigs to enable transistors
    clk : in std_logic;                       -- raw clock from crystal oscillator
    rst : in std_logic                        -- hard reset
  );
end top;


architecture top_arch of top is
constant CNT_BITS : integer   := SET_COUNTER_BITS;      -- 12 bits for the counter (000-FFF)
constant OFF_MODE : std_logic := '0';
constant ON_MODE  : std_logic := '1';

--Intermediate signals
signal tick    : std_logic;                             -- maps max_tick to enable
signal q_to_mux: std_logic_vector(CNT_BITS-1 downto 0); -- connects 12-bit counter to mux

begin
  --prescaler (mod-m counter, Chu listing 4.10)
  prescaler : entity work.mod_m_counter
    generic map(
      N => SET_PRESCALE_BITS,
      M => SET_PRESCALE_VAL
    )
    port map(
      clk      => clk,  -- system clock
      reset    => rst,  -- active high
      max_tick => tick, -- send max_tick -> tick -> en 
      q        => open  -- don't need these, just discard
    );

    --binary counter (universal binary counter, Chu listing 4.9)
    counter : entity work.univ_bin_counter
    generic map(
      N => CNT_BITS         -- 12-bit (3 hex digits)
    )
    port map(
      clk      => clk,      -- system clock
      reset    => rst,      -- active high
      syn_clr  => OFF_MODE, -- active high
      load     => OFF_MODE, -- active high
      en       => tick,     -- enable is on at 10 Hz
      up       => ON_MODE,  -- always increment
      d        => (others => '0'),
      max_tick => open,
      min_tick => open,
      q        => q_to_mux  -- get 12-bit count here
    );

  -- Mux'ed 7-segment decoder (lab 3 custom)
  -- Does not yet allow a different counter width than 12 bits
  mux_hex_decoder : entity work.mux_hex_decoder
    generic map(
      REFRESH_RATE_BITS => SET_REFRESH_RATE_BITS
    )
    port map(
      clk    => clk,
      reset  => rst,
      mux_in => q_to_mux, -- 12 bits in (3 hex digits or 3 BCDs that will be mux'ed)
      dp_in  => DP,
      sseg   => sseg,     -- 7-segment patterns out (incl. dp)
      an     => an        -- transistor mux'ing
    );

end top_arch;