import VkInline as vki
import math
import numpy as np
from PIL import Image
import glm

VK_FORMAT_R8G8B8A8_SRGB = 43
VK_FORMAT_D32_SFLOAT = 126

class RubiksCube:
    def __init__(self):
        self.map = np.array(range(6*9), dtype = np.uint32)
        self.dirs = np.zeros(6*9, dtype = np.uint32)

        self.op_map = { 
        "R": [self.RCW, self.RCCW], "r": [self.R2CW, self.R2CCW], 
        "L": [self.LCW, self.LCCW], "l": [self.L2CW, self.L2CCW],
        "U": [self.UCW, self.UCCW], "u": [self.U2CW, self.U2CCW],
        "D": [self.DCW, self.DCCW], "d": [self.D2CW, self.D2CCW],
        "F": [self.FCW, self.FCCW], "f": [self.F2CW, self.F2CCW],
        "B": [self.BCW, self.BCCW], "b": [self.B2CW, self.B2CCW],
        "x": [self.XCW, self.XCCW], "y": [self.YCW, self.YCCW], "z": [self.ZCW, self.ZCCW], 
        "E": [self.ECW, self.ECCW], "M": [self.MCW, self.MCCW], "S": [self.SCW, self.SCCW]}

    def XCW0(self):
        map_old = self.map.copy()
        dirs_old = self.dirs.copy()

        # right - 0
        self.map[0] = map_old[6]
        self.dirs[0] = (dirs_old[6] + 3)%4
        self.map[1] = map_old[3]
        self.dirs[1] = (dirs_old[3] + 3)%4
        self.map[2] = map_old[0]
        self.dirs[2] = (dirs_old[0] + 3)%4
        self.map[3] = map_old[7]
        self.dirs[3] = (dirs_old[7] + 3)%4
        self.map[4] = map_old[4]
        self.dirs[4] = (dirs_old[4] + 3)%4
        self.map[5] = map_old[1]
        self.dirs[5] = (dirs_old[1] + 3)%4
        self.map[6] = map_old[8]
        self.dirs[6] = (dirs_old[8] + 3)%4
        self.map[7] = map_old[5]
        self.dirs[7] = (dirs_old[5] + 3)%4
        self.map[8] = map_old[2]
        self.dirs[8] = (dirs_old[2] + 3)%4

        # up - 2    
        self.map[20] = map_old[38]
        self.dirs[20] = dirs_old[38]
        self.map[23] = map_old[41]
        self.dirs[23] = dirs_old[41]
        self.map[26] = map_old[44]
        self.dirs[26] = dirs_old[44]

        # down - 3
        self.map[29] = map_old[51]
        self.dirs[29] = (dirs_old[51]+2)%4
        self.map[32] = map_old[48]
        self.dirs[32] = (dirs_old[48]+2)%4
        self.map[35] = map_old[45]
        self.dirs[35] = (dirs_old[45]+2)%4

        # front - 4 
        self.map[38] = map_old[29]
        self.dirs[38] = dirs_old[29]
        self.map[41] = map_old[32]
        self.dirs[41] = dirs_old[32]
        self.map[44] = map_old[35]
        self.dirs[44] = dirs_old[35]

        # back - 5
        self.map[45] = map_old[26]
        self.dirs[45] = (dirs_old[26]+2)%4
        self.map[48] = map_old[23]
        self.dirs[48] = (dirs_old[23]+2)%4
        self.map[51] = map_old[20]
        self.dirs[51] = (dirs_old[20]+2)%4

    def XCW1(self):
        map_old = self.map.copy()
        dirs_old = self.dirs.copy()

        # up - 2    
        self.map[19] = map_old[37]
        self.dirs[19] = dirs_old[37]
        self.map[22] = map_old[40]
        self.dirs[22] = dirs_old[40]
        self.map[25] = map_old[43]
        self.dirs[25] = dirs_old[43]   

        # down - 3
        self.map[28] = map_old[52]
        self.dirs[28] = (dirs_old[52]+2)%4
        self.map[31] = map_old[49]
        self.dirs[31] = (dirs_old[49]+2)%4
        self.map[34] = map_old[46]
        self.dirs[34] = (dirs_old[46]+2)%4

        # front - 4 
        self.map[37] = map_old[28]
        self.dirs[37] = dirs_old[28]
        self.map[40] = map_old[31]
        self.dirs[40] = dirs_old[31]
        self.map[43] = map_old[34]
        self.dirs[43] = dirs_old[34]

        # back - 5
        self.map[46] = map_old[25]
        self.dirs[46] = (dirs_old[25]+2)%4
        self.map[49] = map_old[22]
        self.dirs[49] = (dirs_old[22]+2)%4
        self.map[52] = map_old[19]
        self.dirs[52] = (dirs_old[19]+2)%4

    def XCW2(self):
        map_old = self.map.copy()
        dirs_old = self.dirs.copy()

        # left - 1
        self.map[9] = map_old[11]
        self.dirs[9] = (dirs_old[11] + 1)%4
        self.map[10] = map_old[14]
        self.dirs[10] = (dirs_old[14] + 1)%4
        self.map[11] = map_old[17]
        self.dirs[11] = (dirs_old[17] + 1)%4
        self.map[12] = map_old[10]
        self.dirs[12] = (dirs_old[10] + 1)%4
        self.map[13] = map_old[13]
        self.dirs[13] = (dirs_old[13] + 1)%4
        self.map[14] = map_old[16]
        self.dirs[14] = (dirs_old[16] + 1)%4
        self.map[15] = map_old[9]
        self.dirs[15] = (dirs_old[9] + 1)%4
        self.map[16] = map_old[12]
        self.dirs[16] = (dirs_old[12] + 1)%4
        self.map[17] = map_old[15]
        self.dirs[17] = (dirs_old[15] + 1)%4

        # up - 2  
        self.map[18] = map_old[36]
        self.dirs[18] = dirs_old[36]
        self.map[21] = map_old[39]
        self.dirs[21] = dirs_old[39]
        self.map[24] = map_old[42]
        self.dirs[24] = dirs_old[42]
        
        # down - 3
        self.map[27] = map_old[53]
        self.dirs[27] = (dirs_old[53]+2)%4
        self.map[30] = map_old[50]
        self.dirs[30] = (dirs_old[50]+2)%4
        self.map[33] = map_old[47]
        self.dirs[33] = (dirs_old[47]+2)%4

        # front - 4 
        self.map[36] = map_old[27]
        self.dirs[36] = dirs_old[27]
        self.map[39] = map_old[30]
        self.dirs[39] = dirs_old[30]
        self.map[42] = map_old[33]
        self.dirs[42] = dirs_old[33]

        # back - 5
        self.map[47] = map_old[24]
        self.dirs[47] = (dirs_old[24]+2)%4
        self.map[50] = map_old[21]
        self.dirs[50] = (dirs_old[21]+2)%4
        self.map[53] = map_old[18]
        self.dirs[53] = (dirs_old[18]+2)%4

    def XCCW0(self):
        map_old = self.map.copy()
        dirs_old = self.dirs.copy()

        # left - 1
        self.map[9] = map_old[15]
        self.dirs[9] = (dirs_old[15] + 3)%4
        self.map[10] = map_old[12]
        self.dirs[10] = (dirs_old[12] + 3)%4
        self.map[11] = map_old[9]
        self.dirs[11] = (dirs_old[9] + 3)%4
        self.map[12] = map_old[16]
        self.dirs[12] = (dirs_old[16] + 3)%4
        self.map[13] = map_old[13]
        self.dirs[13] = (dirs_old[13] + 3)%4
        self.map[14] = map_old[10]
        self.dirs[14] = (dirs_old[10] + 3)%4
        self.map[15] = map_old[17]
        self.dirs[15] = (dirs_old[17] + 3)%4
        self.map[16] = map_old[14]
        self.dirs[16] = (dirs_old[14] + 3)%4
        self.map[17] = map_old[11]
        self.dirs[17] = (dirs_old[11] + 3)%4        

        # up - 2
        self.map[18] = map_old[53]
        self.dirs[18] = (dirs_old[53] +2)%4
        self.map[21] = map_old[50]
        self.dirs[21] = (dirs_old[50] +2)%4
        self.map[24] = map_old[47]
        self.dirs[24] = (dirs_old[47] +2)%4

        # down - 3
        self.map[27] = map_old[36]
        self.dirs[27] = dirs_old[36]
        self.map[30] = map_old[39]
        self.dirs[30] = dirs_old[39]
        self.map[33] = map_old[42]
        self.dirs[33] = dirs_old[42]

        # front - 4 
        self.map[36] = map_old[18]
        self.dirs[36] = dirs_old[18]
        self.map[39] = map_old[21]
        self.dirs[39] = dirs_old[21]
        self.map[42] = map_old[24]
        self.dirs[42] = dirs_old[24]

        # back - 5
        self.map[47] = map_old[33]
        self.dirs[47] = (dirs_old[33]+2)%4
        self.map[50] = map_old[30]
        self.dirs[50] = (dirs_old[30]+2)%4
        self.map[53] = map_old[27]
        self.dirs[53] = (dirs_old[27]+2)%4

    def XCCW1(self):
        map_old = self.map.copy()
        dirs_old = self.dirs.copy()

        # up - 2    
        self.map[19] = map_old[52]
        self.dirs[19] = (dirs_old[52] +2)%4
        self.map[22] = map_old[49]
        self.dirs[22] = (dirs_old[49] +2)%4
        self.map[25] = map_old[46]
        self.dirs[25] = (dirs_old[46] +2)%4

        # down - 3
        self.map[28] = map_old[37]
        self.dirs[28] = dirs_old[37]
        self.map[31] = map_old[40]
        self.dirs[31] = dirs_old[40]
        self.map[34] = map_old[43]
        self.dirs[34] = dirs_old[43]

        # front - 4 
        self.map[37] = map_old[19]
        self.dirs[37] = dirs_old[19]
        self.map[40] = map_old[22]
        self.dirs[40] = dirs_old[22]
        self.map[43] = map_old[25]
        self.dirs[43] = dirs_old[25]

        # back - 5
        self.map[46] = map_old[34]
        self.dirs[46] = (dirs_old[34]+2)%4
        self.map[49] = map_old[31]
        self.dirs[49] = (dirs_old[31]+2)%4
        self.map[52] = map_old[28]
        self.dirs[52] = (dirs_old[28]+2)%4

    def XCCW2(self):
        map_old = self.map.copy()
        dirs_old = self.dirs.copy()        

        # right - 0
        self.map[0] = map_old[2]
        self.dirs[0] = (dirs_old[2] + 1)%4
        self.map[1] = map_old[5]
        self.dirs[1] = (dirs_old[5] + 1)%4
        self.map[2] = map_old[8]
        self.dirs[2] = (dirs_old[8] + 1)%4
        self.map[3] = map_old[1]
        self.dirs[3] = (dirs_old[1] + 1)%4
        self.map[4] = map_old[4]
        self.dirs[4] = (dirs_old[4] + 1)%4
        self.map[5] = map_old[7]
        self.dirs[5] = (dirs_old[7] + 1)%4
        self.map[6] = map_old[0]
        self.dirs[6] = (dirs_old[0] + 1)%4
        self.map[7] = map_old[3]
        self.dirs[7] = (dirs_old[3] + 1)%4
        self.map[8] = map_old[6]
        self.dirs[8] = (dirs_old[6] + 1)%4    

        # up - 2    
        self.map[20] = map_old[51]
        self.dirs[20] = (dirs_old[51] +2)%4
        self.map[23] = map_old[48]
        self.dirs[23] = (dirs_old[48] +2)%4
        self.map[26] = map_old[45]
        self.dirs[26] = (dirs_old[45] +2)%4        

        # down - 3
        self.map[29] = map_old[38]
        self.dirs[29] = dirs_old[38]
        self.map[32] = map_old[41]
        self.dirs[32] = dirs_old[41]
        self.map[35] = map_old[44]
        self.dirs[35] = dirs_old[44]

        # front - 4 
        self.map[38] = map_old[20]
        self.dirs[38] = dirs_old[20]
        self.map[41] = map_old[23]
        self.dirs[41] = dirs_old[23]
        self.map[44] = map_old[26]
        self.dirs[44] = dirs_old[26]

        # back - 5
        self.map[45] = map_old[35]
        self.dirs[45] = (dirs_old[35]+2)%4
        self.map[48] = map_old[32]
        self.dirs[48] = (dirs_old[32]+2)%4
        self.map[51] = map_old[29]
        self.dirs[51] = (dirs_old[29]+2)%4

    def YCW0(self):
        map_old = self.map.copy()
        dirs_old = self.dirs.copy()

        # right - 0
        self.map[0:3] = map_old[45:48]
        self.dirs[0:3] = dirs_old[45:48]

        # left - 1
        self.map[9:12] = map_old[36:39]
        self.dirs[9:12] = dirs_old[36:39]

        # up - 2
        self.map[18] = map_old[24]
        self.dirs[18] = (dirs_old[24] + 3)%4
        self.map[19] = map_old[21]
        self.dirs[19] = (dirs_old[21] + 3)%4
        self.map[20] = map_old[18]
        self.dirs[20] = (dirs_old[18] + 3)%4
        self.map[21] = map_old[25]
        self.dirs[21] = (dirs_old[25] + 3)%4
        self.map[22] = map_old[22]
        self.dirs[22] = (dirs_old[22] + 3)%4
        self.map[23] = map_old[19]
        self.dirs[23] = (dirs_old[19] + 3)%4
        self.map[24] = map_old[26]
        self.dirs[24] = (dirs_old[26] + 3)%4
        self.map[25] = map_old[23]
        self.dirs[25] = (dirs_old[23] + 3)%4
        self.map[26] = map_old[20]
        self.dirs[26] = (dirs_old[20] + 3)%4

        # front - 4 
        self.map[36:39] = map_old[0:3]
        self.dirs[36:39] = dirs_old[0:3]

        # back - 5
        self.map[45:48] = map_old[9:12]
        self.dirs[45:48] = dirs_old[9:12]

    def YCW1(self):
        map_old = self.map.copy()
        dirs_old = self.dirs.copy()

        # right - 0
        self.map[3:6] = map_old[48:51]
        self.dirs[3:6] = dirs_old[48:51]

        # left - 1
        self.map[12:15] = map_old[39:42]
        self.dirs[12:15] = dirs_old[39:42]

        # front - 4 
        self.map[39:42] = map_old[3:6]
        self.dirs[39:42] = dirs_old[3:6]

        # back - 5
        self.map[48:51] = map_old[12:15]
        self.dirs[48:51] = dirs_old[12:15]

    def YCW2(self):
        map_old = self.map.copy()
        dirs_old = self.dirs.copy()

        # right - 0
        self.map[6:9] = map_old[51:54]
        self.dirs[6:9] = dirs_old[51:54]

        # left - 1        
        self.map[15:18] = map_old[42:45]
        self.dirs[15:18] = dirs_old[42:45]

        # down - 3 
        self.map[27] = map_old[29]
        self.dirs[27] = (dirs_old[29] + 1)%4
        self.map[28] = map_old[32]
        self.dirs[28] = (dirs_old[32] + 1)%4
        self.map[29] = map_old[35]
        self.dirs[29] = (dirs_old[35] + 1)%4
        self.map[30] = map_old[28]
        self.dirs[30] = (dirs_old[28] + 1)%4
        self.map[31] = map_old[31]
        self.dirs[31] = (dirs_old[31] + 1)%4
        self.map[32] = map_old[34]
        self.dirs[32] = (dirs_old[34] + 1)%4
        self.map[33] = map_old[27]
        self.dirs[33] = (dirs_old[27] + 1)%4
        self.map[34] = map_old[30]
        self.dirs[34] = (dirs_old[30] + 1)%4
        self.map[35] = map_old[33]
        self.dirs[35] = (dirs_old[33] + 1)%4  

        # front - 4 
        self.map[42:45] = map_old[6:9]
        self.dirs[42:45] = dirs_old[6:9]

        # back - 5
        self.map[51:54] = map_old[15:18]
        self.dirs[51:54] = dirs_old[15:18]

    def YCCW0(self):
        map_old = self.map.copy()
        dirs_old = self.dirs.copy()

        # right - 0        
        self.map[6:9] = map_old[42:45]
        self.dirs[6:9] = dirs_old[42:45]

        # left - 1
        self.map[15:18] = map_old[51:54]
        self.dirs[15:18] = dirs_old[51:54]

        # down - 3  
        self.map[27] = map_old[33]
        self.dirs[27] = (dirs_old[33] + 3)%4
        self.map[28] = map_old[30]
        self.dirs[28] = (dirs_old[30] + 3)%4
        self.map[29] = map_old[27]
        self.dirs[29] = (dirs_old[27] + 3)%4
        self.map[30] = map_old[34]
        self.dirs[30] = (dirs_old[34] + 3)%4
        self.map[31] = map_old[31]
        self.dirs[31] = (dirs_old[31] + 3)%4
        self.map[32] = map_old[28]
        self.dirs[32] = (dirs_old[28] + 3)%4
        self.map[33] = map_old[35]
        self.dirs[33] = (dirs_old[35] + 3)%4
        self.map[34] = map_old[32]
        self.dirs[34] = (dirs_old[32] + 3)%4
        self.map[35] = map_old[29]
        self.dirs[35] = (dirs_old[29] + 3)%4

        # front - 4 
        self.map[42:45] = map_old[15:18]
        self.dirs[42:45] = dirs_old[15:18]

        # back - 5
        self.map[51:54] = map_old[6:9]
        self.dirs[51:54] = dirs_old[6:9]

    def YCCW1(self):
        map_old = self.map.copy()
        dirs_old = self.dirs.copy()

        # right - 0
        self.map[3:6] = map_old[39:42]
        self.dirs[3:6] = dirs_old[39:42]

        # left - 1
        self.map[12:15] = map_old[48:51]
        self.dirs[12:15] = dirs_old[48:51]

        # front - 4 
        self.map[39:42] = map_old[12:15]
        self.dirs[39:42] = dirs_old[12:15]

        # back - 5
        self.map[48:51] = map_old[3:6]
        self.dirs[48:51] = dirs_old[3:6]

    def YCCW2(self):
        map_old = self.map.copy()
        dirs_old = self.dirs.copy()        

        # right - 0
        self.map[0:3] = map_old[36:39]
        self.dirs[0:3] = dirs_old[36:39]

        # left - 1
        self.map[9:12] = map_old[45:48]
        self.dirs[9:12] = dirs_old[45:48]

        # up - 2      
        self.map[18] = map_old[20]
        self.dirs[18] = (dirs_old[20] + 1)%4
        self.map[19] = map_old[23]
        self.dirs[19] = (dirs_old[23] + 1)%4
        self.map[20] = map_old[26]
        self.dirs[20] = (dirs_old[26] + 1)%4
        self.map[21] = map_old[19]
        self.dirs[21] = (dirs_old[19] + 1)%4
        self.map[22] = map_old[22]
        self.dirs[22] = (dirs_old[22] + 1)%4
        self.map[23] = map_old[25]
        self.dirs[23] = (dirs_old[25] + 1)%4
        self.map[24] = map_old[18]
        self.dirs[24] = (dirs_old[18] + 1)%4
        self.map[25] = map_old[21]
        self.dirs[25] = (dirs_old[21] + 1)%4
        self.map[26] = map_old[24]
        self.dirs[26] = (dirs_old[24] + 1)%4

        # front - 4 
        self.map[36:39] = map_old[9:12]
        self.dirs[36:39] = dirs_old[9:12]

        # back - 5
        self.map[45:48] = map_old[0:3]
        self.dirs[45:48] = dirs_old[0:3]

    def ZCW0(self):
        map_old = self.map.copy()
        dirs_old = self.dirs.copy()

        # right - 0 
        self.map[0] = map_old[24]
        self.dirs[0] = (dirs_old[24] + 3)%4
        self.map[3] = map_old[25]
        self.dirs[3] = (dirs_old[25] + 3)%4
        self.map[6] = map_old[26]
        self.dirs[6] = (dirs_old[26] + 3)%4

        # left - 1
        self.map[11] = map_old[27]
        self.dirs[11] = (dirs_old[27] + 3)%4
        self.map[14] = map_old[28]
        self.dirs[14] = (dirs_old[28] + 3)%4
        self.map[17] = map_old[29]
        self.dirs[17] = (dirs_old[29] + 3)%4  

        # up - 2  
        self.map[24] = map_old[17]
        self.dirs[24] = (dirs_old[17] + 3)%4
        self.map[25] = map_old[14]
        self.dirs[25] = (dirs_old[14] + 3)%4
        self.map[26] = map_old[11]
        self.dirs[26] = (dirs_old[11] + 3)%4

        # down - 3
        self.map[27] = map_old[6]
        self.dirs[27] = (dirs_old[6] + 3)%4
        self.map[28] = map_old[3]
        self.dirs[28] = (dirs_old[3] + 3)%4
        self.map[29] = map_old[0]
        self.dirs[29] = (dirs_old[0] + 3)%4

        # front - 4 
        self.map[36] = map_old[42]
        self.dirs[36] = (dirs_old[42] + 3)%4
        self.map[37] = map_old[39]
        self.dirs[37] = (dirs_old[39] + 3)%4
        self.map[38] = map_old[36]
        self.dirs[38] = (dirs_old[36] + 3)%4
        self.map[39] = map_old[43]
        self.dirs[39] = (dirs_old[43] + 3)%4
        self.map[40] = map_old[40]
        self.dirs[40] = (dirs_old[40] + 3)%4
        self.map[41] = map_old[37]
        self.dirs[41] = (dirs_old[37] + 3)%4
        self.map[42] = map_old[44]
        self.dirs[42] = (dirs_old[44] + 3)%4
        self.map[43] = map_old[41]
        self.dirs[43] = (dirs_old[41] + 3)%4
        self.map[44] = map_old[38]
        self.dirs[44] = (dirs_old[38] + 3)%4

    def ZCW1(self):
        map_old = self.map.copy()
        dirs_old = self.dirs.copy()

        # right - 0 
        self.map[1] = map_old[21]
        self.dirs[1] = (dirs_old[21] + 3)%4
        self.map[4] = map_old[22]
        self.dirs[4] = (dirs_old[22] + 3)%4
        self.map[7] = map_old[23]
        self.dirs[7] = (dirs_old[23] + 3)%4

        # left - 1
        self.map[10] = map_old[30]
        self.dirs[10] = (dirs_old[30] + 3)%4
        self.map[13] = map_old[31]
        self.dirs[13] = (dirs_old[31] + 3)%4
        self.map[16] = map_old[32]
        self.dirs[16] = (dirs_old[32] + 3)%4

        # up - 2  
        self.map[21] = map_old[16]
        self.dirs[21] = (dirs_old[16] + 3)%4
        self.map[22] = map_old[13]
        self.dirs[22] = (dirs_old[13] + 3)%4
        self.map[23] = map_old[10]
        self.dirs[23] = (dirs_old[10] + 3)%4

        # down - 3
        self.map[30] = map_old[7]
        self.dirs[30] = (dirs_old[7] + 3)%4
        self.map[31] = map_old[4]
        self.dirs[31] = (dirs_old[4] + 3)%4
        self.map[32] = map_old[1]
        self.dirs[32] = (dirs_old[1] + 3)%4       

    def ZCW2(self):
        map_old = self.map.copy()
        dirs_old = self.dirs.copy()

        # right - 0 
        self.map[2] = map_old[18]
        self.dirs[2] = (dirs_old[18] + 3)%4
        self.map[5] = map_old[19]
        self.dirs[5] = (dirs_old[19] + 3)%4
        self.map[8] = map_old[20]
        self.dirs[8] = (dirs_old[20] + 3)%4

        # left - 1
        self.map[9] = map_old[33]
        self.dirs[9] = (dirs_old[33] + 3)%4
        self.map[12] = map_old[34]
        self.dirs[12] = (dirs_old[34] + 3)%4
        self.map[15] = map_old[35]
        self.dirs[15] = (dirs_old[35] + 3)%4

        # up - 2
        self.map[18] = map_old[15]
        self.dirs[18] = (dirs_old[15] + 3)%4
        self.map[19] = map_old[12]
        self.dirs[19] = (dirs_old[12] + 3)%4
        self.map[20] = map_old[9]
        self.dirs[20] = (dirs_old[9] + 3)%4

        # down - 3
        self.map[33] = map_old[8]
        self.dirs[33] = (dirs_old[8] + 3)%4
        self.map[34] = map_old[5]
        self.dirs[34] = (dirs_old[5] + 3)%4
        self.map[35] = map_old[2]
        self.dirs[35] = (dirs_old[2] + 3)%4

        # back - 5
        self.map[45] = map_old[47]
        self.dirs[45] = (dirs_old[47] + 1)%4
        self.map[46] = map_old[50]
        self.dirs[46] = (dirs_old[50] + 1)%4
        self.map[47] = map_old[53]
        self.dirs[47] = (dirs_old[53] + 1)%4
        self.map[48] = map_old[46]
        self.dirs[48] = (dirs_old[46] + 1)%4
        self.map[49] = map_old[49]
        self.dirs[49] = (dirs_old[49] + 1)%4
        self.map[50] = map_old[52]
        self.dirs[50] = (dirs_old[52] + 1)%4
        self.map[51] = map_old[45]
        self.dirs[51] = (dirs_old[45] + 1)%4
        self.map[52] = map_old[48]
        self.dirs[52] = (dirs_old[48] + 1)%4
        self.map[53] = map_old[51]
        self.dirs[53] = (dirs_old[51] + 1)%4

    def ZCCW0(self):
        map_old = self.map.copy()
        dirs_old = self.dirs.copy()

        # right - 0
        self.map[2] = map_old[35]
        self.dirs[2] = (dirs_old[35] + 1)%4
        self.map[5] = map_old[34]
        self.dirs[5] = (dirs_old[34] + 1)%4
        self.map[8] = map_old[33]
        self.dirs[8] = (dirs_old[33] + 1)%4

        # left - 1
        self.map[9] = map_old[20]
        self.dirs[9] = (dirs_old[20] + 1)%4
        self.map[12] = map_old[19]
        self.dirs[12] = (dirs_old[19] + 1)%4
        self.map[15] = map_old[18]
        self.dirs[15] = (dirs_old[18] + 1)%4

        # up - 2
        self.map[18] = map_old[2]
        self.dirs[18] = (dirs_old[2] + 1)%4
        self.map[19] = map_old[5]
        self.dirs[19] = (dirs_old[5] + 1)%4
        self.map[20] = map_old[8]
        self.dirs[20] = (dirs_old[8] + 1)%4     

        # down - 3
        self.map[33] = map_old[9]
        self.dirs[33] = (dirs_old[9] + 1)%4
        self.map[34] = map_old[12]
        self.dirs[34] = (dirs_old[12] + 1)%4
        self.map[35] = map_old[15]
        self.dirs[35] = (dirs_old[15] + 1)%4

        # back - 5
        self.map[45] = map_old[51]
        self.dirs[45] = (dirs_old[51] + 3)%4
        self.map[46] = map_old[48]
        self.dirs[46] = (dirs_old[48] + 3)%4
        self.map[47] = map_old[45]
        self.dirs[47] = (dirs_old[45] + 3)%4
        self.map[48] = map_old[52]
        self.dirs[48] = (dirs_old[52] + 3)%4
        self.map[49] = map_old[49]
        self.dirs[49] = (dirs_old[49] + 3)%4
        self.map[50] = map_old[46]
        self.dirs[50] = (dirs_old[46] + 3)%4
        self.map[51] = map_old[53]
        self.dirs[51] = (dirs_old[53] + 3)%4
        self.map[52] = map_old[50]
        self.dirs[52] = (dirs_old[50] + 3)%4
        self.map[53] = map_old[47]
        self.dirs[53] = (dirs_old[47] + 3)%4

    def ZCCW1(self):
        map_old = self.map.copy()
        dirs_old = self.dirs.copy()

        # right - 0
        self.map[1] = map_old[32]
        self.dirs[1] = (dirs_old[32] + 1)%4
        self.map[4] = map_old[31]
        self.dirs[4] = (dirs_old[31] + 1)%4
        self.map[7] = map_old[30]
        self.dirs[7] = (dirs_old[30] + 1)%4

        # left - 1
        self.map[10] = map_old[23]
        self.dirs[10] = (dirs_old[23] + 1)%4
        self.map[13] = map_old[22]
        self.dirs[13] = (dirs_old[22] + 1)%4
        self.map[16] = map_old[21]
        self.dirs[16] = (dirs_old[21] + 1)%4

        # up - 2
        self.map[21] = map_old[1]
        self.dirs[21] = (dirs_old[1] + 1)%4
        self.map[22] = map_old[4]
        self.dirs[22] = (dirs_old[4] + 1)%4
        self.map[23] = map_old[7]
        self.dirs[23] = (dirs_old[7] + 1)%4

        # down - 3
        self.map[30] = map_old[10]
        self.dirs[30] = (dirs_old[10] + 1)%4
        self.map[31] = map_old[13]
        self.dirs[31] = (dirs_old[13] + 1)%4
        self.map[32] = map_old[16]
        self.dirs[32] = (dirs_old[16] + 1)%4

    def ZCCW2(self):
        map_old = self.map.copy()
        dirs_old = self.dirs.copy()

        # right - 0 
        self.map[0] = map_old[29]
        self.dirs[0] = (dirs_old[29] + 1)%4
        self.map[3] = map_old[28]
        self.dirs[3] = (dirs_old[28] + 1)%4
        self.map[6] = map_old[27]
        self.dirs[6] = (dirs_old[27] + 1)%4

        # left - 1
        self.map[11] = map_old[26]
        self.dirs[11] = (dirs_old[26] + 1)%4
        self.map[14] = map_old[25]
        self.dirs[14] = (dirs_old[25] + 1)%4
        self.map[17] = map_old[24]
        self.dirs[17] = (dirs_old[24] + 1)%4

        # up - 2  
        self.map[24] = map_old[0]
        self.dirs[24] = (dirs_old[0] + 1)%4
        self.map[25] = map_old[3]
        self.dirs[25] = (dirs_old[3] + 1)%4
        self.map[26] = map_old[6]
        self.dirs[26] = (dirs_old[6] + 1)%4

        # down - 3
        self.map[27] = map_old[11]
        self.dirs[27] = (dirs_old[11] + 1)%4
        self.map[28] = map_old[14]
        self.dirs[28] = (dirs_old[14] + 1)%4
        self.map[29] = map_old[17]
        self.dirs[29] = (dirs_old[17] + 1)%4

        # front - 4 
        self.map[36] = map_old[38]
        self.dirs[36] = (dirs_old[38] + 1)%4
        self.map[37] = map_old[41]
        self.dirs[37] = (dirs_old[41] + 1)%4
        self.map[38] = map_old[44]
        self.dirs[38] = (dirs_old[44] + 1)%4
        self.map[39] = map_old[37]
        self.dirs[39] = (dirs_old[37] + 1)%4
        self.map[40] = map_old[40]
        self.dirs[40] = (dirs_old[40] + 1)%4
        self.map[41] = map_old[43]
        self.dirs[41] = (dirs_old[43] + 1)%4
        self.map[42] = map_old[36]
        self.dirs[42] = (dirs_old[36] + 1)%4
        self.map[43] = map_old[39]
        self.dirs[43] = (dirs_old[39] + 1)%4
        self.map[44] = map_old[42]
        self.dirs[44] = (dirs_old[42] + 1)%4

    def RCW(self):
        self.XCW0()

    def RCCW(self):
        self.XCCW2()

    def R2CW(self):
        self.XCW0()
        self.XCW1()

    def R2CCW(self):
        self.XCCW2()
        self.XCCW1()

    def LCW(self):
        self.XCCW0()

    def LCCW(self):
        self.XCW2()

    def L2CW(self):
        self.XCCW0()
        self.XCCW1()

    def L2CCW(self):
        self.XCW2()
        self.XCW1()

    def UCW(self):
        self.YCW0()

    def UCCW(self):
        self.YCCW2()

    def U2CW(self):
        self.YCW0()
        self.YCW1()

    def U2CCW(self):
        self.YCCW2()
        self.YCCW1()

    def DCW(self):
        self.YCCW0()

    def DCCW(self):
        self.YCW2()

    def D2CW(self):
        self.YCCW0()
        self.YCCW1()

    def D2CCW(self):
        self.YCW2()
        self.YCW1()

    def FCW(self):
        self.ZCW0()

    def FCCW(self):
        self.ZCCW2()

    def F2CW(self):
        self.ZCW0()
        self.ZCW1()

    def F2CCW(self):
        self.ZCCW2()
        self.ZCCW1()

    def BCW(self):
        self.ZCCW0()

    def BCCW(self):
        self.ZCW2()

    def B2CW(self):
        self.ZCCW0()
        self.ZCCW1()

    def B2CCW(self):
        self.ZCW2()
        self.ZCW1()

    def XCW(self):
        self.XCW0()
        self.XCW1()
        self.XCW2()

    def XCCW(self):
        self.XCCW0()
        self.XCCW1()
        self.XCCW2()

    def YCW(self):
        self.YCW0()
        self.YCW1()
        self.YCW2()

    def YCCW(self):
        self.YCCW0()
        self.YCCW1()
        self.YCCW2()

    def ZCW(self):
        self.ZCW0()
        self.ZCW1()
        self.ZCW2()

    def ZCCW(self):
        self.ZCCW0()
        self.ZCCW1()
        self.ZCCW2()

    def MCW(self):
        self.XCCW1()

    def MCCW(self):
        self.XCW1()

    def ECW(self):
        self.YCCW1()

    def ECCW(self):
        self.YCW1()

    def SCW(self):
        self.ZCW1()

    def SCCW(self):
        self.ZCCW1()

    def exec_seq(self, seq, reverse=False):
        if type(seq) is str:
            operations = []
            i = 0
            while i<len(seq):
                c = seq[i]
                if c in self.op_map:                    
                    if i < len(seq) -1 and (seq[i+1]=="'" or seq[i+1]=="`"):
                        if reverse:
                            op = self.op_map[c][0]
                        else:
                            op = self.op_map[c][1]
                        i+=1
                    else:
                        if reverse:                            
                            op = self.op_map[c][1]
                        else:
                            op = self.op_map[c][0]

                    count = 1
                    if i < len(seq) -1 and seq[i+1]=="2":
                        count = 2
                        i+=1

                    for j in range(count):
                        operations +=[op]
                i+=1
        else:
            operations = seq

        if reverse:
            for op in reversed(operations):
                op()
        else:
            for op in operations:
                op()        

          
