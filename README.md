/////////////////////////////////////////////////////////////  
///////////////////CS430 Project 5////////////////////////  
/////////////////////Frankie Berry/////////////////////////  
//////////////////December 8, 2016//////////////////////  
////////////////////////////////////////////////////////////

This application is intended to take in a P6 .ppm input file and store it as a texture using OpenGL which is then displayed on the screen. The user is then able to perform a number of affine transformations using different keypresses.

##Keypress Shortcuts
* Translation
  - Translate left (Left arrow key)
  - Translate right (Right arrow key)
  - Translate up (Up arrow key)
  - Translate down (Down arrow key)
* Scale
  - Scale up (S key)
  - Scale down (D key)
* Rotation
  - Rotate clockwise (T key)
  - Rotate counter-clockwise (R key)
* Shear
  - Shear left (Z key)
  - Shear right (X key)
  - Shear up (C key)
  - Shear down (V key)
* Exit (ESC key)

##Running the Program
  This repository is meant to be run with Windows and assumes that the user has installed the GLES2 starter kit located at: https://bitbucket.org/jdpalmer/gles2-starter-kit

  Note that the program will not compile unless it's in the same directory as the files from the above link.

  To run this program, first open up a Visual Studios Developer Command Prompt, and type nmake in order to create the executable named "ezview"

  After the executable is created, it can be called directly from the command prompt along with a P6 .ppm file as the single argument.
  
  An example call to the executable using the included sample P6 .ppm file in this repository is:
```
ezview ex_p6_image.ppm
```
  Once the window opens, the image should be loaded and transformations can be performed by using the aforementioned keypress shortcuts, with the ESC key allowing an easy way to exit the program/window afterwards.

##Special Notes
Although there are no special notes regarding the usage of this program, I do want to mention some specific details regarding the affine transformations in the ezview program. As the project criteria did not specify any specific scalars for any of the transformations, I've chosen different values for each which I felt sufficiently demonstrated the different transformations in an obvious manner. For example, the rotation transformation will rotate the image 45 degrees either clockwise or counter-clockwise, whereas the scale transformation will scale the image either up or down by roughly a third of its current size.

In addition to the scalar values, I also want to mention that the project criteria similarly didn't specify the method in which the affine transformations were to be programmed. I took an approach which involves modifying the position vectors for the image's vertices located within the Vertex struct. Ultimately, as all of the given technical objectives have been met, I just wanted to point out a few aspects to my implementation method which I felt were important.

As a quick note, due to the professor stating that we are allowed to choose a specific .ppm file format (either P3 or P6), I want to clarify that I decided to choose the P6 format, and as mentioned before as well as throughout the comments in the ezview.c file, the program will not accept any other .ppm format and should flag an error accordingly.
