/**
 * @file	RsCGShaderBuilder.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 09.07.2010
 * @brief 	RsCGShaderBuilder class definition.
 */


#include <fstream>
#include <iostream>
#include "RsCGShaderBuilder.h"
#include <boost/filesystem.hpp>

RsCGShaderBuilder::RsCGShaderBuilder()
{
	this->priLanguageVersion = std::string();
	this->priHasVertexShaderSuport = false;
	this->priHasGeometryShaderSuport = false;
	this->priHasFragmentShaderSuport = false;
	this->priInit();
}

RsCGShaderBuilder::~RsCGShaderBuilder() {
	cgDestroyContext(priContext);
}

RsCGShaderBuilder* RsCGShaderBuilder::getSingleton()
{
	static RsCGShaderBuilder instance;
	return &instance;
}

void RsCGShaderBuilder::priInit()
{
	//TODO complete
	this->priContext = cgCreateContext();
#ifdef CG_DEBUG
	cgGLSetDebugMode(CG_TRUE);
#endif
	if (!cgIsContext(this->priContext)){
		std::cerr << RsCGShaderBuilder::IsCGError() << std::endl;
		std::cerr << "CG: Failed to get a valid cg context!" << std::endl;
	}

	this->priProfiles[RsCGShaderBuilder::VertexShader] = cgGLGetLatestProfile(CG_GL_VERTEX);
	this->priProfiles[RsCGShaderBuilder::GeometryShader] = cgGLGetLatestProfile(CG_GL_GEOMETRY);
	this->priProfiles[RsCGShaderBuilder::FragmentShader] = cgGLGetLatestProfile(CG_GL_FRAGMENT);

	if (cgIsProfileSupported(this->priProfiles[RsCGShaderBuilder::VertexShader])){
		cgGLSetOptimalOptions(this->priProfiles[RsCGShaderBuilder::VertexShader]);
		this->priHasVertexShaderSuport = CG_TRUE;
	}
	if (cgIsProfileSupported(this->priProfiles[RsCGShaderBuilder::GeometryShader])){
		cgGLSetOptimalOptions(this->priProfiles[RsCGShaderBuilder::GeometryShader]);
		this->priHasGeometryShaderSuport = CG_TRUE;
	}
	if (cgIsProfileSupported(this->priProfiles[RsCGShaderBuilder::FragmentShader])){
		cgGLSetOptimalOptions(this->priProfiles[RsCGShaderBuilder::FragmentShader]);
		this->priHasFragmentShaderSuport = CG_TRUE;
	}

	this->priLanguageVersion = std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

	// -----------------------------------------------

}
CGprogram RsCGShaderBuilder::BuildShader(const char* _src, const char* _main, RsCGShaderBuilder::CGShaderType _type)
{
	if (_src == 0)
	{
		return 0;
	}

	CGprogram aProgram;
	aProgram = cgCreateProgram(RsCGShaderBuilder::priContext, CG_SOURCE, _src, priProfiles[_type], _main ,0);
	cgGLLoadProgram(aProgram);
	if(RsCGShaderBuilder::IsCGError()){
		VerboseCompileOutput(aProgram, priProfiles[_type], _src);
	}
    return aProgram;

}

CGprogram RsCGShaderBuilder::BuildShader(const char* _src, const char* _vsMain, const char* _gsMain, const char* _fsMain)
{
    CGprogram programs[3];

    int programCount = 0;
	if (_vsMain != 0 && priHasVertexShaderSuport){
		programs[programCount] = cgCreateProgram(
			RsCGShaderBuilder::priContext,
			CG_SOURCE,
			_src,
			priProfiles[RsCGShaderBuilder::VertexShader],
			_vsMain,
			0);
#ifdef CG_DEBUG
		if (RsCGShaderBuilder::IsCGError()){
			VerboseCompileOutput(programs[programCount], priProfiles[RsCGShaderBuilder::VertexShader], _src);
			return 0;
		}
#endif
		programCount++;
	}
	if (_gsMain != 0 && priHasGeometryShaderSuport){
		programs[programCount] = cgCreateProgram(
			RsCGShaderBuilder::priContext,
			CG_SOURCE,
			_src,
			priProfiles[RsCGShaderBuilder::GeometryShader],
			_gsMain,
			0);
#ifdef CG_DEBUG
		if (RsCGShaderBuilder::IsCGError()){
			VerboseCompileOutput(programs[programCount], priProfiles[RsCGShaderBuilder::GeometryShader], _src);
			return 0;
		}
#endif
		programCount++;
	}
	if (_fsMain != 0 && priHasFragmentShaderSuport){
		programs[programCount] = cgCreateProgram(
			RsCGShaderBuilder::priContext,
			CG_SOURCE,
			_src,
			priProfiles[RsCGShaderBuilder::FragmentShader],
			_fsMain,
			0);
#ifdef CG_DEBUG
		if (RsCGShaderBuilder::IsCGError()){
			VerboseCompileOutput(programs[programCount], priProfiles[RsCGShaderBuilder::FragmentShader], _src);
			return 0;
		}
#endif
		programCount++;
	}
	return this->combineAndLoad(programs, programCount);
}

