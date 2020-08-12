-----------------------------------------------------------------------------
-- Entity: top (for the lab 3 quadrature decoder)
-----------------------------------------------------------------------------
-- Filename      : top_lab3_extra.vhd
-- Creation date : 2019-03-30
-- Last changed  : 2019-03-30
-- Author(s)     : Janus Bo Andersen (Team 2.1)
-- Version       : 1.00
-- Description   : Top-level instantiator for quadrature decoder and 3x7 system
-- Target device : Arty A7 35T board (Artix-7)
-----------------------------------------------------------------------------
-- File History:
-- Date        Version   Author    Comment
-- 2019-03-30  0.00      Janus     Creation of file
-- 2019-03-30  1.00      Janus     Swapped decoder in instead of prescaler
-----------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity top_lab3_extra is
  generic(
    DP               : std_logic_vector(3 downto 0) := "0000"; -- no decimal points
    SET_REFRESH_RATE_BITS : integer := 18 -- use only the 2 MSBs, updating at 100MHz/2^16
  );
  Port (
        clk : in std_logic;
        rst : in std_logic;
        enc_io : in STD_LOGIC_VECTOR (1 downto 0); --get 2-signal quadrature
        sseg : out std_logic_vector(7 downto 0); -- 8 bits: 7 segments and 1 dp
        an   : out std_logic_vector(3 downto 0) -- 4 sigs to enable transistors     
  );
end top_lab3_extra;

architecture top_arch of top_lab3_extra is
constant CNT_BITS : integer   := 12;        -- 12 bits for the counter (000-FFF)
constant OFF_MODE : std_logic := '0';

--Intermediate signals
signal tick    : std_logic;               -- maps decoder tick to counter enable
signal q_to_mux: std_logic_vector(CNT_BITS-1 downto 0); -- connects 12-bit counter to mux
signal db_a_level, db_b_level : std_logic;  --sigs for debounced rotary waveforms
signal dir : std_logic;  -- indicates direction of rotation (CW: '1'; CCW: '0')

begin

  -- Debounce signal A. (Chu listing 6.1)
  db_a_unit : entity work.debounce
    generic map (
        n => 16
    )
    port map(
      clk      => clk,
      reset    => rst,
      sw       => enc_io(0),   -- directly from rotary encoder, sig. A
      db_level => db_a_level,  -- stable signal
      db_tick  => open
    );

  -- Debounce signal B. Chu listing 6.1
  db_b_unit : entity work.debounce
    generic map (
        n => 16
    )
    port map(
      clk      => clk,
      reset    => rst,
      sw       => enc_io(1),    -- directly from rotary encoder, sig. B
      db_level => db_b_level,   -- stable signal
      db_tick  => open
    );
    
  -- decode the debounced quadrature (own work)
    decoder_unit : entity work.quadrature_decoder
      port map(
        rot_a      => db_a_level,  -- debounced signal A
        rot_b      => db_b_level,  -- debounced signal B
        clk        => clk,
        rst        => rst,
        event_tick => tick,  -- goes high on rotation event
        event_dir  => dir    -- direction of rotation (CW: '1'; CCW: '0')
      );

--binary counter (universal binary counter, Chu listing 4.9)
    counter : entity work.univ_bin_counter
    generic map(
      N => CNT_BITS         -- 12-bit (3 hex digits)
    )
    port map(
      clk      => clk,      
      reset    => rst,      
      syn_clr  => OFF_MODE, 
      load     => OFF_MODE, 
      en       => tick,     -- event tick from quadrature decoder
      up       => dir,      -- direction of rotation (CW '1': +1; CCW '0': -1)
      d        => (others => '0'),
      max_tick => open,
      min_tick => open,
      q        => q_to_mux  -- get 12-bit count here
    );


  -- Mux'ed 7-segment decoder (lab 3 custom)
  mux_hex_decoder : entity work.mux_hex_decoder
    generic map(
      REFRESH_RATE_BITS => SET_REFRESH_RATE_BITS
    )
    port map(
      clk    => clk,
      reset  => rst,
      mux_in => q_to_mux, -- 12 bits in (3 hex digits to be mux'ed)
      dp_in  => DP,
      sseg   => sseg,     -- 7-segment patterns out (incl. dp)
      an     => an        -- transistor mux'ing
    );

end top_arch;
