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

bool RsCGShaderBuilder::priInitialized = false;
std::string RsCGShaderBuilder::priLanguageVersion = std::string();
CGbool RsCGShaderBuilder::priHasVertexShaderSuport = false;
CGbool RsCGShaderBuilder::priHasGeometryShaderSuport = false;
CGbool RsCGShaderBuilder::priHasFragmentShaderSuport = false;
CGcontext RsCGShaderBuilder::priContext = 0;
CGprofile RsCGShaderBuilder::priProfiles[3];



void RsCGShaderBuilder::priInit()
{
	//TODO complete
	if (!RsCGShaderBuilder::priInitialized){
		priContext = cgCreateContext();
#ifdef CG_DEBUG
		cgGLSetDebugMode(CG_TRUE);
#endif
		if (!cgIsContext(priContext)){
			std::cerr << RsCGShaderBuilder::IsCGError() << std::endl;
			std::cerr << "CG: Failed to get a valid cg context!" << std::endl;
		}

		priProfiles[RsCGShaderBuilder::VertexShader] = cgGLGetLatestProfile(CG_GL_VERTEX);
		priProfiles[RsCGShaderBuilder::GeometryShader] = cgGLGetLatestProfile(CG_GL_GEOMETRY);
		priProfiles[RsCGShaderBuilder::FragmentShader] = cgGLGetLatestProfile(CG_GL_FRAGMENT);

		if (cgIsProfileSupported(priProfiles[RsCGShaderBuilder::VertexShader])){
			cgGLSetOptimalOptions(priProfiles[RsCGShaderBuilder::VertexShader]);
			RsCGShaderBuilder::priHasVertexShaderSuport = CG_TRUE;
		}
		if (cgIsProfileSupported(priProfiles[RsCGShaderBuilder::GeometryShader])){
			cgGLSetOptimalOptions(priProfiles[RsCGShaderBuilder::GeometryShader]);
			RsCGShaderBuilder::priHasGeometryShaderSuport = CG_TRUE;
		}
		if (cgIsProfileSupported(priProfiles[RsCGShaderBuilder::FragmentShader])){
			cgGLSetOptimalOptions(priProfiles[RsCGShaderBuilder::FragmentShader]);
			RsCGShaderBuilder::priHasFragmentShaderSuport = CG_TRUE;
		}

		RsCGShaderBuilder::priLanguageVersion = std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

		RsCGShaderBuilder::priInitialized = true;
	}
	// -----------------------------------------------

}

unsigned RsCGShaderBuilder::BuildShader(const char* _src, CGShaderType _type) {
	//TODO remove?
	return 0;

//    const char * aSrc = _src;
//
//    GLint aShaderComp;
//    GLint aLinkedShader;
//
//    GLuint aShader = glCreateShader(_type);
//    glShaderSource(aShader, 1, &aSrc, 0);
//
//    glCompileShader(aShader);
//    glGetObjectParameterivARB(aShader, GL_OBJECT_COMPILE_STATUS_ARB, &aShaderComp);
//    if (!priPrintInfoLog(aShader, "Shader")) return 0;
//
//    GLuint aShaderProgram = glCreateProgram();
//    glAttachShader(aShaderProgram, aShader);
//    glLinkProgram(aShaderProgram);
//
//    glGetObjectParameterivARB(aShaderProgram, GL_OBJECT_LINK_STATUS_ARB, &aLinkedShader);
//    if(!priPrintInfoLog(aShaderProgram, "Linker")) return 0;
//
//    return aShaderProgram;
}

