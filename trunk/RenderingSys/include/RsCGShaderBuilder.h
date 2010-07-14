/**
 * @file	RsCGShaderBuilder.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 09.07.2010
 * @brief 	RsCGShaderBuilder class declaration.
 */


#ifndef RSCGSHADERBUILDER_H
#define	RSCGSHADERBUILDER_H

#include <GL/glew.h>
#include "Cg/cg.h"
#include "Cg/cgGL.h"

class RsCGShaderBuilder {
public:
    enum CGShaderType {
        VertexShader = 0,
        GeometryShader = 1,
        FragmentShader = 2};

    static unsigned BuildShader(const char* _src, CGShaderType _type);
    static unsigned BuildShader(const char* _vs, const char* _fs);
    static unsigned BuildShader(const char* _vs, const char* _gs, const char* _fs);

    static CGprogram BuildShaderFromFile(const std::string &_file, CGShaderType _type);
    static unsigned BuildShaderFromFile(const std::string &_vsfile, const std::string &_fsfile);
    static unsigned BuildShaderFromFile(const std::string &_vsfile, const std::string &_gsfile, const std::string &_fsfile);
    static CGprogram BuildShaderFromFile(const std::string &_file, const char* _vsMain, const char* _gsMain, const char* _fsMain);

    static void EnableShader(CGprogram _shader);
    static void DisableShader(CGprogram _shader);
    static const char* IsCGError();
    static void VerboseCompileOutput(CGprogram _shader, CGprofile _profile, const std::string& _file);
    static void PrintShadingLanguageString();

private:
    RsCGShaderBuilder();
    virtual ~RsCGShaderBuilder();

    static bool priPrintInfoLog(GLhandleARB _handle, const char* _shader);
    static char* priReadShaderSource(const char *_filename);
    static std::string priLanguageVersion;
    static bool priInitialized;
    static CGcontext priContext;
	static CGprofile priProfiles[3];
	static CGbool priHasVertexShaderSuport;
	static CGbool priHasGeometryShaderSuport;
	static CGbool priHasFragmentShaderSuport;

    static void priInit();
};

#endif	/* RSCGSHADERBUILDER_H */

