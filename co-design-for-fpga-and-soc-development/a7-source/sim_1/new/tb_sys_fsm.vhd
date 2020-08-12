
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity tb_sys_fsm is
end tb_sys_fsm;

architecture Behavioral of tb_sys_fsm is
  -- Test settings
  constant clock_period         : time := 10 ns;     -- clk period for 100 MHz
  --  constant settling_time      : time := 100 ps;    -- time for signals to register
  constant CLK_STEPS            : integer := 4;      -- system clock steps between SPI clock steps
  
  -- Intermediate simulation signals
  signal clk        : std_logic;                     -- simulated clock signal
  signal rst        : std_logic;                     -- simulated reset
  
  -- Simulate signals from SPI
  signal slave_rx         : std_logic_vector(7 downto 0);
  signal spi_done_tick    : std_logic;
  
  -- Read outputs
  signal Q_ADDR           : std_logic_vector(7 downto 0);
  signal Q_DATA           : std_logic_vector(7 downto 0);
  signal Q_EN             : std_logic;

begin

  -- instantiate the sys_fs
  dut2: entity work.sys_fsm
    port map(
      clk           => clk,
      reset         => rst,
      D             => slave_rx,
      TICK          => spi_done_tick,
      Q_ADDR        => Q_ADDR,
      Q_DATA        => Q_DATA,
      Q_EN          => Q_EN
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
    rst <= '0';
    spi_done_tick <= '0';
    slave_rx <= "00000000";
    
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
    
    
    -- send two commands
    for k in 1 to 2 loop
      
      -- send the address byte and tick
      if k = 1 then
        slave_rx <= "01010101";
      else
        slave_rx <= "00010101";
      end if;
      
      for j in 1 to CLK_STEPS loop
        wait until rising_edge(clk);
      end loop;
  
      spi_done_tick <= '1';
      wait until rising_edge(clk);
      spi_done_tick <= '0';
  
  
      -- send the data byte and tick
      if k = 1 then
        slave_rx <= "10101010";
      else
        slave_rx <= "00101010";
      end if;
      
      for j in 1 to CLK_STEPS loop
        wait until rising_edge(clk);
      end loop;
  
      spi_done_tick <= '1';
      wait until rising_edge(clk);
      spi_done_tick <= '0';
  
      
      -- Wait a while
      for j in 1 to CLK_STEPS loop
          wait until rising_edge(clk);
      end loop;
      
    end loop;

    -- End the simulation
    assert false
      report "Simulation OK!"
      severity failure;
  end process test;  


end Behavioral;
