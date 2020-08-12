----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Janus Bo Andersen
-- 
-- Create Date: 05.2020
-- Design Name: 
-- Module Name: Testbench for pwm12
-- Project Name: Co-design
-- Target Devices: 
-- Tool Versions: 
-- Description: Test the PWM to control 4 RGB LEDs (12 registers) 
-- Based on file from Morten Opprud
-- Dependencies: 
-- 
-- Additional Comments:
-- 
----------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;


entity tb_pwm12 is
end tb_pwm12;

architecture Behavioral of tb_pwm12 is
  
  -- Test settings
  constant clock_period           : time := 10 ns;            -- clk period for 100 MHz
  --constant settling_time          : time := 100 ps;         -- time for signals to register
  constant CLK_STEPS              : integer := 4;             -- system clock steps between SPI clock steps
  
  -- Intermediate simulation signals
  signal clk            : std_logic;                        -- simulated clock signal
  signal rst            : std_logic;                        -- simulated reset

  -- simulated input from previous ckt
  signal addr_decoded   : std_logic_vector(0 to 11);        -- Decoded address value (from 8-to-12 decoder)
  signal data           : std_logic_vector(1 downto 0);     -- Received duty cycle value (from SPI) 

  -- simulated output from this tb
  signal leds           : std_logic_vector(0 to 11);        -- LEDs on/off states (R0, G0,  . . . , G3, B3)
  signal tick           : std_logic;                        -- monitor when the counter overflows

begin

-- instantiate the PWM

dut3: entity work.pwm12
  generic map(
      N => 4,
      DV => 2
      
    )
    port map(
      clk  => clk,
      rst  => rst,
      duty => data,
      col  => addr_decoded,
      led  => leds,
      max_tick => tick
    );


-- Generate the system clock
  clock: process
  begin
    clk <= '0';
    wait for clock_period / 2;
    clk <= '1';
    wait for clock_period / 2;
  end process clock;



-- Begin test store adress byte and data byte 
  test : process
  begin
    
    -- initialize
    rst           <= '0';
    addr_decoded  <= "000000000000";
    data          <= "00";
    
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
    
    
    -- LD0
    -- set R0 to minimal brightness
    data          <= "01";
    addr_decoded  <= "100000000000";      -- enable register for R0
    wait until rising_edge(clk);
    addr_decoded  <= "000000000000";
        
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;
    
    -- set G0 to medium brightness
    data          <= "10";
    addr_decoded  <= "010000000000";      -- enable register for G0
    wait until rising_edge(clk);
    addr_decoded  <= "000000000000";
        
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;
    
    -- set B0 to full brightness
    data          <= "11";
    addr_decoded  <= "001000000000";      -- enable register for B0
    wait until rising_edge(clk);
    addr_decoded  <= "000000000000";
    
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;
    
    
    -- LD1
    -- set R1 to max brightness
    data          <= "11";
    addr_decoded  <= "000100000000";      -- enable register for R1
    wait until rising_edge(clk);
    addr_decoded  <= "000000000000";
        
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;
    
    -- set G1 to medium brightness
    data          <= "10";
    addr_decoded  <= "000010000000";      -- enable register for G1
    wait until rising_edge(clk);
    addr_decoded  <= "000000000000";
        
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;
    
    -- set B1 to min brightness
    data          <= "01";
    addr_decoded  <= "000001000000";      -- enable register for B1
    wait until rising_edge(clk);
    addr_decoded  <= "000000000000";
    
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;
    
    
    -- LD2
    -- set R2 to med brightness
    data          <= "10";
    addr_decoded  <= "000000100000";      -- enable register for R2
    wait until rising_edge(clk);
    addr_decoded  <= "000000000000";
        
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;
    
    -- set G2 to min brightness
    data          <= "01";
    addr_decoded  <= "000000010000";      -- enable register for G2
    wait until rising_edge(clk);
    addr_decoded  <= "000000000000";
        
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;
    
    -- set B2 to med brightness
    data          <= "10";
    addr_decoded  <= "000000001000";      -- enable register for B2
    wait until rising_edge(clk);
    addr_decoded  <= "000000000000";
    
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;    
    
    
    -- LD3
    -- set R3 to off brightness
    data          <= "00";
    addr_decoded  <= "000000000100";      -- enable register for R3
    wait until rising_edge(clk);
    addr_decoded  <= "000000000000";
        
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;
    
    -- set G3 to min brightness
    data          <= "01";
    addr_decoded  <= "000000000010";      -- enable register for G3
    wait until rising_edge(clk);
    addr_decoded  <= "000000000000";
        
    for j in 1 to CLK_STEPS loop
      wait until rising_edge(clk);
    end loop;
    
    -- set B3 to off brightness
    data          <= "00";
    addr_decoded  <= "000000000001";      -- enable register for B3
    wait until rising_edge(clk);
    addr_decoded  <= "000000000000";
    
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
