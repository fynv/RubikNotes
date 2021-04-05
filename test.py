import glm
from RubikNotes import RubiksCube, PerspectiveView, TopView

cube = RubiksCube()
cube.exec_seq("x2y2U'DF'BR'LU'D")

p_view = PerspectiveView()
p_view.set_camera(512, 512, 45, glm.lookAt(glm.vec3(8.0,6.0,10.0), glm.vec3(0.0,-0.5,0.0), glm.vec3(0.0, 1.0, 0.0)))
p_view.render(cube, 'perspective.png')

t_view = TopView()
t_view.set_size(512)
t_view.render(cube, 'top.png')



