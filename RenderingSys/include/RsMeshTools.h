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
#include <string>

/**
 * @class RsMeshTools
 * @brief A collection of functions for parsing, loading and manipulating mesh-data.
 */
class RsMeshTools
{
public:
	static RsMeshTools* getSingleton();
	RsObjModel* loadObj(boost::filesystem::path* _file);
	void parseObj(boost::filesystem::path* _file, RsObjInfo* _info);
private:
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

    unsigned analyzeFaceLine(tokenizer* _tokens);
    void loadMtlLib(boost::filesystem::path* _file, std::map<std::string, RsMaterial>* _mtlMap);
    static RsMeshTools* instance;
	RsMeshTools();
	virtual ~RsMeshTools();
    class Guard {
    public:
		~Guard();
    };
    friend class Guard;

};

#endif /* RSMESHTOOLS_H_ */
