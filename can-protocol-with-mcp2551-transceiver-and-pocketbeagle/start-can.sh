# Run this script with sudo
# Starts CAN0 and CAN1 with 50kbit/s
ip link set can0 type can bitrate 50000
ip link set can1 type can bitrate 50000
ip link set can0 up
ip link set can1 up
