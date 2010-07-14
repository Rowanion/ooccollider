/**
 * @file	RenderingSys.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 28.09.2009
 * @brief 	Holds the main-function for an exemplary RenderingSys implementation.
 */


#include <iostream>
#include <exception>

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "RsDissolveRenderer.h"
#include "RsPOMRenderer.h"
#include "RsWindow.h"
#include "RsImageTools.h"
#include "RsStructs.h"

#include "RsStructs.h"

namespace fs = boost::filesystem;
using namespace std;

int main ( int argc, char** argv )   // Create Main Function For Bringing It All Together
{
	RsWindow* rsw = RsWindow::getSingleton();
	rsw->setWindowTitle("zuffi");
	rsw->setWindowDimension(500,500);
//	RsPOMRenderer* renderer = new RsPOMRenderer();
	RsDissolveRenderer* renderer = new RsDissolveRenderer();

	rsw->attachRenderer((RsAbstractRenderer*) renderer);
	RsWindow::animator(10);
	rsw->start();
	delete renderer;
	return 1;
}

