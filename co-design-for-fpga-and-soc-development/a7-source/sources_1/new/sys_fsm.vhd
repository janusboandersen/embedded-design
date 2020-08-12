----------------------------------------------------------------------------------
-- Company: AU Herning
-- Engineer: Janus Bo Andersen
-- 
-- Create Date: 2020
-- Design Name: Co-design
-- Module Name: sys_fsm - Behavioral
-- Project Name: Co-design Spring 2020
-- Target Devices: Arty A7
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


entity sys_fsm is
  Port (
    clk               : in std_logic;                         -- sys clock
    reset             : in std_logic;                         -- master reset
    
    D                 : in std_logic_vector(7 downto 0);      -- 8 bit received from SPI (bus)
    TICK              : in std_logic;                         -- tick when all bits received
    
    Q_ADDR            : out std_logic_vector(7 downto 0);     -- Address byte 
    Q_DATA            : out std_logic_vector(7 downto 0);     -- Data / value byte
    Q_EN              : out std_logic                         -- Enable tick when registers are ready
     
  );
end sys_fsm;


architecture arch of sys_fsm is

  type state_type is (wait_addr, wait_data, set_values);
  signal state_reg, state_next      : state_type;
  signal Q_ADDR_reg, Q_ADDR_next    : std_logic_vector(7 downto 0); -- 8 bit register values for address
  signal Q_DATA_reg, Q_DATA_next    : std_logic_vector(7 downto 0); -- 8 bit register values for data
  signal Q_EN_reg, Q_EN_next        : std_logic;                    -- enable signal for onward circuit

begin

  -- FSMD state and data registers
  process(clk, reset)
  begin
    
    if reset = '1' then
      state_reg   <= wait_addr;
      Q_ADDR_reg  <= "00000000";
      Q_DATA_reg  <= "00000000";
      Q_EN_reg    <= '0';
      
      
    elsif rising_edge(clk) then
      state_reg <= state_next;
      
      -- FSMD data path    
      Q_ADDR_reg <= Q_ADDR_next;
      Q_DATA_reg <= Q_DATA_next;
      Q_EN_reg <= Q_EN_next;
    end if;
  end process;

  
  -- FSMD control path, Next-state-logic
  process(state_reg, Q_ADDR_reg, Q_DATA_reg, D, TICK)
  begin
    
    -- default assignment to avoid latches
    state_next <= state_reg;
    Q_ADDR_next <= Q_ADDR_reg;
    Q_DATA_next <= Q_DATA_reg;
    Q_EN_next <= '0';
    
    case state_reg is 
    
      when wait_addr =>
        if (TICK='1') then            -- SPI is done and address byte is ready
          state_next <= wait_data;
          Q_ADDR_next <= D;                -- Sample input, set address register
        end if;
      
      when wait_data =>
        if (TICK='1') then            -- SPI is done and data byte is ready
          state_next <= set_values;
          Q_DATA_next <= D;                -- Sample input, set data register
        end if;
      
      when set_values =>
        state_next <= wait_addr;
        Q_EN_next <= '1';             -- Give a tick to the onward ckt
      
    end case;

  end process;
  
  
  -- Output
  Q_ADDR <= Q_ADDR_reg;
  Q_DATA <= Q_DATA_reg;
  Q_EN  <= Q_EN_reg;
  
end arch;
