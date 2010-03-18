/*
 * RsImageTools.cpp
 *
 *  Created on: 14.03.2010
 *      Author: TheAvatar
 */

#include "RsImageTools.h"

#include <iostream>
#include <cstring>

#include <boost/system/config.hpp>
#include <boost/filesystem/fstream.hpp>

#include "GL/glew.h"

using namespace std;
namespace fs = boost::filesystem;

RsImageTools* RsImageTools::instance = 0;

RsImageTools* RsImageTools::getSingleton()
{
	static Guard g;
	if( instance == 0 ){
		instance = new RsImageTools();
	}
	return instance;
}

RsImageTools::Guard::~Guard()
{
	if( RsImageTools::instance != 0 )
		delete RsImageTools::instance;
}

RsImageTools::RsImageTools()
{
}

RsImageTools::~RsImageTools()
{
	// TODO Auto-generated destructor stub
}

bool RsImageTools::loadTGA(fs::path* _path, RsTGAimage* _image)
{
	return loadTGA(_path->string().c_str(), _image);
}
bool RsImageTools::loadTGA(const char* filename, RsTGAimage* _image)
{
	unsigned char uncompressed8BitTGAHeader[12]= {0, 1, 1, 0, 0, 0, 1, 24, 0, 0, 0, 0},
			uncompressedTGAHeader[12]    = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			compressedTGAHeader[12]      = {0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			uncompressed8BitBwTGAHeader[12]= {0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			compressed8BitBwTGAHeader[12]= {0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	unsigned char TGAcompare[12];           //Used to compare TGA header

	FILE * file = fopen(filename, "rb");    //Open the TGA file

	if(file==NULL )                         //Does the file exist?
		return false;


	//read the header
	fread(TGAcompare, 1, sizeof(TGAcompare), file);
	fclose(file);

	if(!memcmp(uncompressedTGAHeader, TGAcompare, sizeof(uncompressedTGAHeader)))
	{
		cerr << "loading uncomp. TrueColor" << endl;
		return loadUncompressedTrueColorTGA(filename, _image);
	}
	else if(!memcmp(compressedTGAHeader, TGAcompare, sizeof(compressedTGAHeader)))
	{
		cerr << "loading comp. TrueColor" << endl;
		return loadCompressedTrueColorTGA(filename, _image);
	}
	else if(!memcmp(uncompressed8BitTGAHeader, TGAcompare, sizeof(uncompressed8BitTGAHeader)))
	{
		cerr << "loading uncomp. 8bit" << endl;
		return loadUncompressed8BitTGA(filename, _image);
	}
	else if(!memcmp(uncompressed8BitBwTGAHeader, TGAcompare, sizeof(uncompressed8BitBwTGAHeader)))
	{
		cerr << "uncompressed 8Bit B/W image found. Not implemented yet!" << endl;
		return false;
	}
	else if(!memcmp(compressed8BitBwTGAHeader, TGAcompare, sizeof(compressed8BitBwTGAHeader)))
	{
		cerr << "compressed 8Bit B/W image found. Not implemented yet!" << endl;
		return false;
	}
	else{
		cerr << "No Matching header found:" << endl;
		for (unsigned i=0; i<12; i++){
			cerr << (int)TGAcompare[i] << ", ";
		}
		cerr << endl;
		return false;
	}

	return false;
}

void RsImageTools::swapRBColorChannel(unsigned _width, unsigned _height, unsigned _format, char* _data)
{
	unsigned i;
	unsigned char temp;
	unsigned char bytesPerPixel = 3;

	if (_format == TGA_RGBA)
	{
		bytesPerPixel = 4;
	}

	// swap the R and B values to get RGB since the bitmap color format is in BGR
	for (i = 0; i < _width*_height*bytesPerPixel; i+=bytesPerPixel)
	{
		temp = _data[i];
		_data[i] = _data[i+2];
		_data[i+2] = temp;
	}
}

bool RsImageTools::loadUncompressed8BitTGA(const char * filename, RsTGAimage* _image)
{
  unsigned char   TGAHeader[12]={0, 1, 1, 0, 0, 0, 1, 24, 0, 0, 0, 0};
  unsigned char   TGAcompare[12];           //Used to compare TGA header
  unsigned char   header[6];              //First 6 useful bytes of the header

  FILE * file = fopen(filename, "rb");        //Open the TGA file

  if(file == NULL)                //Does the file exist?
    return false;

  if(fread(TGAcompare, 1, sizeof(TGAcompare), file)!=sizeof(TGAcompare)|| //Are there 12 bytes to read?
    memcmp(TGAHeader, TGAcompare, sizeof(TGAHeader))!=0 ||          //Is the header correct?
    fread(header, 1, sizeof(header), file)!=sizeof(header))   //Read next 6 bytes
  {
    fclose(file);               //If anything else failed, close the file
    return false;
  }

  //save data into class member variables
  _image->width = header[1]*256+header[0];
  _image->height = header[3]*256+header[2];
  _image->components = header[4]/8;

  if(_image->width  <=0 || //if width <=0
		  _image->height <=0 || //or height<=0
     header[4] != 8)    //bpp not 8
  {
    fclose(file);                     //close the file
    return false;
  }

  _image->format = GL_RGB;
  _image->intlFormat = GL_RGB8;

  //make space for palette
  unsigned char * palette=new unsigned char[256*3];
  if(!palette)
    return false;


  //load the palette
  fread(palette, 256*3, 1, file);

  //allocate space for color indices
  unsigned char * indices=new unsigned char[_image->width*_image->height];
  if(!indices)
    return false;


  //load indices
  fread(indices, 1, _image->width*_image->height, file);

  //close the file
  fclose(file);

  if(_image->data){
	  delete[] _image->data;
	  _image->data = 0;
  }

  //allocate space for the image data
  _image->data = new char[_image->width*_image->height*3];

  if(!_image->data)
  {
    fclose(file);
    return false;
  }

  //calculate the color values
  for(int currentRow=0; currentRow<_image->height; currentRow++)
  {
    for(int i=0; i<_image->width; i++)
    {
      _image->data[(currentRow*_image->width+i)*3+0]=palette[indices[currentRow*_image->width+i]*3+2];
      _image->data[(currentRow*_image->width+i)*3+1]=palette[indices[currentRow*_image->width+i]*3+1];
      _image->data[(currentRow*_image->width+i)*3+2]=palette[indices[currentRow*_image->width+i]*3+0];//BGR
    }
  }
  return true;
}

bool RsImageTools::loadUncompressedTrueColorTGA(const char * filename, RsTGAimage* _image)
{
  unsigned char TGAheader[12]={0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //Uncompressed TGA header
  unsigned char TGAcompare[12];           //Used to compare TGA header
  unsigned char header[6];              //First 6 useful bytes of the header
  unsigned int  imageSize;              //Stores Image size when in RAM

  FILE * file = fopen(filename, "rb");        //Open the TGA file

  if(file == NULL)                //Does the file exist?
    return false;

  if( fread(TGAcompare, 1, sizeof(TGAcompare), file)!=sizeof(TGAcompare)||  //Are there 12 bytes to read?
    memcmp(TGAheader, TGAcompare, sizeof(TGAheader))!=0 ||          //Is the header correct?
    fread(header, 1, sizeof(header), file)!=sizeof(header))   //Read next 6 bytes
  {
    fclose(file);               //If anything else failed, close the file
    return false;
  }

  //save data into class member variables
  _image->width = header[1]*256+header[0];           //determine the image width
  _image->height = header[3]*256+header[2];            //determine image height
  _image->components = header[4]/3;

  if(_image->width <=0 ||                      //if width <=0
		  _image->height<=0 ||                      //or height<=0
     (header[4] !=24 && header[4]!=32))                   //bpp not 24 or 32
  {
    fclose(file);                     //close the file
    return false;
  }

  //set format
  if(header[4] == 24){
	  _image->format = GL_BGR;
	  _image->intlFormat = GL_RGB8;
  }
  else{
	  _image->format = GL_BGRA;
	  _image->intlFormat = GL_RGBA8;
  }

  imageSize = _image->width*_image->height*_image->components;

  if(_image->data){
    delete[] _image->data;
    _image->data = 0;
  }

  _image->data = new char[imageSize];

  if(_image->data==NULL)                     //Does the storage memory exist?
  {
    fclose(file);
    return false;
  }

  //read in the image data
  fread(_image->data, 1, imageSize, file);
  fclose(file);
  return true;
}

//load a compressed TGA texture (24 or 32 bpp)
bool RsImageTools::loadCompressedTrueColorTGA(const char * filename, RsTGAimage* _image)
{
  unsigned char TGAheader[12]={0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0};  //Compressed TGA header
  unsigned char TGAcompare[12];           //Used to compare TGA header
  unsigned char header[6];              //First 6 useful bytes of the header
  unsigned int  bytesPerPixel;            //bytes per pixel
  unsigned int  imageSize;              //Stores Image size when in RAM

  FILE * file = fopen(filename, "rb");        //Open the TGA file

  if(file == NULL)                //Does the file exist?
    return false;


  if( fread(TGAcompare, 1, sizeof(TGAcompare), file)!=sizeof(TGAcompare)||  //Are there 12 bytes to read?
    memcmp(TGAheader, TGAcompare, sizeof(TGAheader))!=0 ||          //Is the header correct?
    fread(header, 1, sizeof(header), file)!=sizeof(header))   //Read next 6 bytes
  {
    fclose(file);               //If anything else failed, close the file
    return false;
  }

  //save data into class member variables
  _image->width = header[1]*256+header[0];            //determine the image width
  _image->height = header[3]*256+header[2];            //determine image height
  _image->components = header[4]/8;
  bytesPerPixel = _image->components;
  cerr << "components: bpp: " << bytesPerPixel << endl;

  if(_image->width  <=0 ||                      //if width <=0
		  _image->height <=0 ||                      //or height<=0
     (header[4] !=24 && header[4] !=32))                   //bpp not 24 or 32
  {
    fclose(file);                     //close the file
    return false;
  }

  //set format
  if(header[4] == 24)
  {
	  _image->format = GL_BGR;
	  _image->intlFormat = GL_RGB8;
  }
  else
  {
	  _image->format = GL_BGRA;
	  _image->intlFormat = GL_RGBA8;
  }

  imageSize = _image->width*_image->height*_image->components;

  if(_image->data){
	  delete[] _image->data;
	  _image->data = 0;
  }
  _image->data = new char[imageSize];

  if(!_image->data)                         //Does the storage memory exist?
  {
    fclose(file);
    return false;
  }

  //read in the image data
  int pixelCount  = _image->width*_image->height;
  int currentPixel= 0;
  int currentByte = 0;
  GLubyte *colorBuffer = new GLubyte[_image->components];

  do
  {
    unsigned char chunkHeader=0;

    if(!fread(&chunkHeader, sizeof(unsigned char), 1, file))
    {
      if(file)
        fclose(file);
      delete[] _image->data;
      _image->data = 0;
      return false;
    }

    if(chunkHeader<128) //Read raw color values
    {
      chunkHeader++;

      for(short counter=0; counter<chunkHeader; counter++)
      {
        if(fread(colorBuffer, 1, bytesPerPixel, file) != bytesPerPixel)
        {
          if(file)
            fclose(file);
          delete[] colorBuffer;
          colorBuffer = 0;
          delete[] _image->data;
          _image->data = 0;
          return false;
        }

        //transfer pixel color to data (swapping r and b values)
        _image->data[currentByte]   = colorBuffer[0];
        _image->data[currentByte+1] = colorBuffer[1];
        _image->data[currentByte+2] = colorBuffer[2];

        if(bytesPerPixel==4)
        	_image->data[currentByte+3]=colorBuffer[3];

        currentByte+=bytesPerPixel;
        currentPixel++;

        if(currentPixel > pixelCount)
        {
          if(file)
            fclose(file);
          delete[] colorBuffer;
          colorBuffer = 0;
          delete[] _image->data;
          _image->data = 0;
          return false;
        }
      }
    }
    else  //chunkHeader>=128
    {
      chunkHeader-=127;

      if(fread(colorBuffer, 1, bytesPerPixel, file) != bytesPerPixel)
      {
        if(file)
          fclose(file);
        delete[] colorBuffer;
        colorBuffer = 0;
        delete[] _image->data;
        _image->data = 0;
        return false;
      }

      for(short counter=0; counter<chunkHeader; counter++)
      {
        //transfer pixel color to data (swapping r and b values)
    	  _image->data[currentByte]   = colorBuffer[0];
    	  _image->data[currentByte+1] = colorBuffer[1];
    	  _image->data[currentByte+2] = colorBuffer[2];

        if(bytesPerPixel==4)
        	_image->data[currentByte+3]=colorBuffer[3];

        currentByte+=bytesPerPixel;
        currentPixel++;

        if(currentPixel > pixelCount)
        {
          if(file)
            fclose(file);
          delete[] colorBuffer;
          colorBuffer = 0;
          delete[] _image->data;
          return false;
        }
      }
    }
  }while(currentPixel<pixelCount);

  fclose(file);
  delete[] colorBuffer;
  colorBuffer = 0;
  return true;
}
