-----------------------------------------------------------------------------
-- Entity: testbench for the lab 3 quadrature decoder
-----------------------------------------------------------------------------
-- Filename      : tb_lab3_extra.vhd
-- Creation date : 2019-03-30
-- Last changed  : 2019-03-30
-- Author(s)     : Janus Bo Andersen (Team 2.1)
-- Version       : 1.00
-- Description   : Synchronous testbench
-- Target device : Arty A7 35T board (Artix-7)
-----------------------------------------------------------------------------
-- File History:
-- Date        Version   Author    Comment
-- 2019-03-30  0.00      Janus     Creation of file
-- 2019-03-30  1.00      Janus     Test of decoder circuit passed!
-----------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity tb_lab3_extra is
end tb_lab3_extra;

architecture tb_arch of tb_lab3_extra is
-- Test settings
  constant clock_period      : time := 10 ns;     -- clk period for 100 MHz
--  constant settling_time     : time := 100 ps;    -- time for signals to register
  
  -- Intermediate simulation signals
  signal clk    : std_logic; -- simulated clock signal
  signal rst    : std_logic; -- simulated reset
  
  -- Test signals
  signal ab : std_logic_vector(1 downto 0);    --test signal to emulate AB
  signal a, b : std_logic;
  signal event_tick   : std_logic;
  signal event_dir    : std_logic;
  
  
begin
  
-- Instantiate the decoder

dut1: entity work.quadrature_decoder
  port map(
    clk        => clk,
    rst        => rst,
    rot_a      => a,
    rot_b      => b,
    event_tick => event_tick,
    event_dir  => event_dir
  );

a <= ab(1);
b <= ab(0);

-- Generate the clock
  clock: process
  begin
    clk <= '0';
    wait for clock_period / 2;
    clk <= '1';
    wait for clock_period / 2;
  end process clock;
  

  test : process
  begin
    -- Stimulate the decoder
    
    -- Active low
    ab <= "11";
    
    --Test the reset button for a while
    rst <= '1';
    wait until rising_edge(clk);
    wait until rising_edge(clk);
    rst <= '0';

    -- Input "0100" transition (regular rotation, direction '1')
    ab <= "01";
    wait until rising_edge(clk);
    ab <= "00";
    wait until rising_edge(clk);
    
    -- Go back to inactive
    ab <= "11";
    wait until rising_edge(clk);
    wait until rising_edge(clk);
    
    -- Input "1000" transition (regular rotation, direction '0')
    ab <= "10";
    wait until rising_edge(clk);
    ab <= "00";
    wait until rising_edge(clk);
    
    -- Go back to inactive
    ab <= "11";
    wait until rising_edge(clk);
    wait until rising_edge(clk);

    -- Input "0100" transition (prolonged rotation, direction '1')
    ab <= "01";
    wait until rising_edge(clk);
    ab <= "00";
    wait until rising_edge(clk);
    wait until rising_edge(clk);
    wait until rising_edge(clk);


    -- End the simulation
    assert false
      report "Simulation OK!"
      severity failure;
  end process test;

end tb_arch;