CGprogram RsCGShaderBuilder::BuildShaderFromFile(const std::string &_file, const char* _main, RsCGShaderBuilder::CGShaderType _type)
{
	if (!boost::filesystem::is_regular_file(boost::filesystem::path(_file)))
	{
		return 0;
	}

	CGprogram aProgram;
	aProgram = cgCreateProgramFromFile(RsCGShaderBuilder::priContext, CG_SOURCE, _file.c_str(), priProfiles[_type], _main ,0);
	cgGLLoadProgram(aProgram);
	if(RsCGShaderBuilder::IsCGError()){
		VerboseCompileOutput(aProgram, priProfiles[_type], _file.c_str());
	}
    return aProgram;
}

CGprogram RsCGShaderBuilder::BuildShaderFromFile(const std::string &_vsfile, const char* _vsMain, const std::string &_fsfile, const char* _fsMain)
{
    CGprogram programs[2];

	if (!boost::filesystem::is_regular_file(boost::filesystem::path(_vsfile)) || !boost::filesystem::is_regular_file(boost::filesystem::path(_fsfile))){
		return 0;
	}

    int programCount = 0;
	if (_vsMain != 0 && priHasVertexShaderSuport){
		programs[programCount] = cgCreateProgramFromFile(
			RsCGShaderBuilder::priContext,
			CG_SOURCE,
			_vsfile.c_str(),
			priProfiles[RsCGShaderBuilder::VertexShader],
			_vsMain,
			0);
		if (RsCGShaderBuilder::IsCGError()){
#ifdef CG_DEBUG
			VerboseCompileOutput(programs[programCount], priProfiles[RsCGShaderBuilder::VertexShader], _vsfile.c_str());
#endif
			return 0;
		}
		programCount++;
	}
	if (_fsMain != 0 && priHasFragmentShaderSuport){
		programs[programCount] = cgCreateProgramFromFile(
			RsCGShaderBuilder::priContext,
			CG_SOURCE,
			_fsfile.c_str(),
			priProfiles[RsCGShaderBuilder::FragmentShader],
			_fsMain,
			0);
		if (RsCGShaderBuilder::IsCGError()){
#ifdef CG_DEBUG
			VerboseCompileOutput(programs[programCount], priProfiles[RsCGShaderBuilder::FragmentShader], _fsfile.c_str());
#endif
			return 0;
		}
		programCount++;
	}
	return this->combineAndLoad(programs, programCount);
}

