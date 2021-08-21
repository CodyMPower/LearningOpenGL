#include "GLWindow.h"

GLWindow::GLWindow() {
	width = 800;
	height = 600;
	initVariables();
}

GLWindow::GLWindow(GLint windowWidth, GLint windowHeight) {
	width = windowWidth;
	height = windowHeight;
	initVariables();
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

	createCallbacks();	// Creates key callbacks for the window
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// Disables the cursor and locks the cursor to the window

	glewExperimental = GL_TRUE;	// Enables experimental features

	if (glewInit() != GLEW_OK) {	// Check if glew initialised correctly
		printf("Failed to Initalise GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST);	// Enables Depth testing for proper ordering of primatives (triangle should be behind instead of infront ect)

	glViewport(0, 0, bufferWidth, bufferHeight);	// Sets the view port inside the window (not including the window boarder)

	glfwSetWindowUserPointer(mainWindow, this);	// What window the actions are occuring on, "this" reffers to the structure that owns the window

	return 0;

}

void GLWindow::handleKeys(GLFWwindow* window, int key, int code, int action, int mode) {
	GLWindow* theWindow = static_cast<GLWindow*>(glfwGetWindowUserPointer(window)); // gets the window user and points to it (the active window class)

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {	// If enter was pressed
		glfwSetWindowShouldClose(window, GL_TRUE);			// The window should close
	}

	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			theWindow->keys[key] = true;
		}

		else if (action == GLFW_RELEASE) {
			theWindow->keys[key] = false;
		}
	}

}

void GLWindow::createCallbacks() {
	glfwSetKeyCallback(mainWindow, handleKeys);			// When key callback occurs on mainWindow, send callback to handleKeys
	glfwSetCursorPosCallback(mainWindow, handleMouse);	// When mouse move callback occurs, send callback to handleMouse
}

void GLWindow::handleMouse(GLFWwindow* window, double xPos, double yPos) {
	GLWindow* theWindow = static_cast<GLWindow*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved) {	// Check if the mouse has been moved before
		theWindow->lastX = xPos;		// If not, set previous values to the current values
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;	//Values fliped so the controls are not inverted
	
	theWindow->lastX = xPos;	//Sets the current positions as the previous positions for next callback
	theWindow->lastY = yPos;

}

GLfloat GLWindow::getXChange() {
	GLfloat change = xChange;
	xChange = 0;
	return change;
}

GLfloat GLWindow::getYChange() {
	GLfloat change = yChange;
	xChange = 0;
	return change;
}

GLWindow::~GLWindow() {
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}