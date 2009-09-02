/**
 * @file	ClassId.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	ClassId class declaration.
 */

#ifndef CLASSID_H_
#define CLASSID_H_

#include <string>

#include "declarations.h"

namespace oocframework {

#define QUOTIENT 0x04c11db7

/**
 * @class ClassId
 * @brief Simple implementation of a Runtime-Type-Information system.
 * Depends on a simple crc32 algorithm to verify and ensure uniqueness of class-ids.
 */
class ClassId {
public:
	/**
	 * @brief Constructor of class.
	 * Expects a unique string to calculate the crc32.
	 * @param name string
	 */
	ClassId(std::string name);
	virtual ~ClassId();

	/**
	 * @brief Simple equality-comparison-operator.
	 */
	bool operator==(const ClassId& rhs);

	/**
	 * @brief Simple inequality-comparison-operator.
	 */
	bool operator!=(const ClassId& rhs);

	/**
	 * @brief Returns the uint from the crc for direct comparison.
	 * @return
	 */
	unsigned int getId() const {return mCrcId;};

	/**
	 * @brief Returns half of the id.
	 * This dirty little hack was introduced because MPI only can send int as message type,
	 * but a crc calculation needs full 32byte length to avoid even the rough collisions.
	 * A Warning, though: This is by no means safe!!! It shjould work in a project as small
	 * as this one.
	 * @return the positive int-version of the classid, a.k.a. half of the id.
	 */
	int getShortId() const {return (int)(mCrcId>>1);};

	/**
	 * @brief Returns the real classname. This serves just for debugging purposes to
	 * verify that there are no crc-collisions within the project's used class names.
	 * @return a string with the classname.
	 */
	const std::string getClassName() const {return mClassName;};

private:
	unsigned int mCrcId;
	std::string mClassName;

	/**
	 * @brief Performs the crc32 encoding. It comes in two flavours: this
	 * one takes a c-string and the length of given string and the other one
	 * takes a std::string.
	 * @return the uint of the crc32
	 * @see encode(const std::string& name)
	 */
	unsigned int encode(const unsigned char *data, int len) const;

	/**
	 * @see encode(const unsigned char *data, int len)
	 */
	unsigned int encode(const std::string& name) const;

	/**
	 * @brief It's a table that is used to generate the crc32 code.
	 * It needs to be initialized once. Hence the static-ness.
	 *
	 */
	static unsigned int mCrctab[256];
	static bool initiated;

	static void crc32_init(void);
	static bool isInited(){return initiated;};
};

}

#endif /* CLASSID_H_ */
