----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 20.06.2020 11:08:41
-- Design Name: 
-- Module Name: tb_decoder_sync - Behavioral
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


entity tb_decoder_sync is
end tb_decoder_sync;

architecture Behavioral of tb_decoder_sync is
  -- Test settings
  constant clock_period           : time := 10 ns;            -- clk period for 100 MHz
  constant CLK_STEPS              : integer := 4;             -- system clock steps between SPI clock steps
  
    -- Intermediate simulation signals
  signal clk            : std_logic;                        -- simulated clock signal
  signal rst            : std_logic;                        -- simulated reset
  
  -- I/O
  signal D_ADDR   : std_logic_vector(7 downto 0);         -- Numeric address value (into 8-to-12 decoder)
  signal EN       : std_logic;                            -- Enable signal when data is good
  signal Y        : std_logic_vector(0 to 11);            -- Decoded address line (out of 8-to-12 decoder)
  
begin

  -- instantiate dut
  dut4: entity work.decoder_sync
    port map(
      clk    => clk,
      rst    => rst,
      D_ADDR => D_ADDR,
      EN     => EN,
      Y      => Y
    );


-- Generate the system clock
  clock: process
  begin
    clk <= '0';
    wait for clock_period / 2;
    clk <= '1';
    wait for clock_period / 2;
  end process clock;


-- Begin test 
  test : process
  begin
    
    -- initialize
    rst           <= '0';
    D_ADDR        <= "00000000"; 
    EN            <= '0'; 
    
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
    
    -- Try address line 1
    D_ADDR        <= "00000000"; 
    EN            <= '1';
    wait until rising_edge(clk);
    EN            <= '0';
        
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;

    -- Try address line 2
    D_ADDR        <= "00000001"; 
    EN            <= '1';
    wait until rising_edge(clk);
    EN            <= '0';
        
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;

    -- Try address line 3
    D_ADDR        <= "00000010"; 
    EN            <= '1';
    wait until rising_edge(clk);
    EN            <= '0';
        
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;

    -- Try address line 4
    D_ADDR        <= "00000011"; 
    EN            <= '1';
    wait until rising_edge(clk);
    EN            <= '0';
        
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;

    -- Try address line 5
    D_ADDR        <= "00000100"; 
    EN            <= '1';
    wait until rising_edge(clk);
    EN            <= '0';
        
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;

    -- Try address line 6
    D_ADDR        <= "00000101"; 
    EN            <= '1';
    wait until rising_edge(clk);
    EN            <= '0';
        
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;

    -- Try address line 7
    D_ADDR        <= "00000110"; 
    EN            <= '1';
    wait until rising_edge(clk);
    EN            <= '0';
        
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;

    -- Try address line 8
    D_ADDR        <= "00000111"; 
    EN            <= '1';
    wait until rising_edge(clk);
    EN            <= '0';
        
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;

    -- Try address line 9
    D_ADDR        <= "00001000"; 
    EN            <= '1';
    wait until rising_edge(clk);
    EN            <= '0';
        
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;

    -- Try address line 10
    D_ADDR        <= "00001001"; 
    EN            <= '1';
    wait until rising_edge(clk);
    EN            <= '0';
        
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;

    -- Try address line 11
    D_ADDR        <= "00001010"; 
    EN            <= '1';
    wait until rising_edge(clk);
    EN            <= '0';
        
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;

    -- Try address line 12
    D_ADDR        <= "00001011"; 
    EN            <= '1';
    wait until rising_edge(clk);
    EN            <= '0';
        
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;
    
    -- Wait a while before terminating simulation
        
    for j in 1 to 10*CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;

    -- End the simulation
    assert false
      report "Simulation OK!"
      severity failure;
  end process test;  


end Behavioral;
