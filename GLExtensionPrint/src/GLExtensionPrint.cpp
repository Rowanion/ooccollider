/**
 * @file	GLExtensionPrint.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 */


#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
//
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <cstring>


using namespace std;

bool
space(char c)
{
	return isspace(c);
}

bool
no_space(char c)
{
	return !isspace(c);
}

vector<string>
split(const string& str)
{
	typedef string::const_iterator iter;
	vector<string> result;
	iter i = str.begin();
	while (i!=str.end()){
		i = find_if(i, str.end(), no_space);
		iter j = find_if(i, str.end(), space);
		if (i != str.end())
			result.push_back(string(i,j));
		i = j;
	}
	return result;
}

void
printVertexArrayLimits()
{
	int maxVertices, maxIndices;
	glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &maxVertices);
	glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &maxIndices);
	cout << endl << endl << "VBO-Properties:" << endl;
	cout << "=============================================" << endl;
	cout << "Maximum # of vertices per array: " << maxVertices << endl;
	cout << "Maximum # of indices per array: " << maxIndices << endl;

}

/**
* @brief Fetches the OpenGL-Side capabilities related to VBOs
*
* Fetches VBO-Limits dictated by OpnGL and prints them to StdOut.
*/
void
printFBOLimits()
{
	//TODO
	int maxTexSize, maxColAttach, maxRBSize, fbb, rbb;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &maxColAttach);
	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE_EXT, &maxRBSize);
	glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &fbb);
	glGetIntegerv(GL_RENDERBUFFER_BINDING_EXT, &rbb);

	cout << endl << endl << "FBO-Properties:" << endl;
	cout << "=============================================" << endl;
	cout << "Maximum texture size: " << maxTexSize << endl;
	cout << "Maximum # of color attachments: " << maxColAttach << endl;
	cout << "Maximum Renderbuffer size: " << maxRBSize << endl;
	cout << "Framebuffer binding: " << fbb << endl;
	cout << "Renderbuffer binding: " << rbb << endl;
}

void
printGeneralSystemInfo()
{
	char* vendor = (char*)glGetString(GL_VENDOR);
	char* renderer = (char*)glGetString(GL_RENDERER);
	char* version = (char*)glGetString(GL_VERSION);

	cout << endl << endl << "General System Info:" << endl;
	cout << "=============================================" << endl;
	cout << "GL_VENDOR of GraKa: " << vendor << endl;
	cout << "GL_RENDERER: " << renderer << endl;
	cout << "GL_VERSION (supported): " << version << endl;
}

bool
isSupported(string extension, string extensions)
{
	GLboolean check = gluCheckExtension((const GLubyte*)extension.c_str(), (const GLubyte*)extensions.c_str());
	if (check==GL_TRUE) return true;
	else return false;
}

int
main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	//glewInit();
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	glutInitWindowSize(512,512);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutCreateWindow("ModelConverter");

	cout << "Printing List of supported OpenGL-Extensions:" << endl;
	cout << "=============================================" << endl;

	char* text=(char*)malloc(strlen((char *)glGetString(GL_EXTENSIONS))+1);		// Allocate Memory For Our Extension String
	strcpy (text,(char *)glGetString(GL_EXTENSIONS));
	vector<string> extList = split(string(text));
	vector<string>::const_iterator it;
	for (it = extList.begin(); it!=extList.end(); ++it){
		cout << *it << endl;
	}

	printGeneralSystemInfo();
	printFBOLimits();
	printVertexArrayLimits();

	return 0;
}
