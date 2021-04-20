from RubikNotes import RubiksCube, TopView

PLL = [
"(L'UR'U2)(LU'L'U2)(LRU')",
"(RU'LU2)(R'URU2)(R'L'U)",

"z'(UL'UL)(ULUL')(U'L'U2)z",
"z(U'RU'R')(U'R'U'R)(URU2)z'",
"(F2U'LR')(F2L'RU')F2",
"(F2ULR')(F2L'RU)F2",
"f(RUR'U')f'y2B'(R'U'RU)By2",
"b'(R'U'RU)by2F(RUR'U')F'y2",

"(RU2)(R'U2)(RB'R'U')(RURBR'2U)",
"(L'U2)(LU2)(L'BLU)(L'U'L'B'L2U')",

"(r'2R2U)(r'2R2U2)(r'2R2U)(r'2R2)",
"(R'U')(R'FRF')(UR)b'(R'U'RU)b",

"x'R2D2(R'U'R)D2(R'UR')x",
"(lU'R)D2(R'UR)D2R'2x",
"x'(RU'R'D)(RUR'D')RUR'z'(RU)(L'U'R')zx",

"(R'U'RU)(RB'R'2U)(RUR'U')(RB)",
"B'(R'URUR'U')(RB)(R'U'RU)(RB'R'B)",

"(U'R'U)(RU'R'l')d'(R'UR)y'(RUR'U'R2)x'",
"(R'Ul'f')'x(R'Ul'f')U2(R'U'RUR)x",

"(L'UR'U2)(LU'L')(RUR'U2)(LU'RU')",
"(RU'LU2)(R'UR)(L'U'LU2)(R'UL'U)",

"(RUR')y'(R2u'RU')(R'UR'uR'2)y",
"y'(R2u'RU'R)(UR'uR'2)y(RU'R')",
"(L'U'L)y'(R'2uR'U)(RU'Ru'R2)y",
"y'(R'2uR'UR')(U'Ru'R2)y(L'UL)"

]

t_view = TopView()
t_view.set_size(256)

filename = "FeiPLL/images/pll_0.png"
print(f"Rendering {filename}")
cube = RubiksCube()
t_view.render(cube, filename)

for i in range(len(PLL)):
	filename = f"FeiPLL/images/pll_{i+1}.png"
	print(f"Rendering {filename}")
	seq = PLL[i]
	cube = RubiksCube()
	cube.exec_seq(seq, reverse=True)
	t_view.render(cube, filename)

