//
//  ezview.c
//  CS430 Project 5
//
//  Frankie Berry
//

// pre-processor directives
#define GLFW_DLL 1

#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <assert.h>

// function prototypes
void translate(int dir); // translates the image depending on keyboard input

void rotate(int dir); // rotates the image depending on keyboard input

void scale(int dir); // scales the image depending on keyboard input

void shear(int dir); // shears the image depending on keyboard input

void read_header_data(char* input_file_name); // function meant to read and parse through the header information of a .ppm file

void read_p6_image_data(char* input_file_name); // function meant to read and parse through all iamge information located after the header information in a P6 formatted .ppm file

void skip_ws(FILE* json); // helper function to skip whitespace when reading .ppm file

// Vertex struct containing a Position vector and TexCoord vector
typedef struct {
  float Position[3];
  float TexCoord[2];
} Vertex;

// header_data buffer which is intended to contain all relevant header information from .ppm file
typedef struct header_data {
  char* file_format;
  char* file_height;
  char* file_width;
  char* file_maxcolor;
} header_data;

// image_data buffer which is intended to hold a set of RGB pixels represented as unsigned char's
typedef struct image_data {
  unsigned char r, g, b;
} image_data;

// global header_data buffer
header_data *header_buffer;

// global image_data buffer
image_data *image_buffer;

// global variable meant to track a file pointer, specifically in order to track where the header information in a .ppm file ends for convenience
int current_location;

// global variables intended to hold height/width variables after reading the information in from the header of the .ppm file
int image_height;
int image_width;

GLFWwindow* window;

Vertex Vertices[] =
{
  {{1, -1, 0},  {.9999, .9999}},
  {{1, 1, 0},   {.9999, 0}},
  {{-1, 1, 0},  {0, 0}},
  {{-1, -1, 0}, {0, .9999}}
};

const GLubyte Indices[] = {
  0, 1, 2,
  2, 3, 0
};
	
char* vertex_shader_src =
  "attribute vec4 Position;\n"
  "\n"
  "attribute vec2 TexCoordIn;\n"
  "varying lowp vec2 TexCoordOut;\n"
  "\n"
  "void main(void) {\n"
  "    TexCoordOut = TexCoordIn;\n"
  "    gl_Position = Position;\n"
  "}\n";

char* fragment_shader_src =
  "varying lowp vec2 TexCoordOut;\n"
  "uniform sampler2D Texture;\n"
  "\n"
  "void main(void) {\n"
  "    gl_FragColor = texture2D(Texture, TexCoordOut);\n"
  "}\n";

GLint simple_shader(GLint shader_type, char* shader_src) {

  GLint compile_success = 0;

  int shader_id = glCreateShader(shader_type);

  glShaderSource(shader_id, 1, &shader_src, 0);

  glCompileShader(shader_id);

  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_success);

  if (compile_success == GL_FALSE) {
    GLchar message[256];
    glGetShaderInfoLog(shader_id, sizeof(message), 0, &message[0]);
    printf("glCompileShader Error: %s\n", message);
    exit(1);
  }

  return shader_id;
}

int simple_program() {

  GLint link_success = 0;

  GLint program_id = glCreateProgram();
  GLint vertex_shader = simple_shader(GL_VERTEX_SHADER, vertex_shader_src);
  GLint fragment_shader = simple_shader(GL_FRAGMENT_SHADER, fragment_shader_src);
  
  glAttachShader(program_id, vertex_shader);
  glAttachShader(program_id, fragment_shader);

  glLinkProgram(program_id);

  glGetProgramiv(program_id, GL_LINK_STATUS, &link_success);

  if (link_success == GL_FALSE) {
    GLchar message[256];
    glGetProgramInfoLog(program_id, sizeof(message), 0, &message[0]);
    printf("glLinkProgram Error: %s\n", message);
    exit(1);
  }

  return program_id;
}

