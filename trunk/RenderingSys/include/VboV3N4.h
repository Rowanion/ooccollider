/**
 * @file	VboV3N4.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 20.05.2010
 * @brief 	VboV3N4 class declaration.
 */

#ifndef VBOV3N4_H_
#define VBOV3N4_H_

#include "Vbo.h"
#include "RsStructs.h"

class VboV3N4 : public Vbo{
public:
	VboV3N4();
	virtual ~VboV3N4();
protected:
	RsV3N4 mProData;
};

#endif /* VBOV3N4_H_ */