unsigned RsCGShaderBuilder::BuildShader(const char* _vs, const char* _fs) {
	//TODO remove?
	return 0;
//    const char * aVS = _vs;
//    const char * aFS = _vs;
//
//    GLint aVShaderComp;
//    GLint aFShaderComp;
//    GLint aLinkedShader;
//
//    GLuint aVertexShader = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(aVertexShader, 1, &aVS, 0);
//
//    GLuint aFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(aFragmentShader, 1, &aFS, NULL);
//
//    glCompileShader(aVertexShader);
//    glGetObjectParameterivARB(aVertexShader, GL_OBJECT_COMPILE_STATUS_ARB, &aVShaderComp);
//    if (!priPrintInfoLog(aVertexShader, "VertexShader")) return 0;
//
//    glCompileShader(aFragmentShader);
//    glGetObjectParameterivARB(aFragmentShader, GL_OBJECT_COMPILE_STATUS_ARB, &aFShaderComp);
//    if (!priPrintInfoLog(aFragmentShader, "FragmentShader")) return 0;
//
//    GLuint aShaderProgram = glCreateProgram();
//    glAttachShader(aShaderProgram, aVertexShader);
//    glAttachShader(aShaderProgram, aFragmentShader);
//    glLinkProgram(aShaderProgram);
//
//    glGetObjectParameterivARB(aShaderProgram, GL_OBJECT_LINK_STATUS_ARB, &aLinkedShader);
//    if(!priPrintInfoLog(aShaderProgram, "Linker")) return 0;
//
//    return aShaderProgram;
}

unsigned RsCGShaderBuilder::BuildShader(const char* _vs, const char* _gs, const char* _fs) {
	//TODO remove?
	return 0;

//    const char * aVS = _vs;
//    const char * aGS = _gs;
//    const char * aFS = _vs;
//
//    GLint aVShaderComp;
//    GLint aGShaderComp;
//    GLint aFShaderComp;
//    GLint aLinkedShader;
//
//    GLuint aVertexShader = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(aVertexShader, 1, &aVS, 0);
//
//    GLuint aGeometryShader = glCreateShader(0x8DD9/*GL_GEOMETRY_SHADER_ARB*/);
//    glShaderSource(aGeometryShader, 1, &aGS, 0);
//
//    GLuint aFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(aFragmentShader, 1, &aFS, 0);
//
//    glCompileShader(aVertexShader);
//    glGetObjectParameterivARB(aVertexShader, GL_OBJECT_COMPILE_STATUS_ARB, &aVShaderComp);
//    if (!priPrintInfoLog(aVertexShader, "VertexShader")) return 0;
//
//    glCompileShader(aGeometryShader);
//    glGetObjectParameterivARB(aGeometryShader, GL_OBJECT_COMPILE_STATUS_ARB, &aGShaderComp);
//    if (!priPrintInfoLog(aGeometryShader, "GeometryShader")) return 0;
//
//    glCompileShader(aFragmentShader);
//    glGetObjectParameterivARB(aFragmentShader, GL_OBJECT_COMPILE_STATUS_ARB, &aFShaderComp);
//    if (!priPrintInfoLog(aFragmentShader, "FragmentShader")) return 0;
//
//    GLuint aShaderProgram = glCreateProgram();
//    glAttachShader(aShaderProgram, aVertexShader);
//    glAttachShader(aShaderProgram, aGeometryShader);
//    glAttachShader(aShaderProgram, aFragmentShader);
//    glLinkProgram(aShaderProgram);
//
//    glGetObjectParameterivARB(aShaderProgram, GL_OBJECT_LINK_STATUS_ARB, &aLinkedShader);
//    if(!priPrintInfoLog(aShaderProgram, "Linker")) return 0;
//
//    return aShaderProgram;
}

CGprogram RsCGShaderBuilder::BuildShaderFromFile(const std::string &_file, const char* _main, RsCGShaderBuilder::CGShaderType _type)
{
	if (!RsCGShaderBuilder::priInitialized){
		RsCGShaderBuilder::priInit();
	}

	CGprogram aProgram;
	if (_main == 0){
		aProgram = cgCreateProgramFromFile(RsCGShaderBuilder::priContext, CG_SOURCE, _file.c_str(), priProfiles[_type], "main",0);
	}
	else {
		aProgram = cgCreateProgramFromFile(RsCGShaderBuilder::priContext, CG_SOURCE, _file.c_str(), priProfiles[_type], _main ,0);
	}
	cgGLLoadProgram(aProgram);
	if(RsCGShaderBuilder::IsCGError()){
		VerboseCompileOutput(aProgram, priProfiles[_type], _file);
	}
    return aProgram;
}

