/*
 * RsMeshTools.h
 *
 *  Created on: 16.03.2010
 *      Author: TheAvatar
 */

#ifndef RSMESHTOOLS_H_
#define RSMESHTOOLS_H_

#include <boost/filesystem.hpp>

#include "RsStructs.h"

class RsMeshTools
{
public:
	static RsMeshTools* getSingleton();
	void loadObj(boost::filesystem::path* _file);
private:
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
