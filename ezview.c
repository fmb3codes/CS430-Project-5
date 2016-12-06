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
#include <ctype.h>
#include <assert.h>

// function prototypes
void read_header_data(char* input_file_name); // function meant to read and parse through the header information of a .ppm file

void read_image_data(char* input_file_name); // function meant to read and parse through all image information located after the header information in a .ppm file

void skip_ws(FILE* json);  // helper function to skip whitespace when reading .ppm file

void print_pixels(); // to be removed later (helper function)


// remove color, add textcoord?
typedef struct {
  float Position[3];
  float Color[4];
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


const Vertex Vertices[] =
{
  {{0.5, -0.5, 0}, {1, 0, 0, 1}, {.9999,0}},
  {{0.5, 0.5, 0}, {0, 1, 0, 1}, {.9999,.9999}},
  {{-0.5, 0.5, 0}, {0, 0, 1, 1}, {0,.9999}},
  {{-0.5, -0.5, 0}, {0, 0, 0, 1}, {0,0}}
};

const GLubyte Indices[] = {
  0, 1, 2,
  2, 3, 0
};
	

char* vertex_shader_src =
  "attribute vec4 Position;\n"
  "attribute vec4 SourceColor;\n"
  "\n"
  "attribute vec2 TexCoordIn;\n"
  "varying lowp vec2 TexCoordOut;\n"
  "\n"
  "varying lowp vec4 DestinationColor;\n"
  "\n"
  "void main(void) {\n"
  "    TexCoordOut = TexCoordIn;\n"
  "    DestinationColor = SourceColor;\n"
  "    gl_Position = Position;\n"
  "}\n";


char* fragment_shader_src =
  "varying lowp vec4 DestinationColor;\n"
  "\n"
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
	//printf("shader_type is: %d and shade_src is: %s\n", shader_type, shader_src); // testing code
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

// ADD PROTOTYPES


void translate(int dir)
{
    switch(dir)
    {
        case 0:
            printf("Up arrow key recognized.\n");
            break;
        case 1:
            printf("Down arrow key recognized.\n");
            break;
        case 2:
            printf("Left arrow key recognized.\n");
            break;
        case 3:
            printf("Right arrow key recognized.\n");
            break;
        default:
            printf("Invalid key recognized for translation.\n");
            break;
    }
}

void rotate(int dir)
{
    switch(dir)
    {
        case 0:
            printf("R key recognized.\n");
            break;
        case 1:
            printf("T key recognized.\n");
            break;
        default:
            printf("Invalid key recognized for rotation.\n");
            break;
    }
}

void scale(int dir)
{
    switch(dir)
    {
        case 0:
            printf("S key recognized.\n");
            break;
        case 1:
            printf("D key recognized.\n");
            break;
        default:
            printf("Invalid key recognized for scale.\n");
            break;
    }
}

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
}

static void error_callback(int error, const char* description) {
  fputs(description, stderr);
}

