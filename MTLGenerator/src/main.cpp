/**
 * @file	main.cpp
 * @author	TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date 	Created on: 04.04.2009
 *
 */

#include "MTLGenerator.h"
#include <boost/filesystem.hpp>
#include "BoundingBox.h"


int main(int argc, char *argv[]) {

	// last entry: DPA-E114W4450S01-AD-2.mtl in dir
	// last console-msg: media/EXTERNAL/B3x7/W289W4103S00-AG-7002V1.mtl gübtsnocnnüch-writing
	/* Exception: matname: S280W605-3(4)>S280W605-3(1)
	parsing....
	terminate called after throwing an instance of 'std::length_error'
	  what():  basic_string::_S_create

	*/
	fs::path objDir("/media/External/B3x7/Part1");
	fs::path mtlDir("/media/External/B3x7/Part1");
	fs::path xlsDir("/media/External/B3x7/Farben");
	MTLGenerator::MTLGenerator *mtlg = new MTLGenerator::MTLGenerator(objDir, xlsDir, mtlDir);
//	mtlg->doTheParsing();
//	mtlg->matchObjsVsMtls();
	mtlg->setDirs(fs::path("/media/External/B3x7/Part2"), xlsDir, fs::path("/media/External/B3x7/Part2"));
//	mtlg->doTheParsing();
//	mtlg->matchObjsVsMtls();
//	mtlg->matchMtlsVsObjs();
//	mtlg->debug();
//	mtlg->flipOAndG();

	MTLGenerator::SceneHeader sh;
	sh.triCount = 0;
	sh.vertexCount = 0;
//	mtlg->parseSceneBoundingBox(sh, fs::path("/media/External/B3_ausschnitt_klein"), fs::path("/media/ClemensHDD"));
	mtlg->parseSceneBoundingBox(sh, fs::path("/media/ClemensHDD/B3_binary"), fs::path("/media/ClemensHDD"));


	delete mtlg;
	return 1;
}
