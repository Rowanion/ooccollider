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

int main(int argc, char *argv[]) {
	oocframework::MpiControl* mpic = oocframework::MpiControl::getSingleton();

	mpic->init(argc, argv);

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
		DataCore dc = DataCore(800, 600, WINDOW_WIDTH, WINDOW_HEIGHT);
		cout << "End of display loop " << mpic->getRank() << endl;
	}
	cout << "leaving depending part: " << mpic->getRank() << endl;
	mpic->finalize();
	return 0;
}

