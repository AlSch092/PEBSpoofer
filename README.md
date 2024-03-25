# PEBSpoofer
Example concept written in C for changing the current process PEB's address internally at runtime. Supports building in either x86 or x64.

## What is this?

Copies the current process PEB and headers into a new byte array and then sets the pointer to the PEB (located in the TEB) to the address of our byte array. Before our program ends, we set the PEB pointer address back to the original address and then delete our byte array containing our spoofed PEB. Memory updates which are normally made to the PEB such as the debugger detection flag will instead be written to our new PEB byte array rather than the original, which might cause a bit of extra confusion or difficulty for those trying to reverse engineer the program. Programs such as Process Hacker will show the PEB as being at the original address, while internally we have updated its pointer to a new address.

![Example1](https://github.com/AlSch092/PEBSpoofer/assets/94417808/b99e6235-1bca-430d-86c2-ef86d85ebfef)

## Updates

-Last updated on March 25, 2024: Fixed offsets to grab PEB in x86 __asm blocks, and added the _MYPEB structure which can be casted to from our byte array
