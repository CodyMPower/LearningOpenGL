#include "GLWindow.h"

GLWindow::GLWindow() {
	mainWindow = 0;
	width = 800;
	height = 600;
	bufferWidth = 0;
	bufferHeight = 0;
}

GLWindow::GLWindow(GLint windowWidth, GLint windowHeight) {
	mainWindow = 0;
	width = windowWidth;
	height = windowHeight;
	bufferWidth = 0;
	bufferHeight = 0;
}

int GLWindow::Initialise() {
	// Initialise GLFW
	if (!glfwInit()) {	// Check if glfw initialised correctly
		printf("Failed to Initialise GLFW");
		glfwTerminate();
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// Major version 3 of OpenGL
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);	// Minor version 3 of OpenGL
													// Final version 3.3 of OpenGL

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// Disallows backwards compatability
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);			// Allows forwards compatability

	mainWindow = glfwCreateWindow(width, height, "Test Window", NULL, NULL);
	// Creates a new window
	// WIDTH:	width of the new window
	// HEIGHT:	height of the new window
	// "Test Window":	Name of the window
	// NULL:	What monitor to display to
	// NULL:	?

	if (!mainWindow) {	// Check if the window was created correctly
		printf("Window Creation Failed");
		glfwTerminate();
		return 1;
	}

	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);	// Gets the dimensions of the buffer
	glfwMakeContextCurrent(mainWindow);									// Set the main window to the current context (window being used)

	glewExperimental = GL_TRUE;	// Enables experimental features

	if (glewInit() != GLEW_OK) {	// Check if glew initialised correctly
		printf("Failed to Initalise GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST);	// Enables Depth testing for proper ordering of primatives (triangle should be behind instead of infront ect)

	glViewport(0, 0, bufferWidth, bufferHeight);	// Sets the view port inside the window (not including the window boarder)

	return 0;

}

GLWindow::~GLWindow() {
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}