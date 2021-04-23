# OLL/PLL algorithm discovering

This C++ program enumerates each of all OLL/PLL algorithms that ends with R or R' and doesn't start with U or U'. The program uses a Bidirectional BFS algorithm.

## Usage

1. Run [Prepare.exe](Prepare.exe) for forward searching, which generates "Records_8.dat".
2. Run [Discover.exe](Discover.exe) for backward searching. 
The program can be stopped at anytime by pressing ESC. The OLL/PLL algorithms discovered by the time of stopping will be recorded into found_seqs.txt. [found_seqs.txt](found_seqs.txt) in this repo is an incomplete one.

