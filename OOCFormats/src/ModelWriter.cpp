/*
 * ModelWriter.cpp
 *
 *  Created on: 15.02.2009
 *      Author: ava
 */

#include "ModelWriter.h"

#include <string>
#include <iostream>

#include "BoundingBox.h"
#include "MetaGroup.h"
#include "StructDefs.h"

using namespace std;
using namespace ooctools;

namespace oocformats {

struct VHeader
{
	int fSize;
	float min[3];
	float max[3];
	int vSize;
	int cSize;
	int color[3];

};

ModelWriter::ModelWriter()
{
	// TODO Auto-generated constructor stub
}

ModelWriter::~ModelWriter()
{
	// TODO Auto-generated destructor stub
}

void
ModelWriter::debug()
{
	cout << "Additon: " << 6 * sizeof(int) + 6 * sizeof(float) << endl;
	cout << "Struct: " << sizeof(VHeader) << endl;
	float f = -0.682744f;
	f *= 128.0f;
	cout << "as float:" << f << endl;
	cout << "in char:" << (int) f << endl;
}
void
ModelWriter::writeMetaGrp(MetaGroup* _grp)
{
	FILE *rawFile = fopen(_grp->name.c_str(), "wb+");
	//void *header = buildHeader(obj);

	//fwrite(buf, sizeof(GLint), len, outout);


	//GLint f[] = {-111767};
	//writeBufferToDisk(f, 1);
	fclose(rawFile);
}

void*
ModelWriter::buildHeader(MetaGroup* _grp)
{
	// #Faces, V3 min, V3 max, #Vertices, #Components, V3 color
	//void *buf = new void[10];
	VHeader vh;
	vh.fSize = _grp->nFaces;
	vh.cSize = 3;
	vh.vSize = _grp->nVertices;
	vh.min[0] = _grp->bb->getMin().getX();
	vh.min[1] = _grp->bb->getMin().getY();
	vh.min[2] = _grp->bb->getMin().getZ();
	vh.max[0] = _grp->bb->getMax().getX();
	vh.max[1] = _grp->bb->getMax().getY();
	vh.max[2] = _grp->bb->getMax().getZ();
	vh.color[0] = 255;
	vh.color[1] = 0;
	vh.color[2] = 0;

	void* k;
	return k;
}
/*
 void writeBufferToDisk(void *buf, int len){
 //ofstream bFile("test.buf");
 FILE *outout = fopen( "test.buf", "wb+" );
 fwrite(buf, sizeof(GLint), len, outout);
 fread(buf, sizeof(GLint), len, outout);

 cout << "# written: " << ((GLint*)buf)[0] << endl;
 //fflush(outout);
 fclose(outout);
 }
 */

} // oocformats
