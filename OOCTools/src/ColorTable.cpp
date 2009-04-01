/*
 * ColorTable.cpp
 *
 *  Created on: 23.03.2009
 *      Author: ava
 */

#include "ColorTable.h"

#include <iostream>
#include <cmath>

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "GlToolkit.h"
#include "V3ub.h"
#include "StructDefs.h"

using namespace std;
namespace fs = boost::filesystem;

namespace ooctools {

#define GET_GLERROR(ret) \
{ \
         GLenum err = glGetError(); \
         if (err != GL_NO_ERROR) { \
                 fprintf(stderr, "[%s line %d] GL Error: %s\n", \
                 __FILE__, __LINE__, gluErrorString(err)); \
                 fflush(stderr); \
         } \
}

ColorTable::ColorTable() :
	nColorsi(0), nColorsf(0.0f), quotient(0.0f), halfQuotient(0.0f), texId(0),
			cgTex(0), dummySpace(colors.begin())
{
	addColori(new V3ub(*defaultColorB));
}

ColorTable::ColorTable(const char* data) :
	nColorsi(0), texId(0), cgTex(0), dummySpace(colors.begin()), dummyIndex(0)
{
	addColori(new V3ub(*defaultColorB));

	// what we expect: data's 1st 4 bytes are an uint specifying the number of 3-tupel
	// ubytes we'll find in data. schakkalakka

	// copy pointer
	unsigned char* localData = (unsigned char*)data;

	//fetch color count
	unsigned int localNColors = ((unsigned int*)localData)[0];
	//advance pointer beyond color-count
	localData+=sizeof(unsigned int);
	addByteStream(localData, localNColors);
}

ColorTable::ColorTable(fs::path _filePath) :
	nColorsi(0), texId(0), cgTex(0), dummySpace(colors.begin()), dummyIndex(0)
{
	addColori(new V3ub(*defaultColorB));
	addFile(_filePath);
}

ColorTable::ColorTable(string _filePath) :
	nColorsi(0), texId(0), cgTex(0), dummySpace(colors.begin()), dummyIndex(0)
{
	addColori(new V3ub(*defaultColorB));
	addFile(fs::path(_filePath));
}
ColorTable::~ColorTable()
{
	clear();
}

void
ColorTable::bindTex()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, texId);
	GET_GLERROR(0);
	glEnable(GL_TEXTURE_1D);
	cgGLEnableTextureParameter(cgTex);
}

void
ColorTable::unbindTex()
{
	cgGLDisableTextureParameter(cgTex);
	glBindTexture(GL_TEXTURE_1D, 0);
	GET_GLERROR(0);
	glDisable(GL_TEXTURE_1D);
}

void
ColorTable::setCgParams(CGprogram shader)
{
		cgTex = cgGetNamedParameter(shader, "colorLut");
		cgGLSetTextureParameter(cgTex, texId);
}

void
ColorTable::setupTexture()
{
//	inflateToPowerOfTwo();
	unsigned char pixelData[nColorsi*3];
	toByteStream(pixelData);

	if (texId == 0)
		glGenTextures(1, &texId);
	GET_GLERROR(0);

	glBindTexture(GL_TEXTURE_1D, texId);
	GET_GLERROR(0);

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S,
		GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_1D,
		GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D,
		GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	GET_GLERROR(0);

	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB16, nColorsi,
		0, GL_RGB, GL_UNSIGNED_BYTE, pixelData);
	GET_GLERROR(0);
}

unsigned int
ColorTable::addColori(ooctools::V3ub* _color){
	unsigned int thisIdx = nColorsi;
	map<string, unsigned int>::const_iterator it = dictionary.find(_color->toString());
	if (it==dictionary.end()){
		dictionary.insert(make_pair(_color->toString(), thisIdx));
		colors.push_back(_color);
		nColorsi = colors.size();
		nColorsf = static_cast<float>(nColorsi);
		quotient = 1.0f/nColorsf;
		halfQuotient = quotient/2.0f;
		return thisIdx;
	}
	else
		return it->second;
}

unsigned int
ColorTable::addColori(const Material& _mat)
{
	return addColori(new V3ub(_mat.kdR, _mat.kdG, _mat.kdB));
}

float
ColorTable::addColorf(ooctools::V3ub* _color){
	return static_cast<float>(addColori(_color));
}


/*
 * +-------+-------+-------+-------+-------+
 * |  red  |violet | blue  | gray  | green |	<- 1D-Texture
 * +-------+-------+-------+-------+-------+
 * 0      0.2     0.4     0.6     0.8      1
 *     ^       ^       ^       ^       ^
 *    0.1     0.3     0.5     0.7     0.9
 *
 * 5 colors. index-rage from 0 to 4.
 * tex-coords range from 0.0 to 1.0
 * so index/# colors should work as tex-idx - does it?!
 * It doesn't! Index * 1/#colors puts us directly
 * on the border between 2 colors. The quotient is the size of a color.
 * So if we add half the quotient to the final index we'll end up
 * directly at the center of our color!
 *
 */

float
ColorTable::calculateTexCoord(unsigned int index)
{
	return ((static_cast<float>(index))*quotient)+halfQuotient;
}

unsigned int
ColorTable::calculateIndex(float texCoord)
{
	return static_cast<unsigned int>((texCoord-halfQuotient)*nColorsf);
}
const V3ub&
ColorTable::getColor(unsigned int idx) const
{
	return *colors.at(idx);
}

