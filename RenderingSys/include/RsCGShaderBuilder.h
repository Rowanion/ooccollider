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

/**
 * @class RsCGShaderBuilder (singleton)
 * @brief Collection of all things related to CG shaders - loading/enabling and so on
 */
class RsCGShaderBuilder {
public:
    enum CGShaderType {
        VertexShader = 0,
        GeometryShader = 1,
        FragmentShader = 2};

    static RsCGShaderBuilder* getSingleton();

    /**
     * @brief one src string - up to three programs
     * @param _src a c-string containing uncompiled shadercode
     * @param _vsMain entrypoint for a vertex shader - can be 0
     * @param _gsMain entrypoint for a geometry shader - can be 0
     * @param _fsMain entrypoint for a fragment shader - can be 0
     * @return the ready-to-use (combined) CGProgram
     */
    CGprogram BuildShader(const char* _src, const char* _vsMain, const char* _gsMain, const char* _fsMain);

    /**
     * @brief One source string - one particular program
     * @param _src a c-string containing uncompiled shadercode
     * @param _main entrypoint for the shader - shouln't be 0 - wouldn't make sense to call this method
     * @param _type What kind of shader is it? vertex/geometry/fragment
     * @return the ready-to-use CGProgram
     */
    CGprogram BuildShader(const char* _src, const char* _main, RsCGShaderBuilder::CGShaderType _type);

    /**
     * @brief One file - one particular program
     * @param _file path and filename of the vertex shader file
     * @param _main entrypoint for the shader - shouln't be 0 - wouldn't make sense to call this method
     * @param _type What kind of shader is it? vertex/geometry/fragment
     * @return the ready-to-use CGProgram
     */
    CGprogram BuildShaderFromFile(const std::string &_file, const char* _main, RsCGShaderBuilder::CGShaderType _type);

    /**
     * @brief Two files - for exactly two programs: a vertex- and a fragment shader.
     * @param _vsfile path and filename of the vertex shader file
     * @param _vsMain entrypoint for the vertex shader - can be 0
     * @param _fsfile path and filename of the fragement shader file
     * @param _fsMain entrypoint for the fragment shader - can be 0
     * @return the ready-to-use (combined) CGProgram
     */
    CGprogram BuildShaderFromFile(const std::string &_vsfile, const char* _vsMain, const std::string &_fsfile, const char* _fsMain);

    /**
     * @brief One file - up to three programs
     * @param _file path and filename of the shader file, containing all entrypoints
     * @param _vsMain entrypoint for a vertex shader - can be 0
     * @param _gsMain entrypoint for a geometry shader - can be 0
     * @param _fsMain entrypoint for a fragment shader - can be 0
     * @return the ready-to-use (combined) CGProgram
     */
    CGprogram BuildShaderFromFile(const std::string &_file, const char* _vsMain, const char* _gsMain, const char* _fsMain);

    /**
     * @brief Three compiled object files - exactly three programs
     * @param _file path and filename of the shader file, containing all entrypoints
     * @param _vsMain entrypoint for a vertex shader - can be 0
     * @param _gsMain entrypoint for a geometry shader - can be 0
     * @param _fsMain entrypoint for a fragment shader - can be 0
     * @return the ready-to-use (combined) CGProgram
     */
    CGprogram BuildShaderFromObjectFile(const std::string &_vsFile, const char* _vsMain, const std::string &_gsFile, const char* _gsMain, const std::string &_fsFile, const char* _fsMain);

    /**
     * @brief One compiled object file - one particular program
     * @param _file path and filename of the shader file
     * @param _main entrypoint for the shader - shouln't be 0 - wouldn't make sense to call this method
     * @param _type What kind of shader is it? vertex/geometry/fragment
     * @return the ready-to-use CGProgram
     */
    CGprogram BuildShaderFromObjectFile(const std::string &_file, const char* _main, RsCGShaderBuilder::CGShaderType _type);

    /**
     * @brief Shortcut to enable a (combined) shader
     * @param _shader the shader which to enable
     */
    void EnableShader(CGprogram _shader);

    /**
     * @brief Shortcut to disable a (combined) shader
     * @param _shader the shader which to disable
     */
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

    /**
     * @brief shortcut to combine two programs.
     * @note No error checking is done here.
     * Should cause erroneous behavior if one would try to combine two programs from the same domain (eg. two fragment shaders).
     * @param _programs
     * @param _pCount
     * @return
     */
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

