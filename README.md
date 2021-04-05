# Some Rubic's Cube Notes

## Content

* RubikNotes.py: A script for generating the illustrations.
* [Gan's CFOP](CFOPRemade): A quite old version of Gan's CFOP, remade.

## Usage of RubikNotes.py

Install dependencies:

```
$ pip install pillow
$ pip install VkInline
```
VkInline requires Win64/Linux64 with Vulkan enabled GPU driver.

Run the following example:

```python
import glm
from RubikNotes import RubiksCube, PerspectiveView, TopView

# Create a Rubik's cube state holder
cube = RubiksCube() 

'''
Execute a sequence to change the state of the cube
Single-layer Clockwise: RLUDFB
Dual-layer Clockwise: rludfb
Whole-cube rotate: xyz
Counterclockwise: add an apostrophe R'L'U'...
Rotate twice: add a '2' R2 L2 R'2

Optional: 
Setting parameter reverse=True will reverse the execution. This would be helpful if you are making an illustration for a tutorial.
'''
cube.exec_seq("x2y2U'DF'BR'LU'D")

'''
Create a perspective-viewing renderer

Optional:
Setting parameter fn_skin will change the texture of the cube. It can be any image file.
'''
p_view = PerspectiveView()

# Setting a camera, resolution 512x512, fovy = 45deg, looking from (8,6,10) to (0,-0.5,0)
p_view.set_camera(512, 512, 45, glm.lookAt(glm.vec3(8.0,6.0,10.0), glm.vec3(0.0,-0.5,0.0), glm.vec3(0.0, 1.0, 0.0)))

# Render the cube in perspective view
p_view.render(cube, 'perspective.png')

# Create a top-viewing renderer
t_view = TopView()

# Set resolution to 512x512, this one is always a square.
t_view.set_size(512)

'''
Render the cube in top view

Optional:
Setting parameter up_face_only=True will turn all faces except for the upper face into grey.
'''
t_view.render(cube, 'top.png')
```


## License
Consider it as public domain.
