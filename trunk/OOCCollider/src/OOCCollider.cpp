/**
 * @file	OOCCollider.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	Main for the OOCCollider.
 */

#include "mpi.h"
#include "MpiControl.h"
#include <cmath>
#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>

#include "declarations.h"
#include "OOCWindow.h"
#include "AbstractEventHandler.h"
#include "TestEventHandler.h"
#include "RenderMasterCore.h"
#include "RenderCore.h"
#include "DataCore.h"

using namespace std;

void init()
{
	int width, height; // Window dimensions
	// Get window size
	glfwGetWindowSize(&width, &height);

	// Make sure that height is non-zero to avoid division by zero
	height = height < 1 ? 1 : height;

	// Set viewport
	glViewport(0, 0, width, height);
	// Clear color and depht buffers
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set up projection matrix
	glMatrixMode(GL_PROJECTION); // Select projection matrix
	glLoadIdentity(); // Start with an identity matrix
	gluPerspective( // Set perspective view
			65.0, // Field of view = 65 degrees
			(double) width / (double) height, // Window aspect (assumes square pixels)
			1.0, // Near Z clipping plane
			100.0 // Far Z clippling plane
	);

	// Set up modelview matrix
	glMatrixMode(GL_MODELVIEW); // Select modelview matrix
	glLoadIdentity(); // Start with an identity matrix
	gluLookAt( // Set camera position and orientation
			0.0, 0.0, 10.0, // Camera position (x,y,z)
			0.0, 0.0, 0.0, // View point (x,y,z)
			0.0, 1.0, 0.0 // Up-vector (x,y,z)
	);
}

void Draw(void) {

		// Clear color and depht buffers
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Set up modelview matrix
		glMatrixMode(GL_MODELVIEW); // Select modelview matrix
		glLoadIdentity(); // Start with an identity matrix
		gluLookAt( // Set camera position and orientation
				0.0, 0.0, 10.0, // Camera position (x,y,z)
				0.0, 0.0, 0.0, // View point (x,y,z)
				0.0, 1.0, 0.0 // Up-vector (x,y,z)
		);





	double t; // Time (in seconds)
	// Get current time
	t = glfwGetTime();

	glScalef(2.0f,2.0f,2.0f);
	glRotatef( 60.0f * (float)t, 0.0f, 1.0f, 0.0f );
	// Here is where actual OpenGL rendering calls would begin...
	glBegin(GL_TRIANGLES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.5f, 1.0f, 0.0f);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(1.0f, 0.0f, 0.0f);
		glEnd();
	glColor3f(1.0f, 0.0f, 0.0f);

}


void setupWindow(const char* title)
{
	int ok; // Flag telling if the window was opened
	int running; // Flag telling if the program is running

	glfwInit();

	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 16);

	// Open window
	ok = glfwOpenWindow(WINDOW_WIDTH, WINDOW_HEIGHT, // Width and height of window
			8, 8, 8, // Number of red, green, and blue bits for color buffer
			8, // Number of bits for alpha buffer
			24, // Number of bits for depth buffer (Z-buffer)
			0, // Number of bits for stencil buffer
			GLFW_WINDOW // We want a desktop window (could be GLFW_FULLSCREEN)
			);
	glfwSetWindowTitle(title);

	// Enable sticky keys
	glfwEnable(GLFW_STICKY_KEYS);

	init();
	// Main rendering loop
	do {
		// Call our rendering function
		Draw();

		// Swap front and back buffers (we use a double buffered display)
		glfwSwapBuffers();

		// Check if the escape key was pressed, or if the window was closed
		running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(
				GLFW_OPENED);
	} while (running);

	// Terminate GLFW
	glfwTerminate();

}

int main(int argc, char *argv[]) {
	MpiControl* mpic = MpiControl::getSingleton();
	mpic->init();

	cout << "entering depending part" << endl;
	if (mpic->getRank() == 0) {
		cout << "MY RANK IS " << mpic->getRank() << endl;
		RenderMasterCore rmc = RenderMasterCore(WINDOW_WIDTH, WINDOW_HEIGHT);
//		setupWindow("master");
		cout << "End of display loop " << mpic->getRank()  << endl;
//		setupWindow("My rank is 0");
	}
	else if ((mpic->getRank() >= 1) && (mpic->getRank() <= 2)) {
//		setupWindow("slave");
		RenderCore rc = RenderCore(800, 600, WINDOW_WIDTH, WINDOW_HEIGHT);
		cout << "End of display loop " << mpic->getRank() << endl;
	}
	else {
//		setupWindow("slave");
		DataCore dc = DataCore(800, 600);
		cout << "End of display loop " << mpic->getRank() << endl;
	}
	cout << "leaving depending part: " << mpic->getRank() << endl;
	mpic->finalize();
	return 0;
}

