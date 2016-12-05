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
    bool DeleteProgram();
    
    void SetColor( float r, float g, float b );
    void SetDataMatrix( ShapeData *data );
    void SetDataMatrix( std::vector< ShapeData > *datas );
    
    GLuint m_Program;
    std::vector< std::string > m_Log;
    
private:
    bool CreateProgram();
    bool AddShader( std::string code, GLenum shaderType );
    bool LinkProgram();
    bool UseProgram();
    
};

#endif /* ShaderProgram_hpp */