// translate function which translates the image slightly to either the upwards, downwards, right, or left direction depending on the arrow key entered
void translate(int dir)
{
    switch(dir)
    {
        case 0:
			Vertices[0].Position[1] += .25;
            Vertices[1].Position[1] += .25;
            Vertices[2].Position[1] += .25;
            Vertices[3].Position[1] += .25;
            break;
        case 1:
			Vertices[0].Position[1] += -.25;
            Vertices[1].Position[1] += -.25;
            Vertices[2].Position[1] += -.25;
            Vertices[3].Position[1] += -.25;
            break;
        case 2:
			Vertices[0].Position[0] += -.25;
            Vertices[1].Position[0] += -.25;
            Vertices[2].Position[0] += -.25;
            Vertices[3].Position[0] += -.25;
            break;
        case 3:
			Vertices[0].Position[0] += .25;
            Vertices[1].Position[0] += .25;
            Vertices[2].Position[0] += .25;
            Vertices[3].Position[0] += .25;
            break;
        default:
            printf("Invalid key recognized for translation.\n");
            break;
    }
}

// rotate function which rotates the image 90 degrees clockwise or counter-clockwise depending on key entered (R for counter-clockwise and T for clockwise)
void rotate(int dir)
{
	float angle = 1.5708; // 90 degrees in radians for math.h sin/cos function calls
	float rotation_matrix1[2][2] = {{cos(-angle), -sin(-angle)},
								   {sin(-angle), cos(-angle)}};
    float rotation_matrix2[2][2] = {{cos(angle), -sin(angle)},
								   {sin(angle), cos(angle)}};
	float temp_x;
	float temp_y;

	
    switch(dir)
    {
        case 0:
			temp_x = (Vertices[0].Position[0]*rotation_matrix1[0][0]) + (Vertices[0].Position[1]*rotation_matrix1[1][0]);
			temp_y = (Vertices[0].Position[0]*rotation_matrix1[0][1]) + (Vertices[0].Position[1]*rotation_matrix1[1][1]);
			Vertices[0].Position[0] = temp_x;
			Vertices[0].Position[1] = temp_y;
			
			temp_x = (Vertices[1].Position[0]*rotation_matrix1[0][0]) + (Vertices[1].Position[1]*rotation_matrix1[1][0]);
			temp_y = (Vertices[1].Position[0]*rotation_matrix1[0][1]) + (Vertices[1].Position[1]*rotation_matrix1[1][1]);
			Vertices[1].Position[0] = temp_x;
			Vertices[1].Position[1] = temp_y;
			
			temp_x = (Vertices[2].Position[0]*rotation_matrix1[0][0]) + (Vertices[2].Position[1]*rotation_matrix1[1][0]);
			temp_y = (Vertices[2].Position[0]*rotation_matrix1[0][1]) + (Vertices[2].Position[1]*rotation_matrix1[1][1]);
			Vertices[2].Position[0] = temp_x;
			Vertices[2].Position[1] = temp_y;
			
			temp_x = (Vertices[3].Position[0]*rotation_matrix1[0][0]) + (Vertices[3].Position[1]*rotation_matrix1[1][0]);
			temp_y = (Vertices[3].Position[0]*rotation_matrix1[0][1]) + (Vertices[3].Position[1]*rotation_matrix1[1][1]);
			Vertices[3].Position[0] = temp_x;
			Vertices[3].Position[1] = temp_y;		
            break;
        case 1:
			temp_x = (Vertices[0].Position[0]*rotation_matrix2[0][0]) + (Vertices[0].Position[1]*rotation_matrix2[1][0]);
			temp_y = (Vertices[0].Position[0]*rotation_matrix2[0][1]) + (Vertices[0].Position[1]*rotation_matrix2[1][1]);
			Vertices[0].Position[0] = temp_x;
			Vertices[0].Position[1] = temp_y;
			
			temp_x = (Vertices[1].Position[0]*rotation_matrix2[0][0]) + (Vertices[1].Position[1]*rotation_matrix2[1][0]);
			temp_y = (Vertices[1].Position[0]*rotation_matrix2[0][1]) + (Vertices[1].Position[1]*rotation_matrix2[1][1]);
			Vertices[1].Position[0] = temp_x;
			Vertices[1].Position[1] = temp_y;
			
			temp_x = (Vertices[2].Position[0]*rotation_matrix2[0][0]) + (Vertices[2].Position[1]*rotation_matrix2[1][0]);
			temp_y = (Vertices[2].Position[0]*rotation_matrix2[0][1]) + (Vertices[2].Position[1]*rotation_matrix2[1][1]);
			Vertices[2].Position[0] = temp_x;
			Vertices[2].Position[1] = temp_y;
			
			temp_x = (Vertices[3].Position[0]*rotation_matrix2[0][0]) + (Vertices[3].Position[1]*rotation_matrix2[1][0]);
			temp_y = (Vertices[3].Position[0]*rotation_matrix2[0][1]) + (Vertices[3].Position[1]*rotation_matrix2[1][1]);
			Vertices[3].Position[0] = temp_x;
			Vertices[3].Position[1] = temp_y;
            break;
        default:
            printf("Invalid key recognized for rotation.\n");
            break;
    }
}