CGprogram RsCGShaderBuilder::BuildShaderFromFile(const std::string &_file, const char* _vsMain, const char* _gsMain, const char* _fsMain) {
    CGprogram programs[3];

	if (!boost::filesystem::is_regular_file(boost::filesystem::path(_file))){
		return 0;
	}

    int programCount = 0;
	if (_vsMain != 0 && priHasVertexShaderSuport){
		programs[programCount] = cgCreateProgramFromFile(
			RsCGShaderBuilder::priContext,
			CG_SOURCE,
			_file.c_str(),
			priProfiles[RsCGShaderBuilder::VertexShader],
			_vsMain,
			0);
		if (RsCGShaderBuilder::IsCGError()){
#ifdef CG_DEBUG
			VerboseCompileOutput(programs[programCount], priProfiles[RsCGShaderBuilder::VertexShader], _file.c_str());
#endif
			return 0;
		}
		programCount++;
	}
	if (_gsMain != 0 && priHasGeometryShaderSuport){
		programs[programCount] = cgCreateProgramFromFile(
			RsCGShaderBuilder::priContext,
			CG_SOURCE,
			_file.c_str(),
			priProfiles[RsCGShaderBuilder::GeometryShader],
			_gsMain,
			0);
		if (RsCGShaderBuilder::IsCGError()){
#ifdef CG_DEBUG
			VerboseCompileOutput(programs[programCount], priProfiles[RsCGShaderBuilder::GeometryShader], _file.c_str());
#endif
			return 0;
		}
		programCount++;
	}
	if (_fsMain != 0 && priHasFragmentShaderSuport){
		programs[programCount] = cgCreateProgramFromFile(
			RsCGShaderBuilder::priContext,
			CG_SOURCE,
			_file.c_str(),
			priProfiles[RsCGShaderBuilder::FragmentShader],
			_fsMain,
			0);
		if (RsCGShaderBuilder::IsCGError()){
#ifdef CG_DEBUG
			VerboseCompileOutput(programs[programCount], priProfiles[RsCGShaderBuilder::FragmentShader], _file.c_str());
#endif
			return 0;
		}
		programCount++;
	}

	return this->combineAndLoad(programs, programCount);
}

CGprogram RsCGShaderBuilder::BuildShaderFromObjectFile(const std::string &_file, const char* _main, RsCGShaderBuilder::CGShaderType _type)
{
	if (!boost::filesystem::is_regular_file(boost::filesystem::path(_file)))
	{
		return 0;
	}

	CGprogram aProgram;
	aProgram = cgCreateProgramFromFile(this->priContext, CG_OBJECT, _file.c_str(), priProfiles[_type], _main ,0);
	cgGLLoadProgram(aProgram);
	if(RsCGShaderBuilder::IsCGError()){
		VerboseCompileOutput(aProgram, priProfiles[_type], _file.c_str());
	}
    return aProgram;
}

CGprogram RsCGShaderBuilder::BuildShaderFromObjectFile(const std::string &_vsFile, const char* _vsMain, const std::string &_gsFile, const char* _gsMain, const std::string &_fsFile, const char* _fsMain)
{
    CGprogram programs[3];

	if (!boost::filesystem::is_regular_file(boost::filesystem::path(_vsFile)) ||
			!boost::filesystem::is_regular_file(boost::filesystem::path(_gsFile)) ||
			!boost::filesystem::is_regular_file(boost::filesystem::path(_fsFile))){
		return 0;
	}

    int programCount = 0;
	if (_vsMain != 0 && priHasVertexShaderSuport){
		programs[programCount] = cgCreateProgramFromFile(
			RsCGShaderBuilder::priContext,
			CG_OBJECT,
			_vsFile.c_str(),
			priProfiles[RsCGShaderBuilder::VertexShader],
			_vsMain,
			0);
		if (RsCGShaderBuilder::IsCGError()){
#ifdef CG_DEBUG
			VerboseCompileOutput(programs[programCount], priProfiles[RsCGShaderBuilder::VertexShader], _vsFile.c_str());
#endif
			return 0;
		}
		programCount++;
	}
	if (_gsMain != 0 && priHasGeometryShaderSuport){
		programs[programCount] = cgCreateProgramFromFile(
			RsCGShaderBuilder::priContext,
			CG_OBJECT,
			_gsFile.c_str(),
			priProfiles[RsCGShaderBuilder::GeometryShader],
			_gsMain,
			0);
		if (RsCGShaderBuilder::IsCGError()){
#ifdef CG_DEBUG
			VerboseCompileOutput(programs[programCount], priProfiles[RsCGShaderBuilder::GeometryShader], _gsFile.c_str());
#endif
			return 0;
		}
		programCount++;
	}
	if (_fsMain != 0 && priHasFragmentShaderSuport){
		programs[programCount] = cgCreateProgramFromFile(
			RsCGShaderBuilder::priContext,
			CG_OBJECT,
			_fsFile.c_str(),
			priProfiles[RsCGShaderBuilder::FragmentShader],
			_fsMain,
			0);
		if (RsCGShaderBuilder::IsCGError()){
#ifdef CG_DEBUG
			VerboseCompileOutput(programs[programCount], priProfiles[RsCGShaderBuilder::FragmentShader], _fsFile.c_str());
#endif
			return 0;
		}
		programCount++;
	}

	return this->combineAndLoad(programs, programCount);
}

