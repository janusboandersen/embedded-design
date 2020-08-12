----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Janus Bo Andersen
-- 
-- Create Date: 05.2020
-- Design Name: 
-- Module Name: pwm12
-- Project Name: Co-design
-- Target Devices: 
-- Tool Versions: 
-- Description: PWM to control 4 RGB LEDs (12 registers) 
-- Based on file from Morten Opprud
-- Dependencies: 
-- 
-- Additional Comments:
-- 
----------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity pwm12 is
  generic(
    N   : integer := 16;                          -- Bits in the counter
    DV  : integer := 8                            -- Bits in the duty value
  );
	port(
		clk  : in  std_logic;   
		rst  : in  std_logic;                               -- asynchronous reset
		duty : in  std_logic_vector(DV-1 downto 0) := "00000000";         -- duty values, 8 bits
		col  : in  std_logic_vector(12-1 downto 0);         -- 12 diodes -> 12 enables
		led  : out std_logic_vector(12-1 downto 0);         -- pwm output for the 12 diodes
		max_tick : out std_logic                            -- ticks when counter overflows 
	);
end entity pwm12;


architecture RTL of pwm12 is
	constant reg_width : integer := DV;                          -- width of the duty cycle registers
	signal q          : std_logic_vector(N-1 downto 0);          -- 16 bit counter value
	signal r0, g0, b0 : std_logic_vector(reg_width-1 downto 0);  -- LD0 LED
	signal r1, g1, b1 : std_logic_vector(reg_width-1 downto 0);  -- LD1 LED
	signal r2, g2, b2 : std_logic_vector(reg_width-1 downto 0);  -- LD2 LED
	signal r3, g3, b3 : std_logic_vector(reg_width-1 downto 0);  -- LD3 LED
	

begin

  -- free running binary counter
	cnt : entity work.free_run_bin_counter
		generic map(
			N => N
		)
		port map(
			clk      => clk,
			reset    => rst,
			max_tick => max_tick,
			q        => q
		);
		
  
  -- LD0
	RED0 : entity work.reg_reset
		generic map(
			N => reg_width
		)
		port map(
			clk   => clk,
			reset => rst,
			en    => col(0),
			d     => duty,
			q     => r0
		);
			
  GREEN0 : entity work.reg_reset
    generic map(
      N => reg_width
    )
    port map(
      clk   => clk,
      reset => rst,
      en    => col(1),
      d     => duty,
      q     => g0
    );
		
  BLUE0 : entity work.reg_reset
    generic map(
      N => reg_width
    )
    port map(
      clk   => clk,
      reset => rst,
      en    => col(2),
      d     => duty,
      q     => b0
    );
		
		
  -- LD1
  RED1 : entity work.reg_reset
    generic map(
      N => reg_width
    )
    port map(
      clk   => clk,
      reset => rst,
      en    => col(3),
      d     => duty,
      q     => r1
    );
      
  GREEN1 : entity work.reg_reset
    generic map(
      N => reg_width
    )
    port map(
      clk   => clk,
      reset => rst,
      en    => col(4),
      d     => duty,
      q     => g1
    );
    
  BLUE1 : entity work.reg_reset
    generic map(
      N => reg_width
    )
    port map(
      clk   => clk,
      reset => rst,
      en    => col(5),
      d     => duty,
      q     => b1
    );
		
		
		
  -- LD2
  RED2 : entity work.reg_reset
    generic map(
      N => reg_width
    )
    port map(
      clk   => clk,
      reset => rst,
      en    => col(6),
      d     => duty,
      q     => r2
    );
          
  GREEN2 : entity work.reg_reset
    generic map(
      N => reg_width
    )
    port map(
      clk   => clk,
      reset => rst,
      en    => col(7),
      d     => duty,
      q     => g2
    );
    
  BLUE2 : entity work.reg_reset
    generic map(
      N => reg_width
    )
    port map(
      clk   => clk,
      reset => rst,
      en    => col(8),
      d     => duty,
      q     => b2
    );
		
		
  -- LD3
  RED3 : entity work.reg_reset
    generic map(
      N => reg_width
    )
    port map(
      clk   => clk,
      reset => rst,
      en    => col(9),
      d     => duty,
      q     => r3
    );
        
  GREEN3 : entity work.reg_reset
    generic map(
      N => reg_width
    )
    port map(
      clk   => clk,
      reset => rst,
      en    => col(10),
      d     => duty,
      q     => g3
    );
    
  BLUE3 : entity work.reg_reset
    generic map(
      N => reg_width
    )
    port map(
      clk   => clk,
      reset => rst,
      en    => col(11),
      d     => duty,
      q     => b3
    );
	
	
	-- drive LEDs when counter is below requested dutycycle
	
	-- LD0
	led(0) <= '1' when (q(N-1 downto N-DV) < r0) else '0';
	led(1) <= '1' when (q(N-1 downto N-DV) < g0) else '0';
	led(2) <= '1' when (q(N-1 downto N-DV) < b0) else '0';
	
	-- LD1
	led(3) <= '1' when (q(N-1 downto N-DV) < r1) else '0';
  led(4) <= '1' when (q(N-1 downto N-DV) < g1) else '0';
  led(5) <= '1' when (q(N-1 downto N-DV) < b1) else '0';
  
  -- LD2
  led(6) <= '1' when (q(N-1 downto N-DV) < r2) else '0';
  led(7) <= '1' when (q(N-1 downto N-DV) < g2) else '0';
  led(8) <= '1' when (q(N-1 downto N-DV) < b2) else '0';
  
  -- LD3
  led(9)  <= '1' when (q(N-1 downto N-DV) < r3) else '0';
  led(10) <= '1' when (q(N-1 downto N-DV) < g3) else '0';
  led(11) <= '1' when (q(N-1 downto N-DV) < b3) else '0';

end architecture RTL;
