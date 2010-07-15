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

    static RsCGShaderBuilder* getSingleton();

    /**
     * @brief one src string - up to three programs
     * @param _src
     * @param _vsMain
     * @param _gsMain
     * @param _fsMain
     * @return
     */
    CGprogram BuildShader(const char* _src, const char* _vsMain, const char* _gsMain, const char* _fsMain);

    /**
     * @brief One source string - one particular program
     * @param _src
     * @param _main
     * @param _type
     * @return
     */
    CGprogram BuildShader(const char* _src, const char* _main, RsCGShaderBuilder::CGShaderType _type);

    /**
     * @brief One file - one particular program
     * @param _file
     * @param _main
     * @param _type
     * @return
     */
    CGprogram BuildShaderFromFile(const std::string &_file, const char* _main, RsCGShaderBuilder::CGShaderType _type);

    /**
     * @brief Two files - for exactly two programs: a vertex- and a fragment shader.
     * @param _vsfile
     * @param _vsMain
     * @param _fsfile
     * @param _fsMain
     * @return
     */
    CGprogram BuildShaderFromFile(const std::string &_vsfile, const char* _vsMain, const std::string &_fsfile, const char* _fsMain);

    /**
     * @brief One file - up to three programs
     * @param _file
     * @param _vsMain
     * @param _gsMain
     * @param _fsMain
     * @return
     */
    CGprogram BuildShaderFromFile(const std::string &_file, const char* _vsMain, const char* _gsMain, const char* _fsMain);

    /**
     * @brief Three compiled object files - exactly three programs
     * @param _file
     * @param _vsMain
     * @param _gsMain
     * @param _fsMain
     * @return
     */
    CGprogram BuildShaderFromObjectFile(const std::string &_vsFile, const char* _vsMain, const std::string &_gsFile, const char* _gsMain, const std::string &_fsFile, const char* _fsMain);

    /**
     * @brief One compiled object file - one particular program
     * @param _file
     * @param _main
     * @param _type
     * @return
     */
    CGprogram BuildShaderFromObjectFile(const std::string &_file, const char* _main, RsCGShaderBuilder::CGShaderType _type);
    void EnableShader(CGprogram _shader);
    void DisableShader(CGprogram _shader);

    /**
     * @brief Checks whether or not a CGerror happened.
     * @return the error-string if a CGerror occurred - 0 otherwise.
     */
    const char* IsCGError();

    /**
     * @brief Send some more information to std::cerr when a compile error occured.
     * @param _shader The CGprogram for which the error occured
     * @param _profile The CGprofile for which the error occured
     * @param _code The source code of the erroneous program or, at least, the its filename.
     */
    void VerboseCompileOutput(CGprogram _shader, CGprofile _profile, const char* _data);
    void PrintShadingLanguageString();
    CGprogram combineAndLoad(CGprogram* _programs, int _pCount);

private:
    RsCGShaderBuilder();
    virtual ~RsCGShaderBuilder();

    bool priPrintInfoLog(GLhandleARB _handle, const char* _shader);
    char* priReadShaderSource(const char *_filename);
    std::string priLanguageVersion;
    bool priInitialized;
    CGcontext priContext;
	CGprofile priProfiles[3];
	CGbool priHasVertexShaderSuport;
	CGbool priHasGeometryShaderSuport;
	CGbool priHasFragmentShaderSuport;

    void priInit();
};

#endif	/* RSCGSHADERBUILDER_H */

