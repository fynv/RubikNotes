import glm
from RubikNotes import RubiksCube, PerspectiveView, TopView

F2L = [
"(R U'U' R' U) (R U'U' R' U) y'(R' U' R) y",
"(U R U' R' U') y'(R' U R) y",
"(R' F' RU)(R U' R' F)",
"(R U R' U')(R U'U' R' U')(R U R')",
"(R U' R U)y(R U' R' F2)y'",
"y'(R' U' R U)(R' U' R)y",
"(R U' R' U)(R U' R')",
"(RU'R'U)(R U'U' R'U)(RU'R')",
"R2 y(R U R' U')y'(R' U R')",
"y' (R' U)(R U')(R' U R) y",
"(R U R' U' )(R U R') ",
"(R U R' U') (R U R' U') (R U R')",
"(R U' R') y' (R' U2 R) y",
"y' (R' U2)(R U R' U')R y",
"y' U'(R' U2)(R U' R' U)R y",
"y'(R' U R U'U')(R' U' R) y",
"y' (U2 R2' U2)(R U R' U R2) y",
"(R U R')U2(R U R' U')(R U R')",
"(R U' R' U2)(R U R')",
"U(R U'U')(R' U R U' )R'",
"(R U'U')(R' U' R U)R'",
"U'(R U')(R' U2)(R U' R')",
"U'(RUR')U(F'U'F)",
"d (R' U R U')(R' U' R) y",
"y' (R' U' R) y",
"(d R' U' R U')(R' U' R) y",
"y' (R U'U')R'2 U' R2 U' R' y",
"y' (R' U)(R d' U')(R U R') ",
"U'(R U'U')(R'U2)(RU'R')",
"U'(R U R' U')(R U'U' R')",
"URU'R'",
"U' (R U'U' R' U)(R U R')",
"d (R' U' R) d' (R U R')",
"y' U' (R' U R) y",
"(d R' U' R U'U')(R' U R) y",
"d (R' U2)(R U'U')(R' U R) y",
"(R U' R' U)(d R' U' R) y",
"(R'U2)(R2'U)(R2'UR)",
"U'(R U R' U)(R U R')",
"(R U R')",
"U' (R U' R' U)(R U R')"]


OLL = [
"(R U'U')(R2' F R F')U2(R' F R F')",
"(F R U R' U' F')(f R U R' U' f')",
"f(RUR'U')f'U'F(RUR'U')F'",
"f(RUR'U')yx(R'F)(RUR'U')F'",
"(r' U2)(R U R'U) r",
"(rU'U')(R'U'RU'r')",
"rUR'URU'U'r'",
"r'U'RU'R'U2r",
"(R' U' R) y' x' (R U')(R'F)(R U R') x y",
"(RUR'U)(R'FRF')(RU'U'R')",
"r'(R2UR'U)(RU'U'R'U)(rR')",
"(rR'2U'RU')(R'U2RU'R)r'",
"(rU'r'U')(rUr')(F'UF)",
"R' F R U R' F'R (F U' F')",
"(r' U' r)(R'U'R U)(r' U r)",
"(r U r')(R U R' U')(r U' r')",
"(R U R' U)(R' F R F'U2)R' F R F'",
"F (R U R' d)(R' U2)(R' F R F')",
"R' U2 F R U R'U' y'R2 U'U' R B",
"r'(R U)(R U R'U' r2)(R2'U) (R U') r'",
"(R U'U')(R' U' R U R' U') (R U' R')",
"R U'U' (R'2 U')(R2 U')R'2 U' U'R",
"(R2 D') (R U'U') (R' D) (R U'U' R)",
"x' R U R' z'(R U)(L' U' R') x",
"F'(rUR'U')(r'FR)",
"R U'U' R' U'R U' R'",
"R' U2 R U R' U R",
"(r U R' U')(r' R U)(R U' R')",
"(R U R' U')(R U' R' F' U' F)(R U R')",
"(R2 U R' B')(RU')(R2' U)(R B R')",
"(r'F'UF)(LF'L'U'r)",
"(RU)(B'U')(R'URBR')",
"(R U R' U')(R' F R F')",
"(R'U'R U) x' z'(R U)(L'U')r R'",
"R U'U'R2' F R F'(R U'U'R')",
"R'U'R U' R'U R U l U'R'U x",
"F (R U' R'U'R U) (R' F')",
"(R U R'U)(RU'R'U')(R'F R F')",
"(r U' r' U' r)y(R U R' f')",
"(R' F R U R'U')(F' U R)",
"RU'R'U2RUyRU'R'U'F'",
"(R'U2)(R U R' U R2)y(R U R' U')F'",
"(B' U')(R' U R B)",
"f (R U R' U')f'",
"F(RUR'U')F'",
"(R' U') R' F R F' (U R)",
"B'(R' U' R U) (R' U' R U) B",
"F (R U R' U') (R U R' U') F'",
"R B'(R2 F)(R2 B) R2 F' R",
"L'B (L2 F')(L2B')L2 F L'",
"f (R U R' U') (R U R' U') f'",
"R'U' R U' R' d R' U l U x",
"(r' U2)(R U R'U')(R U R'U) r ",
"(r U'U')(R' U' R U R' U')(R U' r')",
"(R U'U') (R'2 U') R U' R'U2 (F R F')",
"F (R U R'U')(R F')(r U R'U')r'",
"(R U R' U' r)(R' U)(R U' r')"]

