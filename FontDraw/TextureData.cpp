/*************************************************
 * Copyright (c) 2017 Toru Ito
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php
 *************************************************/

#include "TextureData.hpp"

TextureData::TextureData()
:m_VertexArrayObject(0)
,m_VertexBufferObject(0)
,m_TextureBufferObject(0)
{
}

TextureData::~TextureData()
{
}

void TextureData::InitDraw( int w, int h )
{
    float width = (float)w;
    float height  = (float)h;
    
    GLfloat positionData[] = {
        0.0f,  0.0f,   0.0f,
        0.0f,  height, 0.0f,
        width, 0.0f,   0.0f,
        
        width, 0.0f,   0.0f,
        0.0f,  height, 0.0f,
        width, height, 0.0f,
    };
    
    GLfloat textureData[] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
    };

    glGenBuffers( 1, &m_VertexBufferObject );
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), positionData, GL_STATIC_DRAW );
    
    glGenBuffers( 1, &m_TextureBufferObject );
    glBindBuffer(GL_ARRAY_BUFFER, m_TextureBufferObject);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), textureData, GL_STATIC_DRAW );
    

    glGenVertexArrays( 1, &m_VertexArrayObject );
    glBindVertexArray( m_VertexArrayObject );
    
    glEnableVertexAttribArray(0);  // Vertex position
    glEnableVertexAttribArray(1);  // Vertex texture
    
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferObject );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );
    
    glBindBuffer(GL_ARRAY_BUFFER, m_TextureBufferObject);
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );
    glBindVertexArray(0);
    
}

void TextureData::Draw()
{
    glBindVertexArray( m_VertexArrayObject );
    glDrawArrays( GL_TRIANGLES, 0, 3 );
    glDrawArrays( GL_TRIANGLES, 3, 3 );
}

void TextureData::DeleteDraw()
{
    if( m_VertexBufferObject != 0 ) {
        glDeleteBuffers( 1, &m_VertexBufferObject );
        m_VertexBufferObject = 0;
    }
    
    if( m_TextureBufferObject != 0 ) {
        glDeleteBuffers( 1, &m_TextureBufferObject );
        m_TextureBufferObject = 0;
    }
    
    if( m_VertexArrayObject != 0 ) {
        glDeleteVertexArrays( 1, &m_VertexArrayObject );
        m_VertexArrayObject = 0;
    }
}

GLuint TextureData::CreateTexture( Eigen::Matrix< unsigned char, Eigen::Dynamic, Eigen::Dynamic > image, double r, double g, double b )
{
    GLuint texID;
    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &texID );
    glBindTexture( GL_TEXTURE_2D, texID );
    
    unsigned char *data;
    
    if( ( data = (unsigned char *)calloc( image.rows() * image.cols() * 4, sizeof(unsigned char))) == NULL )
        return 0;
    
    long i, j, p;
    double color;
    
    for( j = 0; j < image.cols(); ++j ) {
        for( i = 0; i < image.rows(); ++i ) {
            
            p = ( j + i * image.cols() ) * 4;
            
            color = (double)image( i, j );
            
            data[p+0] = (unsigned char)( color * r );
            data[p+1] = (unsigned char)( color * g );
            data[p+2] = (unsigned char)( color * b );
            data[p+3] = image( i, j );
            
        }
    }
    
    glTexImage2D(
                 GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 (GLsizei)image.cols(),
                 (GLsizei)image.rows(),
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 data
                 );
    
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    
    free( data );
    
    return texID;
}

