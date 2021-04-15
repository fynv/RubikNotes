import glm
from RubikNotes import RubiksCube, PerspectiveView, TopView

F2L = [
"(lDR'D'l')(U'RU)",
"(R'U'RU)(lDR'D')x",

"(FRF')(R'FR2F')(R'FRF')",
"(U'R'U)(RU'R'2U)(RU'R'U)",

"(FR'FRD)(R'D'F2)",
"(F2DR)(D'R'F'RF')",

"(U'R'U)(RU'R'U)",
"(FRF')(R'FRF')",

"(FR'F')(lDR'D')x",
"(U'RUR')(U'RU)",

"(FR2F'R)(FR2F'R)(U'R'U)",

#############################
"(lDR'D')x", # 简单 12-16
"(R'U'RU)",
"(FR'F')(U'R2U)", 
"(U'R'U)",
"(FRF')",

#############################
# 常用 17-29

# 棱块在中层
"(FRF'R')(FRF')(R'FRF')", # 转化，自反

"(x'R'DR'D')(R'2DR'D')x", # 有另一组
"(RU'RU)(R2U'RU)",

"(R'FRF')(RU'R'U)", # 转化
"(RU'R'U)(R'FRF')",

# 角块向上
"(FRF'R)(U'RU)(R'U'RU)", # 角块转化
"(U'R'UR')(FR'F')(lDR'D')x", 

"(U'R'2U)(RU'R'U)", # (角块)转化
"(FR2F')(R'FRF')", 

"(R'U'R2U)(R'U'RU)", # 棱块转化
"(RFR'2F')(lDR'D')x", 

"(R2U'R'U)(R'U'RU)", # 棱块转化， 原公式： (U'RU)R'2(U'R'U) # 自反， (FRF)(DRD'R')(F'RF') ->6
"(R'2FRF')(lDR'D')x",# 棱块转化， 原公式： (FR'F')R2(FRF') # 自反


#############################
# 高级 30 - 41

"(l'URU')l'2(DR'D')x",
"(RU'R'U)R'2(U'RU)",

"(l'UR2)(U'l'2)(DR'D')x",
"(RU'R2)(UR'2)(U'RU)",

"(U'lU'R)(Ul'R'U)", # 原公式 (U'RU)R2(FRF')
"(FR'DR')(D'R2F')", #  原公式 (FR'F')R'2(U'R'U)

"(R'FR'F'R)(U'R'U)", # 原公式 (RU'xl')(DR'D'R'D)x'2, 
"(RU'RUR')(FRF')", # 原公式 (l'URU'R)(UlF'),

"(l'UR2U'l)(U'R'U)",
"(RU'R'2UR')(FRF')",

"(RU'RUR')(U'R'U)",
"(R'FR'F'R)(FRF')"
]

OrangeD = True
RenderSequence = True

if OrangeD:	
	p_view = PerspectiveView(fn_skin="skin_fei_f2l.png")
else:
	p_view = PerspectiveView(fn_skin="skin_f2l.png")
	
p_view.set_camera(256, 256, 45, glm.lookAt(glm.vec3(8.0,6.0,10.0), glm.vec3(0.0,-0.5,0.0), glm.vec3(0.0, 1.0, 0.0)))

filename = "FeiF2L/images/f2l_0.png"
print(f"Rendering {filename}")
cube = RubiksCube()
cube.exec_seq("zx")
p_view.render(cube, filename)

for i in range(len(F2L)):
	filename = f"FeiF2L/images/f2l_{i+1}.png"
	print(f"Rendering {filename}")
	seq = F2L[i]
	cube = RubiksCube()
	cube.exec_seq("zx")
	cube.exec_seq(seq, reverse=True)
	if RenderSequence:
		cube.render_seq(p_view, seq, filename=filename, grouped = True)
	else:
		p_view.render(cube, filename)