void
ColorTable::writeToFile(fs::path _filePath)
{
//	inflateToPowerOfTwo();
	unsigned char* memblock;
	fs::ofstream outFile;
	outFile.open( _filePath,ios::binary|ios::out);
	// reserve space for one int  we'll read - the number of colors in file
	memblock = new unsigned char[sizeof(unsigned char)*3*nColorsi];
	toByteStream(memblock);
	// goto the beginning of the file
	outFile.seekp(0, ios::beg);
	// write in one int - number of colors
	outFile.write((char*) &nColorsi, sizeof(unsigned int));
	// jump to right after the # of colors
	outFile.seekp(sizeof(unsigned int), ios::beg);
	// write all colors
	outFile.write((char*)memblock, 3*nColorsi*sizeof(unsigned char));
	outFile.close();
}

void
ColorTable::addByteStream(const unsigned char* _dataStream, unsigned int _nColors)
{
	const unsigned char* localData = _dataStream;

	for(unsigned int i = 0; i<_nColors*3; i+=3){
		int idx = addColori(new V3ub(localData));
		localData+=3;
		cout << "color: " << colors.at(idx)->toString() << endl;
	}
	cout << "quotient: " << getQuotient() << endl;
}

void
ColorTable::toByteStream(unsigned char* _ref)
{
	V3ub* tempCol = 0;
	for (unsigned int i=0; i<nColorsi*3; i+=3){
		tempCol = colors.at(i/3);
		_ref[i] = tempCol->getX();
		_ref[i+1] = tempCol->getY();
		_ref[i+2] = tempCol->getZ();
	}
}

void
ColorTable::addFile(fs::path _filePath)
{
	char* memblock;
	fs::ifstream inFile;
	inFile.open( _filePath,ios::binary|ios::in);
	// reserve space for one int  we'll read - the number of colors in file
	memblock = new char[sizeof(unsigned int)];
	// goto the beginning of the file
	inFile.seekg(0, ios::beg);
	// read in one int
	inFile.read((char*)memblock, sizeof(unsigned int));
	unsigned int localNColors = *(unsigned int*)memblock;
	// free up reseverd space
	delete memblock;
	// jump ahead one int - to skip future reading of the # colors
	inFile.seekg(sizeof(unsigned int), ios::beg);
	// read in all colors
	memblock = new char[3*localNColors*sizeof(unsigned char)];
	inFile.read((char*)memblock, 3*localNColors*sizeof(unsigned char));
	inFile.close();
	addByteStream((unsigned char*)memblock, localNColors);
//	inflateToPowerOfTwo();
}

ColorTable&
ColorTable::operator=(const ColorTable& _ct)
{
	if (*this == _ct)
		return *this;
	else {
		clear();
		const vector<V3ub*> tempList (_ct.getColorVector());
		vector<V3ub*>::const_iterator it;
		for (it=tempList.begin(); it!=tempList.end(); ++it){
			addColori(new V3ub(*(*it)));
		}

	}
	return *this;
}

bool
ColorTable::operator==(const ColorTable& _ct)
{
	const vector<V3ub*> tempList (_ct.getColorVector());
	if (tempList.size() != colors.size()) return false;
	for(unsigned int i=0; i<nColorsi; ++i){
		if (*tempList.at(i) == *colors.at(i))
			return false;
	}
	return true;
}

void ColorTable::clear()
{
	nColorsi = 0;
	texId = 0;
	if (!colors.empty()){
		vector<V3ub*>::iterator it;
		for(it = colors.begin(); it != colors.end(); ++it){
			delete (*it);
			(*it) = 0;
		}
		colors.clear();
	}
	dictionary.clear();
}

void
ColorTable::debug() const
{
	V3ub* temp = *(colors.end()-2);
	cout << "farbe: " << temp->toString() << endl;
	temp = *(colors.end()-1);
	cout << "farbe: " << temp->toString() << endl;
	cout << "anzahl farben: " << nColorsi << endl;
//	for (unsigned int i=0; i<10; ++i){
//		cout << colors.at(i)->toString()<< endl;
//	}
}
void
ColorTable::inflateToPowerOfTwo()
{
	float log = log2(nColorsf);
	float logCeil = ceil(log);
	if ( log < logCeil && nColorsi != 0 && dummyIndex == 0){
		float nextPowerOfTwo = pow(2.0, logCeil);
		addNDummyColors(static_cast<unsigned int>(nextPowerOfTwo-nColorsf));
	}
	nColorsi = colors.size();
	nColorsf = static_cast<float>(nColorsi);
	quotient = 1.0f/nColorsf;
	halfQuotient = quotient/2.0f;
}

void ColorTable::addNDummyColors(unsigned int n)
{
	for (unsigned int i = 0; i < n; ++i) {
		colors.push_back(new V3ub((unsigned char)0));
		if (i == 0) {
			dummySpace = colors.end() - 1;
			dummyIndex = nColorsi;
		}
	}
}

void
ColorTable::removePowerOfTwoOverhead()
{
	if (nColorsi != 0 && dummySpace != colors.begin()){
		vector<V3ub*>::iterator it = dummySpace;
		for (; it!= colors.end(); ++it){
			delete *it;
		}
		colors.erase(dummySpace, colors.end());
	}
	nColorsi = colors.size();
	nColorsf = static_cast<float>(nColorsi);
	quotient = 1.0f/nColorsf;
	halfQuotient = quotient/2.0f;
	dummySpace = colors.begin();
	dummyIndex = 0;
}

bool ColorTable::isInflated(){
	if (dummyIndex != 0 && dummySpace != colors.begin())
		return true;
	else return false;
}

} // ooctools
