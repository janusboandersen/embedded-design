----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Janus Bo Andersen
-- 
-- Create Date: 06.2020
-- Design Name: 
-- Module Name: spi_slave - Behavioral
-- Project Name: Co-design
-- Target Devices: 
-- Tool Versions: 
-- Description: 8 bit SPI slave, runs mode 0, FSMD implementation 
-- Modeled after Chu p. 332
-- Dependencies: 
-- 
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity spi_slave is
  Port (
    clk               : in std_logic;                         -- sys clock
    reset             : in std_logic;                         -- master reset 
    dtx               : in std_logic_vector(7 downto 0);      -- byte to transmit (bus)
    drx               : out std_logic_vector(7 downto 0);     -- byte received (bus)
    ready             : out std_logic;                        -- high when ready
    spi_done_tick     : out std_logic;                        -- tick when all bits received
    
    sclk              : in std_logic;                         -- SPI clock
    ss_n              : in std_logic;                         -- Slave select
    mosi              : in std_logic;                         -- slave in
    miso              : out std_logic                         -- slave out
  );
end spi_slave;

architecture arch of spi_slave is
  type statetype is (idle, p0, p1);                           -- FSMD states
  signal state_reg        : statetype;
  signal state_next       : statetype;
  signal n_reg, n_next    : unsigned(2 downto 0);             -- register of bits transferred
  signal si_reg, si_next  : std_logic_vector(7 downto 0);     -- register of incoming value
  signal so_reg, so_next  : std_logic_vector(7 downto 0);     -- register of outgoing value
  signal spi_clk_reg      : std_logic;                        -- registered value of SPI clock
  signal spi_clk_lag      : std_logic;
  signal f_edge, r_edge   : std_logic;                        -- detect edges on the SPI CLK
  signal ss_n_reg         : std_logic;                        -- register the slave select value

begin

  -- handle reset and sampling of SPI clock
  process(clk, reset)
  begin
    
    if reset = '1' then                   -- if async reset asserted or ss_n becomes idle, reset the device
      state_reg <= idle;
      spi_clk_reg <= '0';
      ss_n_reg <= '1';
      n_reg <= (others => '0');
      so_reg <= (others => '0');          -- miso will be set to zero via this also
      si_reg <= (others => '0');
      
    elsif rising_edge(clk) then
      spi_clk_reg <= sclk;                -- sample SPI clock on rising edge of system clk
      spi_clk_lag <= spi_clk_reg;         -- one cycle difference (process statements are evaluated only at time instant T
                                          --    and signal assignment statements “schedule” future events)
      ss_n_reg <= ss_n;                   -- sample slave select
                                          
      -- synchronous state machine
      state_reg <= state_next;            
      n_reg <= n_next;                    -- counting number of bits received
      so_reg <= so_next;                  -- registered value of data being transferred / shifted out
      si_reg <= si_next;                  -- registered value of data being received / shifted in
      
    end if;
  end process;

  -- detect edges and states for Mode 0
  f_edge <= spi_clk_lag and not spi_clk_reg; -- clock was high and now is low
  r_edge <= not spi_clk_lag and spi_clk_reg; -- clock was low and now is high
  
  -- Next-state logic   
  process(ss_n_reg, state_reg, dtx, n_reg, r_edge, f_edge, mosi, so_reg, si_reg)
  begin
    
    -- default assignment to avoid latches
    state_next <= state_reg;
    ready <= '0';
    spi_done_tick <= '0';
    n_next <= n_reg;
    so_next <= so_reg;
    si_next <= si_reg;
    
    -- handle state machine
    case state_reg is
    when idle =>
      ready <= '1';                             -- ready until transmission begins
      if ss_n_reg = '0' then                    -- slave select for this device has been asserted
        so_next <= dtx;                         -- load the data from bus to be Tx'ed
        n_next <= (others => '0');              -- start counting received bits from zero
        state_next <= p0;                       -- proceed to next state
      end if;                                   -- if ss_n not asserted, stay in state
      
    when p0 =>
      -- In this state, drive the output, and when rising/leading edge appears, then sample the input
      
      if r_edge = '1' then                      -- if a leading edge is detected
        state_next <= p1;                       -- proceed to next state
        si_next <= si_reg(6 downto 0) & mosi;   -- sample mosi by shifting in newest mosi bit
      end if;
       
    when p1 =>
      -- in this state, wait for falling edge to assert a new value 

      if f_edge = '1' then                        -- wait for falling edge and then assert a new value
        so_next <= so_reg(6 downto 0) & '0';      -- slave release the MISO line when all bits become 0.
        if n_reg = 7 then                         -- done receiving all 8 bits
          spi_done_tick <= '1';                   -- tick for transfer done
          state_next <= idle;                     -- proceed to idle state
          
        else                                         
          state_next <= p0;                       -- go to state to wait for sampling
          n_next <= n_reg + 1;                    -- increment number of bits transferred
        end if;   
      end if;
      
    end case;
  end process;   
  
  -- OUTPUT
  miso <= so_reg(7);                          -- set the output from the registry to the bus
  drx <= si_reg;                              -- keep bus updated with received data

end arch;
