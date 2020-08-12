-- Test

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity top is
  port(
    -- control signals
    clk               : in  std_logic;
    rst               : in  std_logic;

    -- SPI interface
    sclk              : in std_logic;                         -- SPI clock
    ss_n              : in std_logic;                         -- Slave select
    mosi              : in std_logic;                         -- slave in
    miso              : out std_logic;                        -- slave out
     
    -- LED outputs
    led  : out std_logic_vector(0 to 11)
  );
end entity top;


architecture RTL of top is
  
  -- SPI bus
  signal slave_tx         : std_logic_vector(7 downto 0) := "00100100";       -- byte to transmit (bus) (0x24 = 36 base10)
  signal slave_rx         : std_logic_vector(7 downto 0);       -- byte received (bus)
  signal spi_done_tick    : std_logic;
  
  -- Bus connection from sys-fsm to decoder
  signal Q_ADDR           : std_logic_vector(7 downto 0);         -- address value (8 bit)
  signal Q_DATA           : std_logic_vector(7 downto 0);         -- data value (8 bit)
  signal Q_EN             : std_logic;
  
  -- Address line from decoder to PWM ckt
  signal Y                : std_logic_vector(0 to 11);            -- Decoded address line (out of 8-to-12 decoder)
  
begin

  --instantiate the spi slave
  spi: entity work.spi_slave
    port map(
      clk           => clk,
      reset         => rst,
      dtx           => slave_tx,
      drx           => slave_rx,
      ready         => open,
      spi_done_tick => spi_done_tick,
      sclk          => sclk,
      ss_n          => ss_n,
      mosi          => mosi,
      miso          => miso
    );
  
  -- instantiate the sys_fs
  sys_fsm: entity work.sys_fsm
    port map(
      clk           => clk,
      reset         => rst,
      D             => slave_rx,
      TICK          => spi_done_tick,
      Q_ADDR        => Q_ADDR,
      Q_DATA        => Q_DATA,
      Q_EN          => Q_EN
    );

  -- instantiate decoder
  decoder: entity work.decoder_sync
    port map(
      clk    => clk,
      rst    => rst,
      D_ADDR => Q_ADDR,
      EN     => Q_EN,
      Y      => Y
    );
    
  -- instantiate the PWM
  pwm: entity work.pwm12
  generic map(
      N => 16,
      DV => 8
    )
    port map(
      clk  => clk,
      rst  => rst,
      duty => Q_DATA,
      col  => Y,
      led  => led,
      max_tick => open
    );

end architecture RTL;