// 	CONFIRM IF P3 OR P6 AND CHANGE ERROR CODES IN READ_DATA FUNCTIONS TO INDICATE ONE OR THE OTHER


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
	image_buffer = (image_data *)malloc(sizeof(image_data) * image_width * image_height  + 1); // + 1
	
	read_image_data(input_name); // reads and stores image information
	printf("Done reading .ppm file.\n");
	
	//print_pixels(); // testing code
	//exit(1);
	
	
	
	
	
	
	// start of OpenGL calls
	GLint program_id, position_slot, color_slot;
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

	// Create and open a window
	window = glfwCreateWindow(640,//atoi(header_buffer->file_width),
							  480,//atoi(header_buffer->file_height),
							  input_name, // change this?
							  NULL,
							  NULL);

	if (!window) {
		glfwTerminate();
		printf("glfwCreateWindow Error\n");
		exit(1);
	}

	glfwMakeContextCurrent(window);
	
	// potentially move this after executing simple_program()
	// sets up texture ID
	GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, 
				 GL_UNSIGNED_BYTE, image_buffer);
	////
	
	
	program_id = simple_program();

	//
	// potentially bind textures here
	//
	
	glUseProgram(program_id);

	position_slot = glGetAttribLocation(program_id, "Position");
	color_slot = glGetAttribLocation(program_id, "SourceColor");
	glEnableVertexAttribArray(position_slot);
	glEnableVertexAttribArray(color_slot);
	
	// potentially add other stuff around here
	GLint texcoord_location = glGetAttribLocation(program_id, "TexCoordIn");
	//assert(texcoord_location != -1); temp removal of this assertion
	GLint tex_location = glGetUniformLocation(program_id, "Texture"); // maybe swap with below call glEnable...
	//assert(tex_location != -1); temp removal of this assertion
	glEnableVertexAttribArray(texcoord_location);
	///
	

	// Create Buffer
	glGenBuffers(1, &vertex_buffer);

	// Map GL_ARRAY_BUFFER to this buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

	// Send the data
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	// Add texture stuff here?

	// Repeat
	while (!glfwWindowShouldClose(window)) {

		glClearColor(0, 104.0/255.0, 55.0/255.0, 1.0); // may change this later
		glClear(GL_COLOR_BUFFER_BIT);

		glViewport(0, 0, 640, 480);

		glVertexAttribPointer(position_slot,
							  3,
							  GL_FLOAT,
							  GL_FALSE,
							  sizeof(Vertex),
							  0);

		glVertexAttribPointer(color_slot,
							  4,
							  GL_FLOAT,
							  GL_FALSE,
							  sizeof(Vertex),
							  (GLvoid*) (sizeof(float) * 3));
		// added texture stuff here					  
		glVertexAttribPointer(texcoord_location,
							  2,
							  GL_FLOAT,
							  GL_FALSE,
							  sizeof(Vertex),
							  (void*) (sizeof(float) * 7)); // modify last parameter? * 7 due to offset
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID);
		glUniform1i(tex_location, 0);
		////
		
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
	
	fp = fopen(input_file_name, "r");
	
	// error checking again on input_file to validate its existence
	if(fp == NULL)
	{
		fprintf(stderr, "Error: File didn't open properly; filename may be incorrect or file may not exist.\n");
		exit(1); // exits out of program due to error
	}
	
	char* current_line; // character pointer used to read information from fgets
	current_line = (char *)malloc(1500); // allocated memory to current_line; doesn't need too much since a single line in a .ppm file shouldn't be too long
	char temp[64] = {0}; // temporary character array to store header information later on
	int c;// = fgetc(fp); // initializes int c to the first character in the input file
	int i = 0; // initializes iterator variable

	
	skip_ws(fp);
	temp[i++] = fgetc(fp); // Should read in P 
	temp[i++] = fgetc(fp); // Should read in # in P#
	
	
	temp[i] = 0; // adds null-terminator at the end of the temporary character array
	strcpy(header_buffer->file_format, temp); // copies what should be the file_format (P3/P6) into the header_buffer->file_format field
	memset(temp, 0, 64); // resets all values in temp to 0 for later use
	
	// error check to make sure .ppm file contains either P3 or P6 as its magic number
	if((strcmp(header_buffer->file_format, "P3") != 0) && (strcmp(header_buffer->file_format, "P6") != 0))
	{
		fprintf(stderr, "Error: Given file format is neither P3 nor P6.\n");
		exit(1); // exits out of program due to error		
	}
	
	
	// determining file width
	i = 0; // resets iterator variable to 0
	//c = fgetc(fp); // grabs the next character in the file
	
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
	//c = fgetc(fp); // grabs the next character in the file
	
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
	//c = fgetc(fp); // grabs the next character in the file
	
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

