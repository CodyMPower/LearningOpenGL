#pragma once

#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class GLWindow
{
public:
	GLWindow();
	GLWindow(GLint windowWidth, GLint windowHeight);

	int Initialise();	// Initialises GLFW, GLEW, and the main window

	GLint getBufferWidth() { return bufferWidth; }		// Gets the width of the view buffer
	GLint getBufferHeight() { return bufferHeight; }	// Gets the height of the view buffer

	GLfloat getXChange();
	GLfloat getYChange();

	bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }	// Gets if the window inside this class should close
	bool* getKeys() { return keys; };									// Retuns a pointer to keys, not very OOP

	void swapBuffers() { glfwSwapBuffers(mainWindow);  }	// Swaps the buffers of the window inside this class

	~GLWindow();

private:
	GLFWwindow* mainWindow;	// Main window handler of the class

	GLint width, height, bufferWidth, bufferHeight;	// Window variables
	GLfloat lastX, lastY, xChange, yChange;			// Mouse variables

	bool keys[1024];		// Covers the range of ASCII characters for keyboard input
	bool mouseFirstMoved;	// Check for if the mouse has moved yet

	/* Callback function
	* key:		the key that has been pressed
	* action:	the action that was prefirmed on the key
	*/
	static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
	void createCallbacks();
	static void handleMouse(GLFWwindow* window, double xPos, double yPos);

	void initVariables() {	//Initializes the variables to 0 other than width and height
		mainWindow = 0;
		bufferWidth = 0;
		bufferHeight = 0;

		for (size_t i = 0; i < 1024; i++) {
			keys[i] = 0;
		}

		lastX = 0;
		lastY = 0;
		xChange = 0;
		yChange = 0;
	}
};

