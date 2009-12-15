/**
 * @file	main.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 15.12.2009
 * @brief 	main class declaration.
 */

#include "VboFactory.h"
#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>


namespace fs=boost::filesystem;
using namespace ooctools;
using namespace std;

int main(int argc, char* argv[]){
	ooctools::VboFactory* vf = VboFactory::getSingleton();
	fs::ifstream inFile;
	inFile.open("/home/ava/Diplom/Model/SampleTree_packed/Data0.bin", ios::binary);
	IVbo* iVbo = vf->newVbo(&inFile, 0);
	return 0;
}
