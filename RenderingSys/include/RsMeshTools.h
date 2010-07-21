/**
 * @file	RsMeshTools.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 14.05.2010
 * @brief 	RsMeshTools class declaration.
 */

#ifndef RSMESHTOOLS_H_
#define RSMESHTOOLS_H_

#include <boost/system/config.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/tokenizer.hpp>
#include <boost/regex.hpp>

#include "RsStructs.h"
#include "RsObjModel.h"

#include <map>
#include <set>
#include <string>

/**
 * @class RsMeshTools (singleton)
 * @brief A collection of functions for parsing, loading and manipulating mesh-data.
 */
class RsMeshTools
{
public:
	static RsMeshTools* getSingleton();
	RsObjModel* loadObj(boost::filesystem::path* _file);
private:
	RsMeshTools();
	virtual ~RsMeshTools();
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

    /**
     * @brief Method analyzes a face-line (beginning with 'f') from the obj file.
     * Used to determine which components are available (normals, textures) for this group.
     * @param _tokens Tokens of the face-line
     * @return
     */
    unsigned analyzeFaceLine(tokenizer* _tokens);

    /**
     * @brief This method does the first pass when loading the obj.
     * In this ste, the RsObjInfo file is used to collect meta information about the model.
     * @param _file Path to obj-file.
     * @param _info The info-object which gets filled during the course of the 1st loading pass.
     */
	void parseObj(boost::filesystem::path* _file, RsObjInfo* _info);

	/**
	 * @brief Load a material libary file into an RsObjInfo object.
	 * @param _file path to the material library file.
	 * @param _info the info object to the material in.
	 */
    void loadMtlLib(boost::filesystem::path* _file, RsObjInfo* _info);

    /**
     * @brief Loads all texture maps encountered during obj loading
     * @param _path The path to where the image lies - the actual filename is stored within the info object.
     * @param _info The info file
     */
    void loadTextures(boost::filesystem::path* _path, RsObjInfo* _info);

    static RsMeshTools* instance;

	class Guard {
    public:
		~Guard();
    };
    friend class Guard;

};

#endif /* RSMESHTOOLS_H_ */