class PerspectiveView:
    def __init__(self, fn_skin = "skin_default.png"):
        self.width = 640
        self.height = 480
        proj_mat = glm.perspective(glm.radians(45.0), self.width/self.height, 0.1, 1000.0)
        view_mat = glm.lookAt(glm.vec3(10.0,10.0,10.0), glm.vec3(0.0,0.0,0.0), glm.vec3(0.0, 1.0, 0.0))
        self.matrix = proj_mat*view_mat

        self.bg_color = [1.0, 1.0, 1.0, 1.0]

        skin_in =  np.array(Image.open(fn_skin).convert('RGBA'))
        self.skin = vki.Cubemap(skin_in.shape[1], skin_in.shape[1], VK_FORMAT_R8G8B8A8_SRGB)
        self.skin.upload(skin_in)

        self.rp = vki.Rasterizer(["matrix", "map", "dirs"], type_locked=True)

        self.rp.add_draw_call(vki.DrawCall(
'''
const vec3 positions[18] = { 
    vec3(3.0, 3.0, 3.0), vec3(3.0, 3.0, -3.0), vec3(3.0, -3.0, 3.0),
    vec3(-3.0, 3.0, -3.0), vec3(-3.0, 3.0, 3.0), vec3(-3.0, -3.0, -3.0),
    vec3(-3.0, 3.0, -3.0), vec3(3.0, 3.0, -3.0), vec3(-3.0, 3.0, 3.0),
    vec3(-3.0, -3.0, 3.0), vec3(3.0, -3.0, 3.0), vec3(-3.0, -3.0, -3.0),
    vec3(-3.0, 3.0, 3.0), vec3(3.0, 3.0, 3.0), vec3(-3.0, -3.0, 3.0),
    vec3(3.0, 3.0, -3.0), vec3(-3.0, 3.0, -3.0), vec3(3.0, -3.0, -3.0) };

const vec2 indices[24] = { 
    vec2(0.0,0.0), vec2(0.0,1.0), vec2(1.0,1.0), vec2(1.0,1.0), vec2(1.0,0.0), vec2(0.0,0.0),
    vec2(1.0,0.0), vec2(0.0,0.0), vec2(0.0,1.0), vec2(0.0,1.0), vec2(1.0,1.0), vec2(1.0,0.0), 
    vec2(1.0,1.0), vec2(1.0,0.0), vec2(0.0,0.0), vec2(0.0,0.0), vec2(0.0,1.0), vec2(1.0,1.0),
    vec2(0.0,1.0), vec2(1.0,1.0), vec2(1.0,0.0), vec2(1.0,0.0), vec2(0.0,0.0), vec2(0.0,1.0) };

layout (location = 0) out vec3 pos;

void main() 
{   
    uint id_vert = gl_VertexIndex % 6; 
    uint id_sq_out = gl_VertexIndex / 6;    
    uint id_in_face_out = id_sq_out % 9;
    uint id_face_out = id_sq_out / 9;    
    uint id_x_out = id_in_face_out % 3;
    uint id_y_out = id_in_face_out / 3;

    vec3 o_out = positions[id_face_out*3];
    vec3 ox_out = positions[id_face_out*3+1] - o_out;
    vec3 oy_out = positions[id_face_out*3+2] - o_out;

    vec2 idv_out = indices[id_vert];
    vec3 pos_out = o_out + ox_out *(float(id_x_out)+idv_out.x)/3.0 + oy_out *(float(id_y_out)+idv_out.y)/3.0;    

    vec4 wpos = vec4(pos_out, 1.0);
    vec4 ppos = matrix * wpos;    
    ppos.y = -ppos.y;
    ppos.z = (ppos.z + ppos.w) / 2.0;
    gl_Position = ppos;

    uint id_sq_in = get_value(map, id_sq_out);    
    uint id_in_face_in = id_sq_in % 9;
    uint id_face_in = id_sq_in / 9;    
    uint id_x_in = id_in_face_in % 3;
    uint id_y_in = id_in_face_in / 3;

    uint dir_in = get_value(dirs, id_sq_out);

    vec3 o_in = positions[id_face_in*3];
    vec3 ox_in = positions[id_face_in*3+1] - o_in;
    vec3 oy_in = positions[id_face_in*3+2] - o_in;

    vec2 idv_in = indices[6*dir_in + id_vert];
    vec3 pos_in = o_in + ox_in *(float(id_x_in)+idv_in.x)/3.0 + oy_in *(float(id_y_in)+idv_in.y)/3.0; 

    pos = pos_in;
}
''',
'''
layout (location = 0) in vec3 pos;
layout (location = 0) out vec4 outColor;

void main() 
{
    vec3 dir = normalize(pos);
    outColor = texture(arr_cubemap[0], dir);
}
'''))

    def set_background_color(self, bg_color):
        self.bg_color = [bg_color[0], bg_color[1], bg_color[2], 1.0]
        
    def set_camera(self, width, height, fovy, view_mat):
        self.width = width
        self.height = height
        proj_mat = glm.perspective(glm.radians(fovy), self.width/self.height, 0.1, 1000.0)
        self.matrix = proj_mat*view_mat

    def render(self, cube, filename):        
        colorBuf = vki.Texture2D(self.width, self.height, VK_FORMAT_R8G8B8A8_SRGB)
        colorBuf4x = vki.Texture2D(self.width, self.height, VK_FORMAT_R8G8B8A8_SRGB, samples=4)
        depthBuf4x = vki.Texture2D(self.width, self.height, VK_FORMAT_D32_SFLOAT, isDepth=True, samples=4)       
        
        gpu_matrix = vki.SVMat4x4(self.matrix)
        gpu_map = vki.device_vector_from_numpy(cube.map)
        gpu_dirs = vki.device_vector_from_numpy(cube.dirs)
        self.rp.launch([6*9*6], [colorBuf4x], depthBuf4x, self.bg_color, 1.0, [gpu_matrix, gpu_map, gpu_dirs], resolveBufs=[colorBuf], cubemaps = [self.skin])

        image_out = np.empty((self.height, self.width, 4), dtype=np.uint8)
        colorBuf.download(image_out)

        Image.fromarray(image_out, 'RGBA').save(filename)        


