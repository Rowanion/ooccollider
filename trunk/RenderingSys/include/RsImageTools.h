/**
 * @file	RsImageTools.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 14.05.2010
 * @brief 	RsImageTools class declaration.
 */

#ifndef RSIMAGETOOLS_H_
#define RSIMAGETOOLS_H_

#include <boost/filesystem.hpp>

#include "RsStructs.h"

/**
 * @class RsImageTools
 * @brief A collection of tools for reading image formats. At the time of writing,
 * only TGA-Loading is supported.
 */
class RsImageTools
{
public:
	static RsImageTools* getSingleton();
	bool loadTGA(boost::filesystem::path* _path, RsTGAimage* _image);
	bool loadTGA(const char* filename, RsTGAimage* _image);
	void swapRBColorChannel(unsigned _width, unsigned _height, unsigned _format, char* _data);
	bool loadCompressedTrueColorTGA(const char * filename, RsTGAimage* _image);
	bool loadUncompressedTrueColorTGA(const char * filename, RsTGAimage* _image);
	bool loadUncompressed8BitTGA(const char * filename, RsTGAimage* _image);
private:
    static RsImageTools* instance;
	RsImageTools();
	virtual ~RsImageTools();
    class Guard {
    public:
		~Guard();
    };
    friend class Guard;
};

#endif /* RSIMAGETOOLS_H_ */
