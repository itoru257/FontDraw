/*************************************************
 * Copyright (c) 2017 Toru Ito
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php
 *************************************************/

#ifndef TextureData_hpp
#define TextureData_hpp

#include <stdio.h>
#include <vector>
#include <Eigen/Core>
#include <Eigen/Geometry>

#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>

class TextureData
{
public:
    TextureData();
    ~TextureData();
    
    void InitDraw( int w, int h );
    void Draw();
    void DeleteDraw();
    
    static GLuint CreateTexture( Eigen::Matrix< unsigned char, Eigen::Dynamic, Eigen::Dynamic > image, double r, double g, double b );
    
private:
    GLuint m_VertexBufferObject;
    GLuint m_TextureBufferObject;
    GLuint m_VertexArrayObject;
};

#endif /* TextureData_hpp */