bool RsCGShaderBuilder::priPrintInfoLog(GLhandleARB _handle, const char* _shader) {
    GLint aInfoLogLength = 0;
    GLint aCharWritten = 0;
    GLchar* aInfoLog;

    glGetObjectParameterivARB(_handle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &aInfoLogLength);

    if (aInfoLogLength > 1) {
        aInfoLog = new GLchar[aInfoLogLength];
        aInfoLog[aInfoLogLength] = 0;

        if (aInfoLog == 0) {
            std::cerr << "Can not allocate shader info log for " << _shader << "\n";
            return false;
        }

        glGetInfoLogARB(_handle, aInfoLogLength, &aCharWritten, aInfoLog);
        std::cerr << aInfoLogLength << "#" << _shader << ": Shader info log: " << aInfoLog << "\nEnd of log\n\n";

        delete [] aInfoLog;
        return false;
    }

    return true;
}

char* RsCGShaderBuilder::priReadShaderSource(const char *_filename) {
    std::ifstream aIFS(_filename);
    if (!aIFS.is_open()) {
        std::cerr << "ERROR: Can't open file: " << _filename << std::endl;
        return 0;
    }

    aIFS.seekg(0, std::ifstream::end);
    unsigned aFileSize = aIFS.tellg();
    aIFS.seekg(0, std::ifstream::beg);
    char* aShaderSource = new char[aFileSize];
    aIFS.read(aShaderSource, aFileSize);
    aIFS.close();

    return aShaderSource;
}

void RsCGShaderBuilder::EnableShader(CGprogram _shader)
{
	int domains = cgGetNumProgramDomains(_shader);
	for (int i=0; i < domains; i++){
    	cgGLEnableProfile(cgGetProgramDomainProfile(_shader, i));
	}
	cgGLBindProgram(_shader);
}

void RsCGShaderBuilder::DisableShader(CGprogram _shader)
{
	int domains = cgGetNumProgramDomains(_shader);
	for (int i=0; i < domains; i++){
    	cgGLDisableProfile(cgGetProgramDomainProfile(_shader, i));
    	cgGLUnbindProgram(cgGetProgramDomainProfile(_shader, i));
	}
}

const char* RsCGShaderBuilder::IsCGError()
{
	CGerror error = cgGetError();
	if (error != CG_NO_ERROR){
		return cgGetErrorString(error);
	}
	else {
		return 0;
	}
}

void RsCGShaderBuilder::VerboseCompileOutput(CGprogram _shader, CGprofile _profile, const char* _data)
{
	std::cerr << "===============> CG Error-Message <==============================" << std::endl;
	std::cerr << cgGetLastListing(priContext);
	std::cerr << "=================================================================" << std::endl;
	if (cgIsProgram(_shader) && cgIsProgramCompiled(_shader)){
		std::cerr << "===============> LAST LISTING (compiled) <=======================" << std::endl;
		std::cerr << cgGetProgramString(_shader, CG_COMPILED_PROGRAM) << std::endl;
		std::cerr << "=================================================================" << std::endl;
	}
	exit(0);
}

void RsCGShaderBuilder::PrintShadingLanguageString()
{
	std::cerr << "This card's supported shading language version: " <<  RsCGShaderBuilder::priLanguageVersion << std::endl;
}

CGprogram RsCGShaderBuilder::combineAndLoad(CGprogram* _programs, int _pCount)
{
    // combine shader programs from different domains into a single program
    // this is legal only, and only if there is more than one shader program

	CGprogram combined;
	CGerror error = CG_NO_ERROR;

    if (_pCount>1){
    	combined = cgCombinePrograms(_pCount, _programs);
    	error = cgGetError();

    	if (error != CG_NO_ERROR){
    		std::cerr << "CG Combine Programs error:" << std::endl;
    		std::cerr << cgGetErrorString(error) << std::endl;
    		return 0;
    	}
        for (int i =0; i<_pCount; ++i){
        	cgDestroyProgram(_programs[i]);
        }
    }
    else if (_pCount == 1){
    	combined = _programs[0];
    }
    else {
    	return 0;
    }
	cgGLLoadProgram(combined);
    return combined;

}
