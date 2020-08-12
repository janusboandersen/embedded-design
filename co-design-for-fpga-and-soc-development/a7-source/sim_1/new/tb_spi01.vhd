----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 16.06.2020 04:51:29
-- Design Name: 
-- Module Name: tb_spi01 - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity tb_spi01 is
end tb_spi01;

architecture tb_arch of tb_spi01 is
-- Test settings
  constant clock_period         : time := 10 ns;     -- clk period for 100 MHz
--  constant settling_time      : time := 100 ps;    -- time for signals to register
  constant CLK_STEPS            : integer := 4;      -- system clock steps between SPI clock steps
  
  -- Intermediate simulation signals
  signal clk        : std_logic;                     -- simulated clock signal
  signal rst        : std_logic;                     -- simulated reset
  
  -- Test signals
  signal slave_tx         : std_logic_vector(7 downto 0) := "01101100";  -- will be transferred to the master, MSB first
  signal slave_rx         : std_logic_vector(7 downto 0);
  signal ready            : std_logic;
  signal spi_done_tick    : std_logic;
  
  -- bus signals on master
  signal master_tx            : std_logic_vector(7 downto 0) := "10010011"; -- to be transferred to slave, MSB first
  signal master_rx            : std_logic_vector(7 downto 0) := "00000000"; -- will be filled when rx from slave, MSB first
  
  -- SPI signals
  signal sclk             : std_logic;
  signal ss_n             : std_logic;
  signal mosi             : std_logic;
  signal miso             : std_logic;

begin
  
  --instantiate the spi slave
  dut1: entity work.spi_slave
    port map(
      clk           => clk,
      reset         => rst,
      dtx           => slave_tx,
      drx           => slave_rx,
      ready         => ready,
      spi_done_tick => spi_done_tick,
      sclk          => sclk,
      ss_n          => ss_n,
      mosi          => mosi,
      miso          => miso
    );
    

-- Generate the system clock
  clock: process
  begin
    clk <= '0';
    wait for clock_period / 2;
    clk <= '1';
    wait for clock_period / 2;
  end process clock;
  

-- Begin test transfer of 8 bits 
  test : process
  begin
    
    -- initialize
    rst <= '0';
    ss_n <= '1';
    sclk <= '0';
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;
    
    -- test the reset button for a while
    rst <= '1';
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;
    
    rst <= '0';
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;

    -- assert the slave select
    ss_n <= '0';
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;
    
    -- send and receive bits 7-0
    for bit in 0 to 7 loop
       
      sclk <= '0';                             -- spi clock low
      mosi <= master_tx(7 - bit);              -- assert value to send
      for j in 1 to CLK_STEPS loop
        wait until rising_edge(clk);
      end loop;

      
      sclk <= '1';                               -- spi clock high
      --master_rx(7 downto 0) <= "00000000";
      master_rx <= master_rx(6 downto 0) & miso; -- sample value being received

      for j in 1 to CLK_STEPS loop
        wait until rising_edge(clk);
      end loop;
      
    end loop;
  
 
    -- de-assert the slave select
    sclk <= '0';                      -- spi clock low
    ss_n <= '1';
    for j in 1 to CLK_STEPS loop
        wait until rising_edge(clk);
    end loop;

    for j in 1 to 40 loop
        wait until rising_edge(clk);
    end loop;

    -- End the simulation
    assert false
      report "Simulation OK!"
      severity failure;
  end process test;  
  
end tb_arch;