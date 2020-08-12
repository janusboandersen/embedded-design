# Programmable Realtime Unit (PRU) on Sitara AM335x (Arm Cortex-A8)
Codes the PRU peripheral on the PocketBeagle to perform hard realtime high-speed / low-latency I/O operation. 

The two PRUs work independently of the scheduler of the Debian Linux running on the PocketBeagle.

The hand-rolled assembly-version operates significantly faster than the compiled C-version.

Compared to running a similar task on Linux (with its scheduler) shows a factor 2000 speed-up, approximately.