// scale function which scales the image up or down by roughly a third of its original size depending on the key entered (S for scale up and D for scale down)	
void scale(int dir)
{
    switch(dir)
    {
        case 0:
			Vertices[0].Position[0] *= 1.3;
            Vertices[0].Position[1] *= 1.3;
            Vertices[1].Position[0] *= 1.3;
            Vertices[1].Position[1] *= 1.3; // manually scaling the x and y coordinates up by 1.3 its original size of the Position vectors for each vertex
            Vertices[2].Position[0] *= 1.3; // we don't care about the z coordinate
            Vertices[2].Position[1] *= 1.3;
            Vertices[3].Position[0] *= 1.3;
            Vertices[3].Position[1] *= 1.3;
            break;
        case 1:
			Vertices[0].Position[0] *= .77;
            Vertices[0].Position[1] *= .77;
            Vertices[1].Position[0] *= .77;
            Vertices[1].Position[1] *= .77; // manually scaling the x and y coordinates down by .77 its original size of the Position vectors for each vertex
            Vertices[2].Position[0] *= .77; // we don't care about the z coordinate
            Vertices[2].Position[1] *= .77;
            Vertices[3].Position[0] *= .77;
            Vertices[3].Position[1] *= .77;
            break;
        default:
            printf("Invalid key recognized for scale.\n");
            break;
    }
}

// shear function which slightly shears the image to the left/right/up/down depending on the key entered (Z for left, X for right, C for up, V for down)
void shear(int dir)
{
    switch(dir)
    {
        case 0:
			// shearing bottom two vertices
			Vertices[0].Position[0] += 0.25;
            Vertices[3].Position[0] += 0.25;
			// shearing top two vertices
			Vertices[2].Position[0] += -0.25;
            Vertices[1].Position[0] += -0.25;
            break;
        case 1:
			// shearing bottom two vertices
			Vertices[0].Position[0] += -0.25;
            Vertices[3].Position[0] += -0.25;
			// shearing top two vertices
			Vertices[2].Position[0] += 0.25;
            Vertices[1].Position[0] += 0.25;
            break;
		case 2:
			// shearing bottom two vertices
			Vertices[0].Position[1] += 0.25;
            Vertices[3].Position[1] += -0.25;
			// shearing top two vertices
			Vertices[2].Position[1] += -0.25;
            Vertices[1].Position[1] += 0.25;
            break;
        case 3:
			// shearing bottom two vertices
			Vertices[0].Position[1] += -0.25;
            Vertices[3].Position[1] += 0.25;
			// shearing top two vertices
			Vertices[2].Position[1] += 0.25;
            Vertices[1].Position[1] += -0.25;
            break;
        default:
            printf("Invalid key recognized for shear.\n");
            break;
    }
}

// arrow keys for translation
// r, t keys for rotation
// s, d keys for scale
// z, x, c, v keys for shear
// escape key to exit program
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
        translate(0);
    else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        translate(1);
    else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
        translate(2);
    else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
        translate(3);
	else if (key == GLFW_KEY_R && action == GLFW_PRESS)
        rotate(0);
	else if (key == GLFW_KEY_T && action == GLFW_PRESS)
        rotate(1);
	else if (key == GLFW_KEY_S && action == GLFW_PRESS)
        scale(0);
	else if (key == GLFW_KEY_D && action == GLFW_PRESS)
        scale(1);
	else if (key == GLFW_KEY_Z && action == GLFW_PRESS)
        shear(0);
	else if (key == GLFW_KEY_X && action == GLFW_PRESS)
        shear(1);
	else if (key == GLFW_KEY_C && action == GLFW_PRESS)
        shear(2);
	else if (key == GLFW_KEY_V && action == GLFW_PRESS)
        shear(3);
	else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);
}

static void error_callback(int error, const char* description) {
  fputs(description, stderr);
}

