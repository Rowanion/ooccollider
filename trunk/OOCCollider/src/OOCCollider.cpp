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

	int rank = mpic->getRank();

	cout << "entering depending part" << endl;
	if (rank == 0) {
		cout << "MY RANK IS " << mpic->getRank() << endl;
		RenderMasterCore rmc = RenderMasterCore(TARGET_WIDTH, TARGET_HEIGHT);
//		setupWindow("master");
		cout << "End of display loop " << mpic->getRank()  << endl;
		//		setupWindow("My rank is 0");
	}
	else if (mpic->getGroup() == oocframework::MpiControl::RENDERER){ // RENDER-GROUP
		//		setupWindow("slave");
		RenderCore rc = RenderCore(WINDOW_WIDTH, WINDOW_HEIGHT, TARGET_WIDTH, TARGET_HEIGHT);
		cout << "End of display loop " << mpic->getRank() << endl;
	}
	else if (mpic->getGroup() == oocframework::MpiControl::DATA){ // DATA-GROUP
		//		setupWindow("slave");
		DataCore dc = DataCore(WINDOW_WIDTH, WINDOW_HEIGHT, TARGET_WIDTH, TARGET_HEIGHT);
		cout << "End of display loop " << mpic->getRank() << endl;
	}

	cerr << "FINALIZING node: " << rank << endl;
	mpic->finalize();
	cerr << "DONE with " << rank << endl;
	return 0;
}

