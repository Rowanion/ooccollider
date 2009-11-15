/**
 * @file	CgToolkit.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 03.02.2009
 * @brief   Class definition of CgToolkit
 */

#include "CgToolkit.h"

#include <string>
#include <iostream>
#include <cstdlib>
#include <cstdio>

#include "GlToolkit.h"
#include <Cg/cg.h>
#include <Cg/cgGL.h>

namespace ooctools {
using namespace std;

CgToolkit *CgToolkit::mPriInstancePtr = 0;
CgToolkit *CgToolkit::getSingleton()
{
	if (CgToolkit::mPriInstancePtr==0)
		CgToolkit::mPriInstancePtr = new CgToolkit();
	return CgToolkit::mPriInstancePtr;
}

CgToolkit::CgToolkit()
{
	// TODO Auto-generated constructor stub

}

CgToolkit::~CgToolkit()
{
}

void
CgToolkit::initCG(bool verbose)
{
	cgContext = cgCreateContext();
//	cgSetErrorCallback(CgToolkit::CheckCgError);

    CGbool arbfp1 = cgGLIsProfileSupported(CG_PROFILE_ARBFP1);
    CGbool arbvp1 = cgGLIsProfileSupported(CG_PROFILE_ARBVP1);

    CGbool fp40   = cgGLIsProfileSupported(CG_PROFILE_FP40);
    CGbool fp30   = cgGLIsProfileSupported(CG_PROFILE_FP30);
    CGbool fp20   = cgGLIsProfileSupported(CG_PROFILE_FP20);

    CGbool vp40   = cgGLIsProfileSupported(CG_PROFILE_VP40);
    CGbool vp30   = cgGLIsProfileSupported(CG_PROFILE_VP30);
    CGbool vp20   = cgGLIsProfileSupported(CG_PROFILE_VP30);

    //CGbool gp4vp   = cgGLIsProfileSupported(7011);
    //CGbool gp4fp   = cgGLIsProfileSupported(7010);
    //CGbool gp4gp   = cgGLIsProfileSupported(7012);

    if (verbose) {
    	if (cgIsContext(cgContext) == CG_FALSE)
    		cerr << "No cg-context created!" << endl;
    	else cout << "cg-context is a-ok!" << endl;

    	cgGLSetDebugMode(CG_TRUE);
    	cgVertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
    	cgGLSetOptimalOptions(cgVertexProfile);
    	CheckCgError();
    	//exit(1);

    	if (arbvp1 == CG_FALSE)
    		cout << "[CG]: CG_PROFILE_ARBVP1 not supported." << endl;
    	if (arbfp1 == CG_FALSE)
    		cout << "[CG]: CG_PROFILE_ARBFP1 not supported.\n" << endl;

    	if (fp40 == CG_FALSE)
    		cout << "[CG]: CG_PROFILE_FP40 not supported.\n" << endl;
    	if (fp30 == CG_FALSE)
    		cout << "[CG]: CG_PROFILE_FP30 not supported.\n" << endl;
    	if (fp20 == CG_FALSE)
    		cout << "[CG]: CG_PROFILE_FP20 not supported.\n" << endl;

    	if (vp40 == CG_FALSE)
    		cout << "[CG]: CG_PROFILE_VP40 not supported.\n\n" << endl;
    	if (vp30 == CG_FALSE)
    		cout << "[CG]: CG_PROFILE_VP30 not supported.\n\n" << endl;
    	if (vp20 == CG_FALSE)
    		cout << "[CG]: CG_PROFILE_VP20 not supported.\n\n" << endl;
/*
    	if (gp4vp == CG_FALSE)
    		cout << "[CG]: CG_PROFILE_GP4VP not supported.\n\n" << endl;
    	if (gp4fp == CG_FALSE)
    		cout << "[CG]: CG_PROFILE_GP4FP not supported.\n\n" << endl;
    	if (gp4gp == CG_FALSE)
    		cout << "[CG]: CG_PROFILE_GP4GP not supported.\n\n" << endl;
    		*/
    }

    if (cgContext == 0)
    	cerr << "Failed to Create Cg Context" << endl;

    // get best Vertexprofile
    cgVertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
    if ( cgVertexProfile == CG_PROFILE_UNKNOWN)
    	cerr << "Invalid Vertexprofile type!" << endl;
    cgGLSetOptimalOptions(cgVertexProfile);
    if (verbose)
    	cout << "[CG]: Latest CG_Vertexprofile for this gfxcard: " << cgVertexProfile << " = " << cgGetProfileString(cgGLGetLatestProfile(CG_GL_VERTEX)) << "." << endl;


    // get best FragProfile
    cgFragProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
    if ( cgFragProfile == CG_PROFILE_UNKNOWN)
    	cout << "Invalid Fragprofile type" << endl;
    cgGLSetOptimalOptions(cgFragProfile);
    if (verbose)
    	cout << "[CG]: Latest CG_Fragprofile for this gfxcard: " << cgFragProfile << " = " << cgGetProfileString(cgGLGetLatestProfile(CG_GL_FRAGMENT)) << endl;


    // get best GeoProfile
    std::string vpString = cgGetProfileString(cgGLGetLatestProfile(CG_GL_VERTEX));
    if (vpString == "gp4vp"){
    	cgGeometryProfile = cgGLGetLatestProfile(CG_GL_GEOMETRY);
    	if ( cgGeometryProfile == CG_PROFILE_UNKNOWN)
    		cout << "Invalid Geometryprofile type" << endl;
    	cgGLSetOptimalOptions(cgGeometryProfile);
    	if (verbose) {
    		cout << "[CG]: Latest CG_Geometryprofile for this gfxcard: " << cgGeometryProfile << " = " << cgGetProfileString(cgGLGetLatestProfile(CG_GL_GEOMETRY)) << endl;
    		// get geometry program limits
    		GLint max_output_vertices, max_total_output_components;
    		glGetProgramivARB(GL_GEOMETRY_PROGRAM_NV, GL_MAX_PROGRAM_OUTPUT_VERTICES_NV, &max_output_vertices);
    		glGetProgramivARB(GL_GEOMETRY_PROGRAM_NV, GL_MAX_PROGRAM_TOTAL_OUTPUT_COMPONENTS_NV, &max_total_output_components);
    		printf("max output vertices = %d\n", max_output_vertices);
    		printf("max total output components = %d\n", max_total_output_components);
    	}
    }
}

void CgToolkit::CheckCgError()
{
    CGerror lastError = cgGetError();
    if(lastError)
    {
        printf("%s\n", cgGetErrorString(lastError));
//        printf("%s\n", cgGetLastListing(cgContext));
        exit(1);
    }
}

void
CgToolkit::unloadCgProgram(CGprogram prog)
{
	  cgDestroyProgram(prog);
}

void
CgToolkit::disableCg()
{
	  cgDestroyContext(cgContext);
}

void
CgToolkit::startCgShader(CGprofile profile, CGprogram prog)
{
	cgGLEnableProfile(profile);
	cgGLBindProgram(prog);
}

void
CgToolkit::stopCgShader(CGprofile profile)
{
	cgGLDisableProfile(profile);
}


CGprogram
CgToolkit::loadCgShader(CGprofile profile, string filename, bool verbose, std::string entry)
{
	CGprogram shaderprog;
	if (entry.size() == 0){
		shaderprog = cgCreateProgramFromFile(cgContext, CG_SOURCE, filename.c_str(), profile, 0,0);
	}
	else {
		shaderprog = cgCreateProgramFromFile(cgContext, CG_SOURCE, filename.c_str(), profile, entry.c_str(),0);
	}
	cout << shaderprog << "-------------------------" << endl;

	if (verbose && cgGetLastListing(cgContext) != 0) {
		cerr << "LAST LISTING----" << cgGetLastListing(cgContext) << "----" << endl;

		cerr << "---- PROGRAM BEGIN ----" << endl <<
		cgGetProgramString(shaderprog, CG_COMPILED_PROGRAM) << "---- PROGRAM END ----" << endl;
	}
	if (verbose && shaderprog == 0)
		cerr << cgGetErrorString(cgGetError()) << endl;
	cgGLLoadProgram(shaderprog);
	CheckCgError();
	//  checkGlError(2000);
	//       CG_INVALID_PROFILE_ERROR is generated if prog's profile is not a supported OpenGL profile.
	//       CG_INVALID_PROGRAM_HANDLE_ERROR is generated if prog is not a valid program.
	//       CG_PROGRAM_LOAD_ERROR is generated if the program fails to load for any reason.

	cout << "[CG]: shaderprog(" << filename << "):" << shaderprog << endl;
	if (shaderprog != 0) {
		CGprofile newProfile = cgGetProgramProfile(shaderprog);

		if (verbose)
			cout << " Profile " << newProfile << " = " << cgGetProfileString(newProfile) << endl;
	}
	else
		cout << endl;

	return shaderprog;
}

void
CgToolkit::cgDisableTex(string s)
{
	cgGLDisableTextureParameter(cgP[s]);
}

void
CgToolkit::cgEnableTex(string s)
{
	cgGLEnableTextureParameter(cgP[s]);
}

void
CgToolkit::cgNewParam(string s, CGprogram prog)
{
	cgP[s] = cgGetNamedParameter(prog, s.c_str());
}

void
CgToolkit::cgSetMatrixMvp(string s)
{
	cgGLSetStateMatrixParameter(cgP[s],
			CG_GL_MODELVIEW_PROJECTION_MATRIX,
			CG_GL_MATRIX_IDENTITY);
}

void
CgToolkit::cgSetMatrixMvpi(string s)
{
	cgGLSetStateMatrixParameter(cgP[s],
			CG_GL_MODELVIEW_PROJECTION_MATRIX,
			CG_GL_MATRIX_INVERSE);
}

void
CgToolkit::cgSetMatrixMvpit(string s)
{
	cgGLSetStateMatrixParameter(cgP[s],
			CG_GL_MODELVIEW_PROJECTION_MATRIX,
			CG_GL_MATRIX_INVERSE_TRANSPOSE);
}

void
CgToolkit::cgSetMatrixMvpt(string s)
{
	cgGLSetStateMatrixParameter(cgP[s],
			CG_GL_MODELVIEW_PROJECTION_MATRIX,
			CG_GL_MATRIX_TRANSPOSE);
}

void
CgToolkit::cgSetMatrixMv(string s)
{
	cgGLSetStateMatrixParameter(cgP[s],
			CG_GL_MODELVIEW_MATRIX,
			CG_GL_MATRIX_IDENTITY);
}

void
CgToolkit::cgSetMatrixMvi(string s)
{
	cgGLSetStateMatrixParameter(cgP[s],
			CG_GL_MODELVIEW_MATRIX,
			CG_GL_MATRIX_INVERSE);
}

void
CgToolkit::cgSetMatrixMvit(string s)
{
	cgGLSetStateMatrixParameter(cgP[s],
			CG_GL_MODELVIEW_MATRIX,
			CG_GL_MATRIX_INVERSE_TRANSPOSE);
}

void
CgToolkit::cgSetMatrixMvt(string s)
{
	cgGLSetStateMatrixParameter(cgP[s],
			CG_GL_MODELVIEW_MATRIX,
			CG_GL_MATRIX_TRANSPOSE);
}

void
CgToolkit::cgSetMatrixP(string s)
{
	cgGLSetStateMatrixParameter(cgP[s],
			CG_GL_PROJECTION_MATRIX,
			CG_GL_MATRIX_IDENTITY);
}

void
CgToolkit::cgSetMatrixPi(string s)
{
	cgGLSetStateMatrixParameter(cgP[s],
			CG_GL_PROJECTION_MATRIX,
			CG_GL_MATRIX_INVERSE);
}

void
CgToolkit::cgSetMatrixPit(string s)
{
	cgGLSetStateMatrixParameter(cgP[s],
			CG_GL_PROJECTION_MATRIX,
			CG_GL_MATRIX_INVERSE_TRANSPOSE);
}

void
CgToolkit::cgSetMatrixPt(string s)
{
	cgGLSetStateMatrixParameter(cgP[s],
			CG_GL_PROJECTION_MATRIX,
			CG_GL_MATRIX_TRANSPOSE);
}

void
CgToolkit::cgSetMatrixT(string s)
{
	cgGLSetStateMatrixParameter(cgP[s],
			CG_GL_TEXTURE_MATRIX,
			CG_GL_MATRIX_IDENTITY);
}

void
CgToolkit::cgSetMatrixTi(string s)
{
	cgGLSetStateMatrixParameter(cgP[s],
			CG_GL_TEXTURE_MATRIX,
			CG_GL_MATRIX_INVERSE);
}

void
CgToolkit::cgSetMatrixTit(string s)
{
	cgGLSetStateMatrixParameter(cgP[s],
			CG_GL_TEXTURE_MATRIX,
			CG_GL_MATRIX_INVERSE_TRANSPOSE);
}

void
CgToolkit::cgSetMatrixTt(string s)
{
	cgGLSetStateMatrixParameter(cgP[s],
			CG_GL_TEXTURE_MATRIX,
			CG_GL_MATRIX_TRANSPOSE);
}

/**
 * @param s Name of the parameter to be set
 * @param offset An offset into the array parameter to start setting from. A value of 0 will start setting from the first element of the array.
 * @param nelements The number of elements to set. A value of 0 will default to the number of elements in the array minus the offset value.
 * @param v The array of values to set the parameter to. This must be a contiguous set of values that total nelements times the number of elements in the matrix.
 * @param v_offset
 */
void
CgToolkit::cgSetArrayParam1d(string s, long offset, long nelements, double v[])
{
	cgGLSetParameterArray1d(cgP[s], offset, nelements, v);
}

void
CgToolkit::cgSetArrayParam1f(string s, long offset, long nelements, float v[])
{
	cgGLSetParameterArray1f(cgP[s], offset, nelements, v);
}

void
CgToolkit::cgSetArrayParam2d(string s, long offset, long nelements, double v[])
{
	cgGLSetParameterArray2d(cgP[s], offset, nelements, v);
}

void
CgToolkit::cgSetArrayParam2f(string s, long offset, long nelements, float v[])
{
	cgGLSetParameterArray2f(cgP[s], offset, nelements, v);
}

void
CgToolkit::cgSetArrayParam3d(string s, long offset, long nelements, double v[])
{
	cgGLSetParameterArray3d(cgP[s], offset, nelements, v);
}

void
CgToolkit::cgSetArrayParam3f(string s, long offset, long nelements, float v[])
{
	cgGLSetParameterArray1f(cgP[s], offset, nelements, v);
}

void
CgToolkit::cgSetArrayParam4d(string s, long offset, long nelements, double v[])
{
	cgGLSetParameterArray4d(cgP[s], offset, nelements, v);
}

void
CgToolkit::cgSetArrayParam4f(string s, long offset, long nelements, float v[])
{
	cgGLSetParameterArray4f(cgP[s], offset, nelements, v);
}

void
CgToolkit::cgSetMatrixParamdc(string s, long offset, long nelements, double m[])
{
	cgGLSetMatrixParameterArraydc(cgP[s], offset, nelements, m);
}

void
CgToolkit::cgSetMatrixParamdr(string s, long offset, long nelements, double m[])
{
	cgGLSetMatrixParameterArraydr(cgP[s], offset, nelements, m);
}

void
CgToolkit::cgSetMatrixParamfc(string s, long offset, long nelements, float m[])
{
	cgGLSetMatrixParameterArrayfc(cgP[s], offset, nelements, m);
}

void
CgToolkit::cgSetMatrixParamfr(string s, long offset, long nelements, float m[])
{
	cgGLSetMatrixParameterArrayfr(cgP[s], offset, nelements, m);
}

void
CgToolkit::cgSetParam1d(string s, double x)
{
	cgGLSetParameter1d(cgP[s], x);
}

void
CgToolkit::cgSetParam1dv(string s, double v[])
{
	cgGLSetParameter1dv(cgP[s], v);
}

void
CgToolkit::cgSetParam1f(string s, float x)
{
	cgGLSetParameter1f(cgP[s], x);
}

void
CgToolkit::cgSetParam1fv(string s, float v[])
{
	cgGLSetParameter1fv(cgP[s], v);
}

void
CgToolkit::cgSetParam2d(string s, double x, double y)
{
	cgGLSetParameter2d(cgP[s], x, y);
}

void
CgToolkit::cgSetParam2dv(string s, double v[])
{
	cgGLSetParameter2dv(cgP[s], v);
}

void
CgToolkit::cgSetParam2f(string s, float x, float y)
{
	cgGLSetParameter2f(cgP[s], x, y);
}

void
CgToolkit::cgSetParam2fv(string s, float v[])
{
	cgGLSetParameter2fv(cgP[s], v);
}

void
CgToolkit::cgSetParam3d(string s, double x, double y, double z)
{
	cgGLSetParameter3d(cgP[s], x, y, z);
}

void
CgToolkit::cgSetParam3dv(string s, double v[])
{
	cgGLSetParameter3dv(cgP[s], v);
}

void
CgToolkit::cgSetParam3f(string s, float x, float y, float z)
{
	cgGLSetParameter3f(cgP[s], x, y, z);
}

void
CgToolkit::cgSetParam3f(CGparameter _cgp, float x, float y, float z)
{
	cgGLSetParameter3f(_cgp, x, y, z);
}

void
CgToolkit::cgSetParam3fv(string s, const float v[])
{
	cgGLSetParameter3fv(cgP[s], v);
}

void
CgToolkit::cgSetParam3fv(CGparameter _cgp, const float v[])
{
	cgGLSetParameter3fv(_cgp, v);
}

void
CgToolkit::cgSetParam4d(string s, double x, double y, double z, double w)
{
	cgGLSetParameter4d(cgP[s], x, y, z, w);
}

void
CgToolkit::cgSetParam4dv(string s, double v[])
{
	cgGLSetParameter4dv(cgP[s], v);
}

void
CgToolkit::cgSetParam4f(string s, float x, float y, float z, float w)
{
	cgGLSetParameter4f(cgP[s], x, y, z, w);
}

void
CgToolkit::cgSetParam4fv(string s, float v[])
{
	cgGLSetParameter4fv(cgP[s], v);
}
void
CgToolkit::cgSetTexParam(string s, int texobj)
{
	cgGLSetTextureParameter(cgP[s], texobj);
}

} // namespace OOCTools