int main(int argc, char** argv) {
	if(argc != 2) // checks for "2" arguments which includes the argv[0] path argument as well as the 1 required argument of format [input.ppm]
	{
		fprintf(stderr, "Error: Incorrect number of arguments; format should be -> [input.ppm]\n");
		return -1;
	}
	
	char *input_name = argv[1]; // stores input file name given in input_name variable as char*
	
	// block of code which checks to make sure that user inputted a .ppm file for both the input and output command line arguments
	char* temp_ptr;
	int input_length = strlen(input_name);
	temp_ptr = input_name + (input_length - 4); // sets temp_ptr to be equal to the last 4 characters of the input_name, which should be .ppm
	if(strcmp(temp_ptr, ".ppm") != 0)
	{
		fprintf(stderr, "Error: Input file must be a .ppm file\n");
		return -1;
	}
	// end of .ppm extension error checking	
	
	// block of code which immediately checks to see if input file exists, whereas output file will be created if not
	FILE *fp;
	
	fp = fopen(input_name, "r");
	
	if(fp == NULL)
	{
		fprintf(stderr, "Error: File didn't open properly; filename may be incorrect or file may not exist.\n");
		return -1;
	}		
	fclose(fp);
	// end of input file error checking
	
	// block of code allocating memory to global header_buffer before its use
	header_buffer = (struct header_data*)malloc(sizeof(struct header_data)); 
	header_buffer->file_format = (char *)malloc(100);
	header_buffer->file_height = (char *)malloc(100);
	header_buffer->file_width = (char *)malloc(100);
	header_buffer->file_maxcolor = (char *)malloc(100);
	
	// function calls which start the bulk of the program, including reading header data, reading image data, and then writing out both that header and image data into a file
	read_header_data(input_name); // reads and parses header information

	// intermediate image_buffer memory allocation here as image_width and image_height were previously unavailable
	image_buffer = (image_data *)malloc(sizeof(image_data) * image_width * image_height  + 1);
	
	read_p6_image_data(input_name); // reads and stores image information from P6 file
	
	// start of OpenGL calls
	GLint program_id, position_slot;
	GLuint vertex_buffer;
	GLuint index_buffer;

	glfwSetErrorCallback(error_callback);

	// Initialize GLFW library
	if (!glfwInit())
		return -1;

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// Create and open a window using 1024x768 and the project name
	window = glfwCreateWindow(1024,
							  768,
							  "ezview - Image Viewer",
							  NULL,
							  NULL);

	if (!window) {
		glfwTerminate();
		printf("glfwCreateWindow Error\n");
		exit(1);
	}

	glfwMakeContextCurrent(window);
	
	// sets up texture ID and generates/binds as well as loads the read-in .ppm file as the texture
	GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, 
				 GL_UNSIGNED_BYTE, image_buffer);
	//
	
	program_id = simple_program();

	glUseProgram(program_id);

	// sets position_slot and asserts it was set correctly, followed by calling glEnableVertexAttribArray on it
	position_slot = glGetAttribLocation(program_id, "Position");
	assert(position_slot != -1);
	glEnableVertexAttribArray(position_slot);
	
	// sets texcoord_location and asserts it was set correctly
	GLint texcoord_location = glGetAttribLocation(program_id, "TexCoordIn");
	assert(texcoord_location != -1);
	
	// sets tex_location and asserts it was set correctly
	GLint tex_location = glGetUniformLocation(program_id, "Texture");
	assert(tex_location != -1);
	
	// calls glEnableVertexAtribArray on previously set texcoord_location, now that we know the tex_location was set properly
	glEnableVertexAttribArray(texcoord_location);

	// Create Buffer
	glGenBuffers(1, &vertex_buffer);

	// Map GL_ARRAY_BUFFER to this buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

	// Send the data
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);


	// Repeat
	while (!glfwWindowShouldClose(window)) {
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW); // updates image if any transformations were performed

		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// temp width and height variables to get frame buffer size
		int temp_width;
		int temp_height;
		
		glfwGetFramebufferSize(window, &temp_width, &temp_height); // grabs frame buffer size and stores width and height in respective temp variables

		glViewport(0, 0, temp_width, temp_height); // sets image size to fit window

		glVertexAttribPointer(position_slot,
							  3,
							  GL_FLOAT,
							  GL_FALSE,
							  sizeof(Vertex),
							  0);		
							  
		glVertexAttribPointer(texcoord_location,
							  2,
							  GL_FLOAT,
							  GL_FALSE,
							  sizeof(Vertex),
							  (void*) (sizeof(float) * 3));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID);
		glUniform1i(tex_location, 0);

		
		glDrawElements(GL_TRIANGLES,
					   sizeof(Indices) / sizeof(GLubyte),
					   GL_UNSIGNED_BYTE, 0);

		glfwSwapBuffers(window);
		glfwSetKeyCallback(window, key_callback);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

