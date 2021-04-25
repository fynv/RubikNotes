# OLL/PLL Algorithm Discovering

This C++ program enumerates each of all "optimal-moves" OLL/PLL algorithms that ends with R or R'.
The program uses a Bidirectional BFS algorithm.

## OLL Algorithm Discovering
1. Run [PrepareOLL.exe](PrepareOLL.exe) for forward searching, which generates OLL8.dat.
2. Run [DiscoverOLL.exe](DiscoverOLL.exe) for backward searching, which generates [found_oll.txt](found_oll.txt)

## PLL Algorithm Discovering
1. Run [PreparePLL.exe](PreparePLL.exe) for forward searching, which generates PLL8.dat.
2. Run [DiscoverPLL.exe](DiscoverPLL.exe) for backward searching, which generates [found_pll.txt](found_pll.txt)

The program can be stopped at anytime by pressing "ESC", which will end with an incomplete result file.

## OLL Results

### OLL 1
![](../CFOPRemade/images/OLL_1.png)
```
F'U'LF'L'FFUFR'F'R
BUL'BLBBU'B'RBR'
```