class TopView:
    def __init__(self, colors = [[0.0, 1.0, 0.0], [0.0, 0.0, 1.0], [1.0, 1.0, 0.0], [1.0, 1.0, 1.0], [1.0, 0.0, 0.0], [1.0, 0.2, 0.0], [0.2, 0.2, 0.2]]):
        self.wh = 512
        self.bg_color = [0.0,0.0,0.0,1.0]
        self.colors = np.array(colors, dtype = np.float32)
        self.rp = vki.Rasterizer(["map", "colors", "up_face_id"], type_locked=True)
        self.rp.add_draw_call(vki.DrawCall(
'''
const vec2 positions[6] = { vec2(0.0, 0.0), vec2(0.0, 1.0), vec2(1.0, 1.0), vec2(1.0, 1.0), vec2(1.0, 0.0), vec2(0.0, 0.0) };    

layout (location = 0) flat out uint v_id_face_in;

void main() 
{   
    uint id_vert = gl_VertexIndex % 6; 
    uint id_sq_out = gl_VertexIndex / 6;
    uint id_in_face_out, id_face_out;

    vec2 k = positions[id_vert];
    vec2 pos;

    if (id_sq_out<9)
    {
        id_in_face_out = id_sq_out;
        id_face_out = 2;        
        uint id_x_out = id_in_face_out % 3;
        uint id_y_out = id_in_face_out / 3;       

        pos.x = -0.77 + 0.52*float(id_x_out) + k.x*0.5;
        pos.y = -0.77 + 0.52*float(id_y_out) + k.y*0.5;
    }
    else if(id_sq_out<12)
    {
        id_in_face_out = id_sq_out - 9;
        id_face_out = 0;

        pos.x = 0.78 + k.x*0.05;
        pos.y = -0.77 + 0.52*float(2 - id_in_face_out) + k.y*0.5;
    }
    else if(id_sq_out<15)
    {
        id_in_face_out = id_sq_out - 12;
        id_face_out = 1;
        pos.x = -0.83 + k.x*0.05;
        pos.y = -0.77 + 0.52*float(id_in_face_out) + k.y*0.5;
    }
    else if(id_sq_out<18)
    {
        id_in_face_out = id_sq_out - 15;
        id_face_out = 4;
        pos.x = -0.77 + 0.52*float(id_in_face_out) + k.x*0.5;
        pos.y = 0.78 + k.y*0.05;

    }
    else
    {
        id_in_face_out = id_sq_out - 18;
        id_face_out = 5;
        pos.x = -0.77 + 0.52*float(2 - id_in_face_out) + k.x*0.5;
        pos.y = -0.83 + k.y*0.05;
    }
    
    gl_Position = vec4(pos, 0.5, 1.0);

    id_sq_out = id_face_out * 9 + id_in_face_out;
    uint id_sq_in = get_value(map, id_sq_out);
    v_id_face_in = id_sq_in / 9;
}
''',
'''
layout (location = 0) flat in uint v_id_face_in;
layout (location = 0) out vec4 outColor;

void main() 
{    
    if (up_face_id!=-1 && v_id_face_in != up_face_id)
    {    
        outColor = vec4(get_value(colors,6),1.0);
    }
    else
    {
        outColor = vec4(get_value(colors,v_id_face_in),1.0);
    }    
}
'''))

    def set_size(self, wh):
        self.wh = wh

    def set_background_color(self, bg_color):
        self.bg_color = [bg_color[0], bg_color[1], bg_color[2], 1.0]        

    def render(self, cube, filename, up_face_only=False, up_face_id = 2):                
        colorBuf = vki.Texture2D(self.wh, self.wh, VK_FORMAT_R8G8B8A8_SRGB)             
        
        gpu_map = vki.device_vector_from_numpy(cube.map)
        gpu_colors = vki.device_vector_from_numpy(self.colors)
        if not up_face_only:
            up_face_id = -1
        gpu_up_face_id = vki.SVInt32(up_face_id)
        self.rp.launch([(4*3+9)*6], [colorBuf], None, self.bg_color, 1.0, [gpu_map, gpu_colors, gpu_up_face_id])

        image_out = np.empty((self.wh, self.wh, 4), dtype=np.uint8)
        colorBuf.download(image_out)

        Image.fromarray(image_out, 'RGBA').save(filename) 