// read_header_data function takes in a sole input_file_name argument in order to know which file to read from
void read_header_data(char* input_file_name)
{
	FILE *fp;
	fp = fopen(input_file_name, "rb");
	
	// error checking again on input_file to validate its existence
	if(fp == NULL)
	{
		fprintf(stderr, "Error: File didn't open properly; filename may be incorrect or file may not exist.\n");
		exit(1); // exits out of program due to error
	}
	
	char* current_line; // character pointer used to read information from fgets
	current_line = (char *)malloc(1500); // allocated memory to current_line; doesn't need too much since a single line in a .ppm file shouldn't be too long
	char temp[64] = {0}; // temporary character array to store header information later on
	int c; // declares int c variable
	int i = 0; // initializes iterator variable

	
	skip_ws(fp); // skips any white space at beginning of file
	temp[i++] = fgetc(fp); // Should read in P 
	temp[i++] = fgetc(fp); // Should read in # in P# (in this project's case the 6 of P6)
	
	
	temp[i] = 0; // adds null-terminator at the end of the temporary character array
	strcpy(header_buffer->file_format, temp); // copies what should be the file_format (P6 in this project's case) into the header_buffer->file_format field
	memset(temp, 0, 64); // resets all values in temp to 0 for later use
	
	// error check to make sure .ppm file contains either P6 or p6 as its magic number
	if((strcmp(header_buffer->file_format, "P6") != 0) && (strcmp(header_buffer->file_format, "p6") != 0))
	{
		fprintf(stderr, "Error: Given file format for this project must be P6.\n");
		exit(1); // exits out of program due to error		
	}
	
	
	// determining file width
	i = 0; // resets iterator variable to 0
	
	skip_ws(fp);
	c = fgetc(fp);
	while(!isspace(c))
	{
		if(c == '#')
		{
			fgets(current_line, 1024, fp); // immediately reads to end of line if a comment is found
			skip_ws(fp);
			c = fgetc(fp); // new character grabbed from file for next while evaluation
		}
		temp[i++] = c; // stress non-white space character into temp chaacter array
        c = fgetc(fp);	// stores next character in file in c	
	}

	
	temp[i] = 0; // adds null-terminator at the end of the temporary character array
	strcpy(header_buffer->file_width, temp); // stores file width as a string in the global header_buffer
	image_width = atoi(temp); // converts read-in width to int for error checking and stores in global variable
	memset(temp, 0, 64); // resets all values in temp to 0 for later use
	
	
	
	// determining file height
	i = 0; // resets iterator variable to 0
	
	skip_ws(fp);
	c = fgetc(fp);
	while(!isspace(c))
	{
		if(c == '#')
		{
			fgets(current_line, 1024, fp); // immediately reads to end of line if a comment is found
			skip_ws(fp);
			c = fgetc(fp); // new character grabbed from file for next while evaluation
		}
		temp[i++] = c; // stress non-white space character into temp chaacter array
        c = fgetc(fp);	// stores next character in file in c	
	}
	
	temp[i] = 0; // adds null-terminator at the end of the temporary character array
	strcpy(header_buffer->file_height, temp); // stores file height as a string in the global header_buffer
	image_height = atoi(temp); // converts read-in height to int for error checking and stores in global variable
	memset(temp, 0, 64); // resets all values in temp to 0 for later use
	 
	// error check to make sure height and width are both greater than 0
	if(image_height < 0 || image_width < 0)
	{
		fprintf(stderr, "Error: Invalid height or width.\n");
		exit(1); // exits out of program due to error
	}
		
		
    // determining max color
	i = 0; // resets iterator variable to 0
	
	skip_ws(fp);
	c = fgetc(fp);
	while(!isspace(c))
	{
		if(c == '#')
		{
			fgets(current_line, 1024, fp); // immediately reads to end of line if a comment is found
			skip_ws(fp);
			c = fgetc(fp); // new character grabbed from file for next while evaluation
		}
		temp[i++] = c; // stress non-white space character into temp chaacter array
        c = fgetc(fp);	// stores next character in file in c	
	}
	
	temp[i] = 0; // adds null-terminator at the end of the temporary character array
	strcpy(header_buffer->file_maxcolor, temp); // stores file maxcolor as a string in the global header_buffer
	int maxcolor = atoi(temp); // converts read-in maxcolor to int for error checking
	memset(temp, 0, 64); // resets all values in temp to 0 for later use
	
	// error check to make sure max color fits within the correct color channel for this project
	if(maxcolor != 255)
	{
		fprintf(stderr, "Error: Image not an 8-bit channel (max color value is not 255)\n");
		exit(1); // exits out of program due to error	
	}
	
	skip_ws(fp);
	current_location = ftell(fp); // stores location of file pointer after header is read in for later use when reading image data	
	 
	fclose(fp);
}


