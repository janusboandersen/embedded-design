----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 20.06.2020 12:49:42
-- Design Name: 
-- Module Name: tb_top - Behavioral
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
use IEEE.NUMERIC_STD.ALL;


entity tb_top is
end tb_top;

architecture Behavioral of tb_top is
-- Test settings
  constant clock_period         : time := 10 ns;     -- clk period for 100 MHz
  constant CLK_STEPS            : integer := 4;      -- system clock steps between SPI clock steps
  
  -- Intermediate simulation signals
  signal clk                    : std_logic;                     -- simulated clock signal
  signal rst                    : std_logic;                     -- simulated reset

  -- SPI interface signals
  signal sclk                   : std_logic;
  signal ss_n                   : std_logic;
  signal mosi                   : std_logic;
  signal miso                   : std_logic;

  -- LED outputs
  signal led              : std_logic_vector(0 to 11);        -- LEDs on/off states (R0, G0,  . . . , G3, B3)
  
  -- bus signals on master
  signal master_tx            : std_logic_vector(7 downto 0); -- to be transferred to slave, MSB first
  signal master_rx            : std_logic_vector(7 downto 0) := "00000000"; -- will be filled when rx from slave, MSB first
  
  
begin

  -- instantiate full circuit
  dut: entity work.top
    port map(
      clk  => clk,
      rst  => rst,
      sclk => sclk,
      ss_n => ss_n,
      mosi => mosi,
      miso => miso,
      led  => led
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

-- Send address command
    -- assert the slave select
    ss_n <= '0';
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;
    
    -- send and receive bits 7-0
    master_tx <= "00000001"; -- address command: Address line 2
    wait until rising_edge(clk);
    
    for bit in 0 to 7 loop
       
      sclk <= '0';                             -- spi clock low
      wait until rising_edge(clk);
      
      mosi <= master_tx(7 - bit);              -- assert value to send
      for j in 1 to CLK_STEPS loop
        wait until rising_edge(clk);
      end loop;
   
      sclk <= '1';                               -- spi clock high
      master_rx <= master_rx(6 downto 0) & miso; -- sample value being received

      for j in 1 to CLK_STEPS loop
        wait until rising_edge(clk);
      end loop;
      
    end loop;
  
    -- de-assert the slave select
    sclk <= '0';                      -- spi clock low
    wait until rising_edge(clk);
    ss_n <= '1';
    
    for j in 1 to CLK_STEPS loop
        wait until rising_edge(clk);
    end loop;

-- Send data command
    -- assert the slave select
    ss_n <= '0';
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;
    
    -- send and receive bits 7-0
    master_tx <= "11111111"; -- data command: value 255
    wait until rising_edge(clk);
    
    for bit in 0 to 7 loop
       
      sclk <= '0';                             -- spi clock low
      wait until rising_edge(clk);
      
      mosi <= master_tx(7 - bit);              -- assert value to send
      for j in 1 to CLK_STEPS loop
        wait until rising_edge(clk);
      end loop;
   
      sclk <= '1';                               -- spi clock high
      master_rx <= master_rx(6 downto 0) & miso; -- sample value being received

      for j in 1 to CLK_STEPS loop
        wait until rising_edge(clk);
      end loop;
      
    end loop;

    -- de-assert the slave select
    sclk <= '0';                      -- spi clock low
    wait until rising_edge(clk);
    ss_n <= '1';
    
    
    for j in 1 to CLK_STEPS loop
        wait until rising_edge(clk);
    end loop;



-- done sending

    for j in 1 to 40 loop
        wait until rising_edge(clk);
    end loop;

    -- End the simulation
    assert false
      report "Simulation OK!"
      severity failure;
  end process test;  
   

end Behavioral;
