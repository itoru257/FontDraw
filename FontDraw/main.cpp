//
//  main.cpp
//  FontDraw
//
//  Created by toru ito on 2016/12/3.
//  Copyright © 2016 toru ito. All rights reserved.
//

#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <string>

#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "ShaderProgram.hpp"
#include "ShapeData.hpp"
#include "TextureData.hpp"

#include "FontData.hpp"

int main(int argc, const char * argv[])
{
    GLFWwindow* window;
    
    // Initialize the library
    if (!glfwInit())
        return -1;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow( 300, 300, "FontDraw", NULL, NULL );
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    // Make the window's context current
    glfwMakeContextCurrent(window);
    
    //Font
    FontData font;
    if( !font.InitFromFile( "/Library/Fonts/Arial Unicode.ttf" ) )
        return -1;
    
    //texture
    Eigen::Matrix< unsigned char, Eigen::Dynamic, Eigen::Dynamic > image;
    if( !font.CreateBitmap( L"アイウエオ", 50, true, image ))
        return -1;
    
    ShaderProgram TexProgram;
    if( !TexProgram.InitProgramTexture( image, Eigen::Vector3f( 0.0f, - 0.25f, 0.0f ) ) )
        return -1;
    
    TextureData texture;
    texture.InitDraw( (int)image.cols(), (int)image.rows() );
    
    //outline
    std::vector< std::vector< std::vector< Eigen::Vector2d > > > OutlinePointList;
    if( !font.CreateOutlinePoint( L"あいうえお", &OutlinePointList ) )
        return -1;
    
    ShaderProgram outlineProgram;
    if( !outlineProgram.InitProgram() )
        return -1;
    
    ShapeData outline;
    outline.SetPolyline( &OutlinePointList );

    outlineProgram.SetColor( 0.0f, 1.0f, 0.0f );
    outlineProgram.SetDataMatrix( &outline, Eigen::Vector3f( 0.0f, 0.25f, 0.0f ) );
    
    outline.InitDraw();
    
    font.Destroy();
    
    // Loop until the user closes the window
    while( !glfwWindowShouldClose( window ) ) {
        
        // Render
        glClear( GL_COLOR_BUFFER_BIT );
        
        TexProgram.UseProgram();
        texture.Draw();
        
        outlineProgram.UseProgram();
        outline.Draw();
        
        glBindVertexArray( 0 );
        
        // Swap front and back buffers
        glfwSwapBuffers( window );
        
        // Poll for and process events
        glfwPollEvents();
        
    }
    
    texture.DeleteDraw();
    TexProgram.DeleteProgram();
    
    outline.DeleteDraw();
    outlineProgram.DeleteProgram();
    
    glfwTerminate();
    
    return 0;
}