// read_image_data function takes in a sole input_file_name argument in order to know which file to read from
void read_image_data(char* input_file_name)
{
	FILE *fp;
	
	fp = fopen(input_file_name, "r");
		
	// error checking again on input_file to validate its existence
	if(fp == NULL)
	{
		fprintf(stderr, "Error: File didn't open properly; filename may be incorrect or file may not exist.\n");
		exit(1); // exits out of program due to error
	}
		
	// fseek points file pointer to space right after header information in the input file, so that the image_data buffer only reads in image data
	fseek(fp, current_location - 3, SEEK_SET); // POTENTIALLY CHANGE THIS
	
	// strcmp to check for type of input file format
	if(strcmp(header_buffer->file_format, "P3") == 0)
	{
		char temp[64] = {0}; // temporary character array to store header information later on
		int c; // declares int c to be used later on in order to store next character in file
		int current_number = 0; // used hold the atoi value of the number read in
		image_data current_pixel; // temp image_data struct which will hold RGB pixels
		image_data* temp_ptr = image_buffer; // temp ptr to image_data struct which will be used to navigate through global buffer
		current_pixel.r = '0';
		current_pixel.g = '0'; // initializes current pixel RGB values to 0
		current_pixel.b = '0';
		
		while(1)
		{
			if(feof(fp))
				break;
			skip_ws(fp);
			fgets(temp, 5, fp);
			current_number = atoi(temp);
			if(current_number < 0 || current_number > 255)
			{
				fprintf(stderr, "Error: Invalid color value in given file (RGB value not between 0-255).\n");
				exit(1); // exits out of program due to error				
			}
			current_pixel.r = current_number; // stores red value
			
			//skip_ws(fp);
			fgets(temp, 5, fp);
			current_number = atoi(temp);
			if(current_number < 0 || current_number > 255)
			{
				fprintf(stderr, "Error: Invalid color value in given file (RGB value not between 0-255).\n");
				exit(1); // exits out of program due to error				
			}
			current_pixel.g = current_number; // stores green value
			
			skip_ws(fp);
			fgets(temp, 5, fp);
			current_number = atoi(temp);
			if(current_number < 0 || current_number > 255)
			{
				fprintf(stderr, "Error: Invalid color value in given file (RGB value not between 0-255).\n");
				exit(1); // exits out of program due to error				
			}
			current_pixel.b = current_number; // store blue value
			
			*temp_ptr = current_pixel; // effectively stores current pixel in temporary buffer
			temp_ptr++; // increments temp_ptr to point to next image_data struct in global buffer
			current_pixel.r = '0';
			current_pixel.g = '0'; // resets current pixel RGB values to 0
			current_pixel.b = '0';
			skip_ws(fp);
		}

		fclose(fp);
	}
	
	// strcmp to check for type of input file format
	else if(strcmp(header_buffer->file_format, "P6") == 0)
	{	
		char* current_line; // character pointer used to read information from fgets
		current_line = (char *)malloc(1500); // allocated memory to current_line; doesn't need too much since a single line in a .ppm file shouldn't be too long
		char temp[64] = {0}; // temporary character array to store header information later on
		//int c = fgetc(fp); // initializes int c to the first character in the input file
		int i = 0; // initializes iterator variable
		int current_number = 0; // used hold the atoi value of the number read in
		image_data current_pixel; // temp image_data struct which will hold RGB pixels
		image_data* temp_ptr = image_buffer; // temp ptr to image_data struct which will be used to navigate through global buffer
		current_pixel.r = '0';
		current_pixel.g = '0'; // initializes current pixel RGB values to 0
		current_pixel.b = '0';
		
		
		skip_ws(fp);
		current_location = ftell(fp);
		
		fclose(fp); // after using fgetc to verify white space after header is gone, closes file
		fopen(input_file_name, "rb"); // reopens file to be able to read in bytes
		fseek(fp, current_location, SEEK_SET); // sets file pointer to previously calculated current_location global variable
		
		
		// while loop which iterates for every pixel in the file using width * height
		while(i < atoi(header_buffer->file_width) * atoi(header_buffer->file_height))
		{
			fread(&current_pixel.r, sizeof(unsigned char), 1, fp); // reads a byte "unsigned char" pixel into current_pixel.r field
			fread(&current_pixel.g, sizeof(unsigned char), 1, fp); // reads a byte "unsigned char" pixel into current_pixel.g field
			fread(&current_pixel.b, sizeof(unsigned char), 1, fp); // reads a byte "unsigned char" pixel into current_pixel.b field
			*temp_ptr = current_pixel; // effectively stores current pixel in temporary buffer
			temp_ptr++; // increments temp_ptr to point to next image_data struct in global buffer
			
			// error checking block for each individual pixel to make sure they're not outside the color range limit
			if(current_pixel.r < 0 || current_pixel.r  > 255)
			{
				fprintf(stderr, "Error: Invalid color value in given file (RGB value not between 0-%d).\n", atoi(header_buffer->file_maxcolor));
				exit(1); // exits out of program due to error				
			}
			
			if(current_pixel.g < 0 || current_pixel.g > 255)
			{
				fprintf(stderr, "Error: Invalid color value in given file (RGB value not between 0-%d).\n", atoi(header_buffer->file_maxcolor));
				exit(1); // exits out of program due to error				
			}
			
			if(current_pixel.b < 0 || current_pixel.b > 255)
			{
				fprintf(stderr, "Error: Invalid color value in given file (RGB value not between 0-%d).\n", atoi(header_buffer->file_maxcolor));
				exit(1); // exits out of program due to error				
			}
			
			current_pixel.r = '0';
			current_pixel.g = '0'; // resets current pixel RGB values to 0
			current_pixel.b = '0';
			i++; // increments iterator variable
		}
		
		fclose(fp);
	}
	
	// file format was neither P3 nor P6 so exits with error
	else
	{
		fprintf(stderr, "Error: File format to read in not recognized\n");
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

// helper function 
void print_pixels()
{
	int i = 0;
	printf("\nHeader info:\nformat:%s\nheight:%s\nwidth:%s\nmaxcolor:%s\n\n", header_buffer->file_format, header_buffer->file_height, header_buffer->file_width, header_buffer->file_maxcolor);
	while(i != (atoi(header_buffer->file_width) * atoi(header_buffer->file_height)))
	{
		//if(i == 20)
			//break;
		printf("Pixel #%d\n", i);
		printf("R: %d G: %d B: %d\n", image_buffer->r, image_buffer->g, image_buffer->b);
		image_buffer++;
		i++;
	}
}