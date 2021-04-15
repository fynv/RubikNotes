from RubikNotes import RubiksCube, TopView

OLL =  [
"F(RUR'U')F'",
"B'(R'U'RU)B",
"f(RUR'U')f'",
"b'(R'U'RU)b",
"RU2R'U'RU'R'",
"R'U2RUR'UR",

"(RU2)(R'U'RUR'U')(RU'R')",
"(R'U2)(RUR'U'RU)(R'UR)",
"R'U2(R'2U)(R'2U)(R'2U2)R'",
"(R2D')(RU2)(R'D)(RU2R)",
"x'RUR'z'(RU)(L'U'R')x",
"F'(rUR'U')(r'FR)",
"x'(RU'R'D)(RUR'D')x",

"F(RUR'U')(RUR'U')F'",
"B'(R'U'RU)(R'U'RU)B",
"f(RUR'U')(RUR'U')f'",
"b'(R'U'RU)(R'U'RU)b",

"F(RUR'U')F'f(RUR'U')f'",
"f(RUR'U')f'y'F(RUR'U')F'",
"b'(R'U'RU)byF(RUR'U')F'",

"F(RUR'U')F'yF(RUR'U')F'",
"B'(R'U'RU)By'B'(R'U'RU)B",

"(r'U2)(RUR'U)r",
"(rU2)(R'U'RU')r'",
"r'(U'RU'R')(U'2r)",
"r(UR'UR)(U2r')",

"(r'U2)(RUR'U')(RUR'U)r",
"(rU2)(R'U'RU)(R'U'RU')r'",

"(R'U'R)y'x'(RU')(R'F)(RUR')x",
"(LUL')yx'(L'U)(LF')(L'U'L)x",
"(rU'r'U')(rUr')zx(U'RU)z'",
"(l'UlU)(l'U'l)zx'(DR'D')z'",

"(rUR'U')(r'RU)(RU'R')",
"(RUR'U'r)(R'U)(RU'r')",

"(RU2R'U'RU'R')y'B'(R'U'RU)B",
"(R'U2RUR'UR)yF(RUR'U')F'",

"x'(RU')(R'2D)(R2U)(R'2D')Rx",
"x'(R'D)(R2U')(R'2D')(R2U)R'x",

"(R'U'RU)(RB'R'B)",
"B'(R'URUR'U')(RB)",
"(R'U'RU')(R'U)y'(R'U)(RB)",
"(R'U')(R'FRF')(UR)",
"(R'U'R U)x'z'(RU)(L'U')rR'",

"(RU'U')(R2'FRF')U2(R'FRF')",
"f(RUR'U')f'y2(R'U'RU')(R'U)y'(R'U)(RB)",

"(r'U'r)(R'U'RU)(r'Ur)",
"f(RUR'U')f'y'b'(R'U'RU)b",
"(rUr')(RUR'U')(rU'r')",
"b'(R'U'RU)byf(RUR'U')f'",

"(RU)(B'U')(R'URBR')",
"f(RUR'U')f'yx'RUR'z'(RU)(L'U'R')x",
"(L'U')(BU)(LU'L'B'L)",
"b'(R'U'RU)byx'RUR'z'(RU)(L'U'R')x",

"(rU'r'U'r)y(RUR'f')",
"B'(R'U'RU)Bx'RUR'z'(RU)(L'U'R')x",
"(l'UlUl')y'(L'U'Lf)",
"F(RUR'U')F'y2x'RUR'z'(RU)(L'U'R')x",

"(R'U'RU')(R'URU)(RB'R'B)",
"(RU2R'U'RU'R')y2b'(R'U'RU)b",
"(RUR'U)(RU'R'U')(R'FRF')",
"(R'U2RUR'UR)y2f(RUR'U')f'",

"(RU'2R2'FRF')(RU'2R')",
"f(RUR'U')f'y'(R'U2RUR'UR)",

"F(RUR'U')(RF')(rUR'U')r'",
"F(RUR'U')F'y'x'RUR'z'(RU)(L'U'R')x",

"(RUR'U')(RU'R'F'U'F)(RUR')",
"F(RUR'U')F'y2(R'U'RU)(RB'R'B)",
"b'(R'U'RU)by2F'(rUR'U')(r'FR)",

"(R2UR'B')(RU')(R2'U)(RBR')",
"f(RUR'U')f'yF'(rUR'U')(r'FR)",
"B'(R'U'RU)By2(R'U'RU)(RB'R'B)",

"(RUR'U)(R'FRF'U2)(R'FRF')",
"f(RUR'U')f'y'(R'U'RU)(RB'R'B)",

"F(RUR'd)(R'U2)(R'FRF')",
"F(RUR'U')F'yr'(U'RU'R')(U'2r)",

"(R'U2)(FRUR'U')y'(R2U2RB)",
"B'(R'URUR'U')(RB)y'B'(R'U'RU)B",

"r'(RU)(RUR'U'r2)(R2'U)(RU')r'",
"f(RUR'U')f'(R'U')(R'FRF')(UR)",

"(RU2)(R'2U')(RU'R'U2)(FRF')",
"(R'U')(R'FRF')(UR)yx'RUR'z'(RU)(L'U'R')x"

]


t_view = TopView()
t_view.set_size(256)

filename = "FeiOLL/images/oll_0.png"
print(f"Rendering {filename}")
cube = RubiksCube()
t_view.render(cube, filename, up_face_only=True)

for i in range(len(OLL)):
	filename = f"FeiOLL/images/oll_{i+1}.png"
	print(f"Rendering {filename}")
	seq = OLL[i]
	cube = RubiksCube()
	cube.exec_seq(seq, reverse=True)
	t_view.render(cube, filename, up_face_only=True)

