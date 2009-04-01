/*
 * FileHeader.h
 *
 *  Created on: 23.03.2009
 *      Author: ava
 */

#ifndef COLORTABLE_H_
#define COLORTABLE_H_

#include <vector>
#include <map>

#include "GlToolkit.h"

#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "V3ub.h"
#include "declarations.h"

namespace fs = boost::filesystem;
namespace ooctools {

class ColorTable
{
	public:
		ColorTable();
		ColorTable(const char* data);
		ColorTable(fs::path _filePath);
		ColorTable(std::string _filePath);
		virtual ~ColorTable();
//		const float* getTextureData();
		void setupTexture();
		const GLuint getTextureId();
		void setCgParams(const CGprogram shader);
		unsigned int addColori(ooctools::V3ub* _color);
		unsigned int addColori(const Material& _mat);
		float addColorf(ooctools::V3ub* _color);
		void bindTex();
		void unbindTex();
		unsigned int getNColorsi(){return nColorsi;};
		float getNColorsf(){return nColorsf;};
		float getQuotient(){return quotient;};
		float getHalfQuotient(){return halfQuotient;};
		float calculateTexCoord(unsigned int index);
		unsigned int calculateIndex(float texCoord);
		const ooctools::V3ub& getColor(unsigned int idx) const;
		void writeToFile(fs::path _filePath);
		static ColorTable* readFromFile(fs::path _filePath);
		void addByteStream(const unsigned char* _dataStream, unsigned int _nColors);
		void addFile(fs::path _filePath);
		const std::vector<ooctools::V3ub*>& getColorVector() const {return colors;};

		ColorTable& operator=(const ColorTable& _ct);
		bool operator==(const ColorTable& _ct);
		void clear();
		void debug() const;
		bool isInflated();

	private:
		// keeps track of colors.size() to quench the need of recalculation
		unsigned int nColorsi;
		// same as above, only in float
		float nColorsf;
		// this always 1.0 / nColors - for quicker access
		float quotient;
		// well....take a wild guess....
		float halfQuotient;
		// this is used to bind/unbind and upload the texture
		GLuint texId;
		// this holds the refernce to the shader which will actually access the texture.
		CGparameter cgTex;
		// center of this class - the data vault
		std::vector<ooctools::V3ub*> colors;

		std::vector<V3ub*>::iterator dummySpace;
		unsigned int dummyIndex;
		// ah, not elegant though - but it works.
		// this map holds a string-representation of a color and its index in the data vault.
		// i use this to "quickly" find out if the color is already in the vault,
		// and if that's the case, return its index
		std::map<std::string, unsigned int> dictionary;
		void toByteStream(unsigned char* _ref);
		void addNDummyColors(unsigned int n);
		void removePowerOfTwoOverhead();
		void inflateToPowerOfTwo();
};

} // ooctools

#endif /* COLORTABLE */
