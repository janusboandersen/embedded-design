----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 05.2020 
-- Design Name: 
-- Module Name: decoder_sync
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: Synkron dekoder mellem 8-bit adresse og 12-bit enable bus
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

entity decoder_sync is
  Port ( 
    clk, rst    : in  std_logic;
    D_ADDR      : in  std_logic_vector(7 downto 0);   -- 8-bit address input value
    EN          : in  std_logic;                      -- enable-signal when data and address are updated
    Y           : out std_logic_vector(0 to 11)       -- 12 bit address line
    
  );
end decoder_sync;

architecture arch of decoder_sync is
  signal Y_reg, Y_next  : std_logic_vector(0 to 11);
  signal EN_reg         : std_logic;
  signal D_ADDR_reg     : std_logic_vector(7 downto 0);
  
begin
  
  process(clk, rst)
  begin
    if rst = '1' then
      Y_reg <= (others => '0');
      
    elsif rising_edge(clk) then
      Y_reg <= Y_next;
      
      -- sample on rising edge
      EN_reg <= EN;
      D_ADDR_reg <= D_ADDR;
      
    end if;
  end process;


  process(EN_reg, D_ADDR_reg)
  begin
    
    -- default assignment to avoid latches
    Y_next <= (others => '0');
    
    if (EN_reg = '1') then          -- if enable is on, set active address line based on address value
      
      case D_ADDR_reg is
      when "00000000" =>            -- 0
        Y_next <= "100000000000";   -- Line 1
      
      when "00000001" =>            -- 1
        Y_next <= "010000000000";   -- Line 2
      
      when "00000010" =>            -- 2
        Y_next <= "001000000000";   -- Line 3
      
      when "00000011" =>            -- 3
        Y_next <= "000100000000";   -- Line 4
        
      when "00000100" =>            -- 4
        Y_next <= "000010000000";   -- Line 5

      when "00000101" =>            -- 5
        Y_next <= "000001000000";   -- Line 6

      when "00000110" =>            -- 6
        Y_next <= "000000100000";   -- Line 7
        
      when "00000111" =>            -- 7
        Y_next <= "000000010000";   -- Line 8        

      when "00001000" =>            -- 8
        Y_next <= "000000001000";   -- Line 9    

      when "00001001" =>            -- 9
        Y_next <= "000000000100";   -- Line 10        

      when "00001010" =>            -- 10
        Y_next <= "000000000010";   -- Line 11
        
      when "00001011" =>            -- 11
        Y_next <= "000000000001";   -- Line 12        
                
      when others =>
        Y_next <= "000000000000";   -- No lines
        
      end case;
        
    end if;
  end process;
  
  
  -- Output
  Y <= Y_reg;

end arch;
