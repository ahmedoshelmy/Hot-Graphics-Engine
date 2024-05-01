#include "shader.hpp"

#include <cassert>
#include <iostream>
#include <fstream>
#include <string>

//Forward definition for error checking functions
std::string checkForShaderCompilationErrors(GLuint shader);
std::string checkForLinkingErrors(GLuint program);

bool our::ShaderProgram::attach(const std::string &filename, GLenum type, std::string includeIndentifier) const {
    // Here, we open the file and read a string from it containing the GLSL code of our shader
    std::ifstream file(filename);
    if(!file){
        std::cerr << "ERROR: Couldn't open shader file: " << filename << std::endl;
        return false;
    }
    std::string sourceString = "";
    // std::string sourceString = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    std::string lineBuffer;
    while(std::getline(file, lineBuffer)){
        if (lineBuffer.find(includeIndentifier) != lineBuffer.npos) {
            lineBuffer.erase(0, includeIndentifier.size()); //  remove #include to get the file name

            std::string pathWithoutFileName = filename.substr(0, filename.find_last_of("/"));
            while(lineBuffer.find("../") != lineBuffer.npos) {
                lineBuffer.erase(0, 3); // remove ../
                pathWithoutFileName = pathWithoutFileName.substr(0, pathWithoutFileName.find_last_of("/"));
            }
            pathWithoutFileName += "/";
            lineBuffer.insert(0, pathWithoutFileName); // insert the path of the folder

            std::ifstream includedFile(lineBuffer);
            std::string includedSourceString = std::string(std::istreambuf_iterator<char>(includedFile), std::istreambuf_iterator<char>());
            sourceString += includedSourceString + "\n";
        } else {
            sourceString += lineBuffer + "\n";
        }
    }
    // ==================== allow includes =============

    const char* sourceCStr = sourceString.c_str();
    file.close();
    //TODO: Complete this function
    //Note: The function "checkForShaderCompilationErrors" checks if there is
    // an error in the given shader. You should use it to check if there is a
    // compilation error and print it so that you can know what is wrong with
    // the shader. The returned string will be empty if there is no errors.

    // Creating and compiling the shader
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &sourceCStr, nullptr);
    glCompileShader(shader);

    // Checking compilation status
    std::string compilationError = checkForShaderCompilationErrors(shader);
    if(!compilationError.empty()){
        std::cout<<"Shader Compile Error: " + compilationError;
        return false;
    }

    glAttachShader(program, shader);
    // Shader is not needed anymore, since it's been attached to the program
    glDeleteShader(shader);

    //We return true if the compilation succeeded
    return true;
}



bool our::ShaderProgram::link() const {
    //TODO: Complete this function
    //Note: The function "checkForLinkingErrors" checks if there is
    // an error in the given program. You should use it to check if there is a
    // linking error and print it so that you can know what is wrong with the
    // program. The returned string will be empty if there is no errors.
    glLinkProgram(program);
    std::string linkError = checkForLinkingErrors(program);
    if(!linkError.empty()){
        std::cout<<"Link Error: " + linkError ;
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////
// Function to check for compilation and linking error in shaders //
////////////////////////////////////////////////////////////////////

std::string checkForShaderCompilationErrors(GLuint shader){
     //Check and return any error in the compilation process
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char *logStr = new char[length];
        glGetShaderInfoLog(shader, length, nullptr, logStr);
        std::string errorLog(logStr);
        delete[] logStr;
        return errorLog;
    }
    return std::string();
}

std::string checkForLinkingErrors(GLuint program){
     //Check and return any error in the linking process
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
        GLint length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char *logStr = new char[length];
        glGetProgramInfoLog(program, length, nullptr, logStr);
        std::string error(logStr);
        delete[] logStr;
        return error;
    }
    return std::string();
}