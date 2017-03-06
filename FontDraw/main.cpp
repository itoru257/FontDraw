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
    if( !font.Init( "/Library/Fonts/Arial Unicode.ttf" ) )
        return -1;
    
    //texture
    Eigen::Matrix< unsigned char, Eigen::Dynamic, Eigen::Dynamic > image;
    if( !font.CreateBitmap( L"アイウエオ", 50, image ))
        return -1;
    
    ShaderProgram TexProgram;
    TexProgram.InitProgramTexture( image );
    
    TextureData texture;
    texture.InitDraw( (int)image.cols(), (int)image.rows() );
    
    font.Destroy();
    
    // Loop until the user closes the window
    while( !glfwWindowShouldClose( window ) ) {
        
        // Render
        glClear( GL_COLOR_BUFFER_BIT );
        
        texture.Draw();
        glBindVertexArray( 0 );
        
        // Swap front and back buffers
        glfwSwapBuffers( window );
        
        // Poll for and process events
        glfwPollEvents();
        
    }
    
    texture.DeleteDraw();
    TexProgram.DeleteProgram();
    
    glfwTerminate();
    
    /*
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
    
    ShaderProgram program;
    
    if( !program.InitProgram() )
        return -1;
    
    // Font
    FontData font;
    if( !font.Init( "/Library/Fonts/Arial Unicode.ttf" ) )
        return -1;
    
    std::vector< std::vector< std::vector< Eigen::Vector2d > > > OutlinePointList;
    if( !font.CreateOutlinePoint( L"アイウエオ", &OutlinePointList ) )
        return -1;
    
    ShapeData data;
    data.SetPolyline( &OutlinePointList );
    
    font.Destroy();
    
    //setting
    program.SetColor( 0.0f, 1.0f, 0.0f );
    program.SetDataMatrix( &data, Eigen::Vector3f::Zero() );

    data.InitDraw();
    
    // Loop until the user closes the window
    while( !glfwWindowShouldClose( window ) ) {
        
        // Render
        glClear( GL_COLOR_BUFFER_BIT );

        data.Draw();
        glBindVertexArray( 0 );
        
        // Swap front and back buffers
        glfwSwapBuffers( window );
        
        // Poll for and process events
        glfwPollEvents();
        
    }
    
    program.DeleteProgram();
    data.DeleteDraw();
    
    glfwTerminate();
    */
    
    return 0;
}
