/*
 * ModelWriter.h
 *
 *  Created on: 15.02.2009
 *      Author: ava
 */

#ifndef MODELWRITER_H_
#define MODELWRITER_H_

#include <vector>

#include "../../OOCTools/include/declarations.h"

namespace oocformats {

class ModelWriter {
public:
	ModelWriter();
	virtual ~ModelWriter();
	static void writeMetaGrp(ooctools::MetaGroup *_grp);
	static void writeMetaGrps(std::vector<ooctools::MetaGroup> *_grps);
	static void debug();

private:
	void *buildHeader(ooctools::MetaGroup *_grp);
};

} // oocformats

#endif /* MODELWRITER_H_ */
