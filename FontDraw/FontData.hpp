/*************************************************
 * Copyright (c) 2016 Toru Ito
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php
 *************************************************/

#ifndef FontData_hpp
#define FontData_hpp

#include <stdio.h>
#include <string>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_BBOX_H

#include <Eigen/Core>
#include <Eigen/Geometry>

class FontData
{
 public:
    struct BitmapData
    {
        unsigned int    width;
        unsigned int    height;
        unsigned char*  buffer;
        
        BitmapData()
        {
            width  = 0;
            height = 0;
            buffer = NULL;
        }
        
        void Delete()
        {
            if( buffer != NULL ) {
                free( buffer );
                buffer = NULL;
            }
        }
    };
    
public:
    FontData();
    ~FontData();
    
    bool Init( std::string fname );
    void Destroy();
    
    bool CreateOutlinePoint( const wchar_t *text, std::vector< std::vector< std::vector< Eigen::Vector2d > > > *pOutlinePointList );
    
    static void PrintOutlinePoint( std::vector< std::vector< std::vector< Eigen::Vector2d > > > *pOutlinePointList );
    
    bool CreateBitmap( const wchar_t *text, int pixel_size, Eigen::Matrix< unsigned char, Eigen::Dynamic, Eigen::Dynamic >& image );
    
    bool CreateBitmap( const wchar_t *text, int pixel_size, double r, double g, double b, BitmapData *bitmap );
    
    static void PrintBitmap( Eigen::Matrix< unsigned char, Eigen::Dynamic, Eigen::Dynamic >& image );
    
private:
    FT_Library  m_Library;
    FT_Face     m_Face;
    
    bool InitLibrary();
    void DestroyLibrary();
    
    bool InitFace( std::string fname );
    void DestroyFace();
};

#endif /* FontData_hpp */
