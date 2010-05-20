/*
 * RsMeshTools.h
 *
 *  Created on: 16.03.2010
 *      Author: TheAvatar
 */

#ifndef RSMESHTOOLS_H_
#define RSMESHTOOLS_H_

#include <boost/system/config.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/tokenizer.hpp>
#include <boost/regex.hpp>

#include "RsStructs.h"

class RsMeshTools
{
public:
	static RsMeshTools* getSingleton();
	void loadObj(boost::filesystem::path* _file);
	void parseObj(boost::filesystem::path* _file, ObjInfo* _info);
private:
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

    unsigned analyzeFaceLine(tokenizer* _tokens);

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
