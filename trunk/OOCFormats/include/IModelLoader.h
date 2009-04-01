/*
 * IModelLoader.h
 *
 *  Created on: 02.02.2009
 *      Author: ava
 */

#ifndef IMODELLOADER_H_
#define IMODELLOADER_H_

#include "../../OOCTools/include/declarations.h"

namespace oocformats {

class IModelLoader {
public:
	IModelLoader();
	virtual ~IModelLoader();
	virtual void *readFile() = 0;
	virtual ooctools::Model *getModel() = 0;

private:

};

} // moocformats
#endif /* IMODELLOADER_H_ */
