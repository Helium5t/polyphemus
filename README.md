# Polyphemus #
A small renderer used to test things for learning PBR and OpenGL. Definitely not commercially ready and not the best case study as there are most likely some memory leaks here and there.  
It implementes two different PBR models, one from the LearnOpenGL's website and one from the 2014 SIGGRAPH talk detailing the PBR model used by the Frostbite Engine. Other concepts from the PBR book were explored with this project. 

## Dependencies ##
 - OpenGL
 - ImGui
 - tinyGLTF (for older versions)
 - Assimp 
 - GLAD
 - GLFW
 - stb_image

 Literary References:
  - [Physically Based Rendering](https://www.pbr-book.org/)
  - [LearnOpenGL](learnopengl.com/PBR/Lighting)
  - [Siggraph 2014 Moving Frostbite to PBR](https://www.ea.com/frostbite/news/moving-frostbite-to-pb)

## Generic Model PBR ##
PBR taken from OpenGL.com. Uses lambertian model for the diffuse term, cook-torrance with GGX NDF for the specular term. 

## Frostbite PBR ##
Uses Disneys's Energy Conserving Diffuse Formulation.
For specular, GGX with Smith's height correlated masking function is used.