// read_p6_image_data function takes in a sole input_file_name argument in order to know which file to read from
void read_p6_image_data(char* input_file_name)
{	
	// strcmp to verify type of input file format
	if((strcmp(header_buffer->file_format, "P6") == 0) || (strcmp(header_buffer->file_format, "p6") == 0))
	{	
		int i = 0; // initializes iterator variable
		image_data current_pixel; // temp image_data struct which will hold RGB pixels
		image_data* temp_ptr = image_buffer; // temp ptr to image_data struct which will be used to navigate through global buffer
		current_pixel.r = '0';
		current_pixel.g = '0'; // initializes current pixel RGB values to 0
		current_pixel.b = '0';
				
		
		FILE* p6_file = fopen(input_file_name, "rb"); // opens file to be able to read in bytes
		fseek(p6_file, current_location, SEEK_SET); // sets file pointer to previously calculated current_location global variable
		skip_ws(p6_file); // skips any white space after the header
		
		// while loop which iterates for every pixel in the file using width * height
		while(i < atoi(header_buffer->file_width) * atoi(header_buffer->file_height))
		{
			fread(&current_pixel.r, sizeof(unsigned char), 1, p6_file); // reads a byte "unsigned char" pixel into current_pixel.r field
			fread(&current_pixel.g, sizeof(unsigned char), 1, p6_file); // reads a byte "unsigned char" pixel into current_pixel.g field
			fread(&current_pixel.b, sizeof(unsigned char), 1, p6_file); // reads a byte "unsigned char" pixel into current_pixel.b field

			*temp_ptr = current_pixel; // effectively stores current pixel in temporary buffer
			temp_ptr++; // increments temp_ptr to point to next image_data struct in global buffer
			
			// error checking block for each individual pixel to make sure they're not outside the color range limit
			if(current_pixel.r < 0 || current_pixel.r  > atoi(header_buffer->file_maxcolor))
			{
				fprintf(stderr, "Error: Invalid color value in given file (RGB value not between 0-%d).\n", atoi(header_buffer->file_maxcolor));
				exit(1); // exits out of program due to error				
			}
			
			if(current_pixel.g < 0 || current_pixel.g > atoi(header_buffer->file_maxcolor))
			{
				fprintf(stderr, "Error: Invalid color value in given file (RGB value not between 0-%d).\n", atoi(header_buffer->file_maxcolor));
				exit(1); // exits out of program due to error				
			}
			
			if(current_pixel.b < 0 || current_pixel.b > atoi(header_buffer->file_maxcolor))
			{
				fprintf(stderr, "Error: Invalid color value in given file (RGB value not between 0-%d).\n", atoi(header_buffer->file_maxcolor));
				exit(1); // exits out of program due to error				
			}
			
			current_pixel.r = '0';
			current_pixel.g = '0'; // resets current pixel RGB values to 0
			current_pixel.b = '0';
			i++; // increments iterator variable
		}
		
		fclose(p6_file); // closes p6 file after reading
	}
	
	// file format was neither P3 nor P6 so exits with error
	else
	{
		fprintf(stderr, "Error: File format to read in not recognized. For this project, file format of choice should be P6\n");
		exit(1); // exits out of program due to error	
	}
}

// skip_ws() skips white space in the file.
void skip_ws(FILE* input_file) 
{
  int c = fgetc(input_file);
  while (isspace(c)) {
    c = fgetc(input_file);
  }
  ungetc(c, input_file);
}