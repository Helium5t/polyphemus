# Polyphemus #
A small renderer used to just test things for learning PBR and OpenGL. Definitely not commercially ready and not the best case study as there are most likely some memory leaks here and there.  

## Dependencies ##
 - OpenGL
 - ImGui
 - tinyGLTF (for older versions)
 - Assimp 
 - GLAD
 - GLFW
 - stb_image

 Literary References:
  - Physically Based Rendering
  - LearnOpenGL
  - Siggraph 2014 Moving Frostbite to PBR

## Generic Model Scene ##
Uses Disneys's Energy Conserving Diffuse Formulation.
For specular, GGX with Smith's height correlated masking function is used.