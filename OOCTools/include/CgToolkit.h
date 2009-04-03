/**
 * @file	CgToolkit.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 03.02.2009
 *
 */

#ifndef CGTOOLKIT_H_
#define CGTOOLKIT_H_

#include <string>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>
#include <map>

namespace ooctools {

/**
 * @class CgToolkit
 *
 * @brief Management class for using nVidias Cg.
 *
 * Includes setting shader params, initialization, creating shader programs, etc.
 * Is a Singleton.
 */
class CgToolkit
{
public:
	static CgToolkit *getInstancePtr();

	virtual ~CgToolkit();
	void initCG(bool verbose);
	static void CheckCgError();
	void unloadCgProgram(CGprogram prog);
	void disableCg();
	void startCgShader(CGprofile profile, CGprogram prog);
	void stopCgShader(CGprofile profile);
	CGprogram loadCgShader(CGprofile profile, std::string filename, bool verbose);
	void cgDisableTex(std::string s);
	void cgEnableTex(std::string s);
	void cgNewParam(std::string s, CGprogram prog);
	void cgSetMatrixMvp(std::string s);
	void cgSetMatrixMvpi(std::string s);
	void cgSetMatrixMvpit(std::string s);
	void cgSetMatrixMvpt(std::string s);
	void cgSetMatrixMv(std::string s);
	void cgSetMatrixMvi(std::string s);
	void cgSetMatrixMvit(std::string s);
	void cgSetMatrixMvt(std::string s);
	void cgSetMatrixP(std::string s);
	void cgSetMatrixPi(std::string s);
	void cgSetMatrixPit(std::string s);
	void cgSetMatrixPt(std::string s);
	void cgSetMatrixT(std::string s);
	void cgSetMatrixTi(std::string s);
	void cgSetMatrixTit(std::string s);
	void cgSetMatrixTt(std::string s);
	void cgSetArrayParam1d(std::string s, long offset, long nelements, double v[]);
	void cgSetArrayParam1f(std::string s, long offset, long nelements, float v[]);
	void cgSetArrayParam2d(std::string s, long offset, long nelements, double v[]);
	void cgSetArrayParam2f(std::string s, long offset, long nelements, float v[]);
	void cgSetArrayParam3d(std::string s, long offset, long nelements, double v[]);
	void cgSetArrayParam3f(std::string s, long offset, long nelements, float v[]);
	void cgSetArrayParam4d(std::string s, long offset, long nelements, double v[]);
	void cgSetArrayParam4f(std::string s, long offset, long nelements, float v[]);
	void cgSetMatrixParamdc(std::string s, long offset, long nelements, double m[]);
	void cgSetMatrixParamdr(std::string s, long offset, long nelements, double m[]);
	void cgSetMatrixParamfc(std::string s, long offset, long nelements, float m[]);
	void cgSetMatrixParamfr(std::string s, long offset, long nelements, float m[]);
	void cgSetParam1d(std::string s, double x);
	void cgSetParam1dv(std::string s, double v[]);
	void cgSetParam1f(std::string s, float x);
	void cgSetParam1fv(std::string s, float v[]);
	void cgSetParam2d(std::string s, double x, double y);
	void cgSetParam2dv(std::string s, double v[]);
	void cgSetParam2f(std::string s, float x, float y);
	void cgSetParam2fv(std::string s, float v[]);
	void cgSetParam3d(std::string s, double x, double y, double z);
	void cgSetParam3dv(std::string s, double v[]);
	void cgSetParam3f(std::string s, float x, float y, float z);
	void cgSetParam3f(CGparameter _cgp, float x, float y, float z);
	void cgSetParam3fv(std::string s, const float v[]);
	void cgSetParam3fv(CGparameter _cgp, const float v[]);
	void cgSetParam4d(std::string s, double x, double y, double z, double w);
	void cgSetParam4dv(std::string s, double v[]);
	void cgSetParam4f(std::string s, float x, float y, float z, float w);
	void cgSetParam4fv(std::string s, float v[]);
	void cgSetTexParam(std::string s, int texobj);

	CGcontext cgContext;
	CGprofile /*int*/ cgFragProfile;
	CGprofile /*int*/ cgVertexProfile;
	CGprofile /*int*/ cgGeometryProfile;


private:
	static CgToolkit * mPriInstancePtr;


	CgToolkit();
	//std::map<V3f, int> uft;
	std::map<std::string, CGparameter> cgP;
	//= new HashMap<std::string, CGparameter>();
};

} // end of namespace OOCTools

#endif /* CGTOOLKIT_H_ */
