/*************************************************
 * Copyright (c) 2017 Toru Ito
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
    FontData();
    ~FontData();
    
    bool InitFromFile( std::string fname );

#if defined(_WIN32)
    bool InitFromWindowsFont( std::string FontName );
#endif

    void Destroy();
    
    bool CreateOutlinePoint( const wchar_t *text, std::vector< std::vector< std::vector< Eigen::Vector2d > > > *pOutlinePointList );
    bool CreateBitmap( const wchar_t *text, int pixel_size, Eigen::Matrix< unsigned char, Eigen::Dynamic, Eigen::Dynamic >& image );
    
    static void PrintOutlinePoint( std::vector< std::vector< std::vector< Eigen::Vector2d > > > *pOutlinePointList );
    static void PrintBitmap( Eigen::Matrix< unsigned char, Eigen::Dynamic, Eigen::Dynamic >& image );
    
	static unsigned long FontReadMemory( FT_Stream stream, unsigned long offset, unsigned char *buffer, unsigned long count );

private:
    FT_Library    m_Library;
    FT_Face       m_Face;

	FT_StreamRec  m_Stream;
	unsigned char *m_FontBuffer;
	unsigned long m_FontBufferSize;

    bool InitLibrary();
    void DestroyLibrary();
    
    bool InitFace( std::string fname );
    void DestroyFace();
};

#endif /* FontData_hpp */