PLL = [
"(R U' R)(U R U R)(U' R' U' R2)",
"(R2' U)(R U R' U')(R' U')(R' U R')",
"L2(l'2 U)L2(l'2 U2)L2(l'2 U)L2 l2",
"(U R'U')(R U'R)(U R U')(R'U R U)(R2 U')(R'U)",
"x' R2 D2(R' U' R)D2(R' U R') x",
"(l U' R)D2(R' U R)D2 R2 x",
"x'(R U' R' D)(R U R')u2'(R' U R)D(R' U' R) x' y2",
"(R U R' U')(R' F)(R2 U' R' U')(R U R' F')",
"U'(R'U R U' R'2 b')x(R'U R)y'(R U R' U' R2) x'",
"(R' U R' U')yx2(R' U R' U'R2)xz'(R'U'R U R) x",
"F(R U'R' U')(R U R' F')(R U R' U')(R' F R F')",
"z(U'RD')(R2UR'U'R2U)z'(RU')",
"(R U R'F')(R U R'U')(R'F R2 U'R'U')",
"(R' U2)(R U'U')(R' F R U R' U')(R'F' R2 U')",
"(R U'U')(R' U2)(R B' R' U')(R U R B R2' U)",
" (R'2 u' R U' R)(U R' u)(R2 f R' f')",
"(R U R')y'(R2' u' R U')(R' U R' u R2) y",
"(R2 u)(R' U R' U')(R u') (R2' F' U F)",
"(R' d' F)(R2 u)(R' U)(R U' R u' R2) y'",
"z(R' U R')z'(R U2 L' U R') z(U R')z'(R U2 L' U R')",
"z(U'R D')(R2 U R'U')z'(R U R')z(R2 U R')z'(R U')"]

p_view = PerspectiveView(fn_skin = "skin_f2l.png")
p_view.set_camera(256, 256, 45, glm.lookAt(glm.vec3(8.0,6.0,10.0), glm.vec3(0.0,-0.5,0.0), glm.vec3(0.0, 1.0, 0.0)))
t_view = TopView()
t_view.set_size(256)


# F2L
for i in range(len(F2L)):
	filename = f"CFOPRemade/images/f2l_{i+1}.png"
	print(f"Rendering {filename}")
	seq = F2L[i]
	cube = RubiksCube()
	cube.exec_seq(seq, reverse=True)
	p_view.render(cube, filename)
	
for i in range(len(OLL)):
	filename = f"CFOPRemade/images/OLL_{i+1}.png"
	print(f"Rendering {filename}")
	seq = OLL[i]
	cube = RubiksCube()
	cube.exec_seq(seq, reverse=True)
	t_view.render(cube, filename, up_face_only=True)

for i in range(len(PLL)):
	filename = f"CFOPRemade/images/PLL_{i+1}.png"
	print(f"Rendering {filename}")
	seq = PLL[i]
	cube = RubiksCube()
	cube.exec_seq(seq, reverse=True)
	t_view.render(cube, filename)




