library ieee;
use ieee.std_logic_1164.all;
entity reg_reset is
 	generic(N : integer := 8);
   	port(
      clk   : in  std_logic;
      reset : in  std_logic;
      en 	  : in  std_logic;
      d     : in  std_logic_vector(N-1 downto 0);
      q     : out std_logic_vector(N-1 downto 0)
   	);
end reg_reset;

architecture arch of reg_reset is
   signal r_reg, r_next : std_logic_vector(N-1 downto 0);
begin
   -- D FF
   process(clk, reset)
   begin
      if (reset = '1') then
       r_reg <= (others => '0');
      elsif (clk'event and clk = '1') then
         r_reg <= r_next;
      end if;
   end process;
   -- next-state logic
   r_next <= d when en = '1' else 
             r_reg;
   -- output logic
   q <= r_reg;
end arch;
