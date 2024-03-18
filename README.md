# PEBSpoofer
Example concept written in C for changing the current process PEB's address internally at runtime. Supports building in either 32 or 64-bit.

## What is this?

Copies the current process PEB into a new byte array and then sets the pointer to the PEB (located in the TEB) to the address of our byte array. Before our program ends, we set the PEB pointer address back to the original address and then delete our byte array containing our spoofed PEB. Programs such as Process Hacker will show the PEB as being at the original address, while internally we have updated the pointer to a new address. 

![Alt text](Example1.jpg?raw=true "PEB Spoof Example")
