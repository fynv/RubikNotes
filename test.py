import glm
from RubikNotes import RubiksCube, PerspectiveView, TopView

cube = RubiksCube()
# cube.exec_seq("z2U'DF'BR'LU'D")

# cube.exec_seq("RUR'U'F'U2FURUR'", reverse=True) # OLL 36
cube.exec_seq("(RU'U')(R2'FRF')U2(R'FRF')", reverse=True)


p_view = PerspectiveView()
p_view.set_camera(256, 256, 45, glm.lookAt(glm.vec3(8.0,6.0,10.0), glm.vec3(0.0,-0.5,0.0), glm.vec3(0.0, 1.0, 0.0)))
p_view.render(cube, 'perspective.png')

t_view = TopView()
t_view.set_size(256)
t_view.render(cube, 'top.png', up_face_only=True)

cube.render_seq(p_view, "(RU'U')(R2'FRF')U2(R'FRF')", filename="full.png", grouped = True)

