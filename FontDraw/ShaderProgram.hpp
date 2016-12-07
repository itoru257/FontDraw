/*************************************************
 * Copyright (c) 2016 Toru Ito
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php
 *************************************************/

#ifndef ShaderProgram_hpp
#define ShaderProgram_hpp

#include <stdio.h>
#include <string>
#include <vector>

#include <OpenGL/gl3.h>

#include "ShapeData.hpp"

class ShaderProgram
{
public:
    ShaderProgram();
    ~ShaderProgram();
    
    bool InitProgram();
    bool InitProgramFromFile( std::string fname_vertex, std::string fname_fragment );
    bool DeleteProgram();
    
    bool CreateProgram();
    bool AddShader( std::string code, GLenum shaderType );
    bool AddShaderFromFile( std::string file_name, GLenum shaderType );
    bool LinkProgram();
    bool UseProgram();
    
    void SetColor( float r, float g, float b );
    void SetDataMatrix( ShapeData *data );
    void SetDataMatrix( std::vector< ShapeData > *datas );
    
    void PrintLog();
    
    GLuint m_Program;
    std::vector< std::string > m_Log;
    
private:

    
};

#endif /* ShaderProgram_hpp */