CGprogram RsCGShaderBuilder::BuildShaderFromFile(const std::string &_vsfile, const char* _vsMain, const std::string &_fsfile, const char* _fsMain)
{
	if (!RsCGShaderBuilder::priInitialized){
		RsCGShaderBuilder::priInit();
	}
    CGprogram combined;
    CGprogram programs[2];
    CGerror error = CG_NO_ERROR;

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
			VerboseCompileOutput(programs[programCount], priProfiles[RsCGShaderBuilder::VertexShader], _vsfile);
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
			VerboseCompileOutput(programs[programCount], priProfiles[RsCGShaderBuilder::FragmentShader], _fsfile);
#endif
			return 0;
		}
		programCount++;
	}

    // combine shader programs from different domains into a single program
    // this is legal only, and only if there is more than one shader program
    if (programCount>1){
    	combined = cgCombinePrograms(programCount, programs);
    	error = cgGetError();
    	if (error != CG_NO_ERROR){
    		std::cerr << "CG Combine Programs" << std::endl;
    		std::cerr << cgGetErrorString(error) << std::endl;
    		return 0;
    	}
        for (int i =0; i<programCount; ++i){
        	cgDestroyProgram(programs[i]);
        }
    }
    else if (programCount == 1){
    	combined = programs[0];
    }
    else {
    	return 0;
    }
	cgGLLoadProgram(combined);
    return combined;
}

CGprogram RsCGShaderBuilder::BuildShaderFromFile(const std::string &_file, const char* _vsMain, const char* _gsMain, const char* _fsMain) {
	if (!RsCGShaderBuilder::priInitialized){
		RsCGShaderBuilder::priInit();
	}
    CGprogram combined;
    CGprogram programs[3];
    CGerror error = CG_NO_ERROR;

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
			VerboseCompileOutput(programs[programCount], priProfiles[RsCGShaderBuilder::VertexShader], _file);
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
			VerboseCompileOutput(programs[programCount], priProfiles[RsCGShaderBuilder::GeometryShader], _file);
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
			VerboseCompileOutput(programs[programCount], priProfiles[RsCGShaderBuilder::FragmentShader], _file);
#endif
			return 0;
		}
		programCount++;
	}

    // combine shader programs from different domains into a single program
    // this is legal only, and only if there is more than one shader program
    if (programCount>1){
    	combined = cgCombinePrograms(programCount, programs);
    	error = cgGetError();
    	if (error != CG_NO_ERROR){
    		std::cerr << "CG Combine Programs" << std::endl;
    		std::cerr << cgGetErrorString(error) << std::endl;
    		return 0;
    	}
        for (int i =0; i<programCount; ++i){
        	cgDestroyProgram(programs[i]);
        }
    }
    else if (programCount == 1){
    	combined = programs[0];
    }
    else {
    	return 0;
    }
	cgGLLoadProgram(combined);
    return combined;
}

CGprogram RsCGShaderBuilder::BuildShaderFromFile(const std::string &_vsfile, const std::string &_gsfile, const std::string &_fsfile) {
	//TODO
	return 0;
//    const char* aVShaderSource = priReadShaderSource(_vsfile.c_str());
//    const char* aGShaderSource = priReadShaderSource(_gsfile.c_str());
//    const char* aFShaderSource = priReadShaderSource(_fsfile.c_str());
//    unsigned aHandle = NGLBuildShader(aVShaderSource, aGShaderSource, aFShaderSource);
//    delete [] aVShaderSource;
//    delete [] aGShaderSource;
//    delete [] aFShaderSource;
//
//    return aHandle;
}

RsCGShaderBuilder::RsCGShaderBuilder() {
}

RsCGShaderBuilder::~RsCGShaderBuilder() {
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

/**
 * @brief Checks whether or not a CGerror happened.
 * @return the error-string if a CGerror occurred - 0 otherwise.
 */
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

/**
 * @brief Send some more information to std::cerr when a compile error occured.
 * @param _shader The CGprogram for which the error occured
 * @param _profile The CGprofile for which the error occured
 * @param _code The source code of the erroneous program or, at least, the its filename.
 */
void RsCGShaderBuilder::VerboseCompileOutput(CGprogram _shader, CGprofile _profile, const std::string& _file)
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
