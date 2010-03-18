/*
 * RenderingSys.cpp
 *
 *  Created on: 13.03.2010
 *      Author: TheAvatar
 */

#include <iostream>
#include <exception>

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "RsRendererImpl.h"
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
	RsRendererImpl* renderer = new RsRendererImpl();

	rsw->attachRenderer((RsAbstractRenderer*) renderer);
	RsWindow::animator(10);
	rsw->start();
	delete renderer;
	return 1;
}

