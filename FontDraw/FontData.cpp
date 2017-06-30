/*************************************************
 * Copyright (c) 2017 Toru Ito
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php
 *************************************************/

#include "FontData.hpp"
#include <iostream>

#include "OutlineFunction.hpp"

#if defined(_WIN32)
#include "afxwin.h"
#include "afxcmn.h"
#endif

FontData::FontData()
	:m_FontBufferSize( 0 )
{
	m_FontBuffer = NULL;
}

FontData::~FontData()
{
}

bool FontData::InitFromFile( std::string fname )
{
    if( !InitLibrary() )
        return false;
    
    if( !InitFace( fname ) )
        return false;
    
    return true;
}

unsigned long FontData::FontReadMemory( FT_Stream stream, unsigned long offset, unsigned char *buffer, unsigned long count )
{
    if( count <= 0 ) return 0;

	unsigned long i, pos;
    unsigned char *data;
	
	data = static_cast<unsigned char *>( stream->descriptor.pointer );

	for( i = 0, pos = offset; i < count; ++i, ++pos ) {
		buffer[i] = data[pos];
	}

    return count;
}

#if defined(_WIN32)

bool FontData::InitFromWindowsFont( std::string FontName )
{
	if( !InitLibrary() )
		return false;

	bool result = false;
	auto commonDC = CreateCompatibleDC( nullptr );

	HFONT font = CreateFontA( 
		-12,
		0,
		0, 
		0, 
		FW_REGULAR, 
		FALSE, 
		FALSE,
		FALSE, 
		DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_DONTCARE,
		FontName.c_str()
		);

    SelectObject( commonDC, font );

	DWORD fontDataSize;

    fontDataSize = GetFontData( commonDC, 0x66637474, 0, nullptr, 0 );

	if( ( fontDataSize > 0 ) && ( fontDataSize != GDI_ERROR ) ) {

		m_FontBuffer     = new unsigned char[fontDataSize];
		m_FontBufferSize = fontDataSize;

		if( GetFontData( commonDC, 0x66637474, 0, m_FontBuffer, fontDataSize ) != GDI_ERROR ) {
			result = true;
		}

	} else {

		fontDataSize = GetFontData( commonDC, 0, 0, nullptr, 0 );

		if( ( fontDataSize > 0 ) && ( fontDataSize != GDI_ERROR ) ) {

			m_FontBuffer     = new unsigned char[fontDataSize];
			m_FontBufferSize = fontDataSize;

			if( GetFontData( commonDC, 0, 0, m_FontBuffer, fontDataSize ) != GDI_ERROR ) {
				result = true;
			}

		} 

	}

    DeleteObject( font );
    DeleteDC( commonDC );

	if( !result )
		return false;

	FT_Error error;
	result = true;

	if( error = FT_New_Memory_Face( m_Library, m_FontBuffer, m_FontBufferSize, 0, &m_Face ) ) {
		result = false;
	}

	/*
	//“Ç‚Ýž‚Ý
	memset(&m_Stream, 0, sizeof(m_Stream));
	FT_StreamRec *stream = &m_Stream;

	stream->base = 0;
	stream->pos = 0;
	stream->size = fontDataSize;
	stream->descriptor.pointer = m_FontBuffer;
	stream->read  = FontReadMemory;
	stream->close = nullptr;

	int index = 0;

	while( true ) {

		FT_Open_Args args;
		memset(&args, 0, sizeof(args));

		args.flags = FT_OPEN_STREAM;
		args.stream = stream;
		args.driver = 0;
		args.num_params = 0;
		args.params = NULL;

		if( error = FT_Open_Face( m_Library, &args, index, &m_Face ) ) {
			result = false;
			break;
		}

		index++;

		break;
	}
	*/

	return result;
}

#endif

void FontData::Destroy()
{
    DestroyFace();
    DestroyLibrary();

	if( m_FontBuffer != NULL ) {
		delete m_FontBuffer;
		m_FontBuffer     = NULL;
		m_FontBufferSize = 0;
	}
}

bool FontData::InitLibrary()
{
    FT_Error error = FT_Init_FreeType( &m_Library );
    
    if( error )
        return false;
    
    return true;
}

void FontData::DestroyLibrary()
{
    if( m_Library ) {
        FT_Done_FreeType( m_Library );
    }
}

bool FontData::InitFace( std::string fname )
{
    FT_Error  error;
    
    error = FT_New_Face( m_Library,
                        fname.c_str(),
                        0,
                        &m_Face );

	//FT_Open_Args args;
	//args.flags = FT_OPEN_PATHNAME;
	//args.pathname = (FT_String*)fname.c_str();

	//error = FT_Open_Face( m_Library, &args, 0, &m_Face );

    if ( error == FT_Err_Unknown_File_Format ) {
        return false;
    } else if ( error ) {
        return false;
    }
    
    return true;
}

void FontData::DestroyFace()
{
    if( m_Face ) {
        FT_Done_Face( m_Face );
    }
}

void FontData::PrintBitmap( Eigen::Matrix< unsigned char, Eigen::Dynamic, Eigen::Dynamic >& image )
{
    unsigned int i, j, data;
    
    std::cout << "w = " << image.cols() << ", h = " << image.rows() << std::endl;;
    
    for( i = 0; i < image.rows(); ++i ) {
        for( j = 0; j < image.cols(); ++j ) {
        
            data = image( image.rows() - i - 1, j );
            
            if( data == 0 ) {
                std::cout << "-";
            } else if( data > 128 ) {
                std::cout << "+";
            } else {
                std::cout << "*";
            }
            
        }
        
        std::cout << std::endl;
    }
}

bool FontData::CreateBitmap( const wchar_t *text, int pixel_size, bool base_line_flg, Eigen::Matrix< unsigned char, Eigen::Dynamic, Eigen::Dynamic >& image )
{
    FT_Set_Pixel_Sizes( m_Face, 0, pixel_size );
    
    long x, y, ix, iy, w, h, y_min, y_max;
    long i, j;
    long buffer_size_x, buffer_size_y;
    long origin_x, origin_y;
    long bounds[4];
    
    buffer_size_x = pixel_size * ( static_cast< unsigned char >( wcslen( text ) ) + 2 );
    buffer_size_y = static_cast< long >( static_cast< double >( pixel_size ) * 1.35 );
    
    Eigen::Matrix< unsigned char, Eigen::Dynamic, Eigen::Dynamic > buffer;
    buffer = Eigen::Matrix< unsigned char, Eigen::Dynamic, Eigen::Dynamic >::Zero( buffer_size_x, buffer_size_y );
    
    bounds[0] = buffer_size_x;
    bounds[1] = 0;
    bounds[2] = buffer_size_y;
    bounds[3] = 0;
    
    origin_x = 0;
    origin_y = 0;
    
    for( const wchar_t *p = text; *p != '\0'; ++p ) {
        
        FT_Error error;
        FT_UInt index;
        
        index = FT_Get_Char_Index( m_Face, *p );
        error = FT_Load_Glyph( m_Face, index, FT_LOAD_RENDER );
        
        if( error )
            return false;
        
        FT_GlyphSlot slot = m_Face->glyph;
        FT_Bitmap *bitmap = &slot->bitmap;
        
        w = bitmap->width;
        h = bitmap->rows;
        
        x = origin_x + slot->bitmap_left;
        y = origin_y + ( pixel_size - slot->bitmap_top );
        
        y_min = buffer_size_y - ( y + h );
        y_max = buffer_size_y - y;
        
        bounds[0] = ( bounds[0] < x )     ? bounds[0] : x;
        bounds[1] = ( bounds[1] > x + w ) ? bounds[1] : x + w;
        bounds[2] = ( bounds[2] < y_min ) ? bounds[2] : y_min;
        bounds[3] = ( bounds[3] > y_max ) ? bounds[3] : y_max;
        
        for( i = 0, ix = x; i < w; ++i, ++ix ) {
            for( j = 0, iy = y; j < h; ++j, ++iy ) {
                
                if( ix >= buffer_size_x || ix < 0 )
                    continue;
                
                if( iy > buffer_size_y || iy <= 0 )
                    continue;
                
                buffer( ix, buffer_size_y - iy ) = bitmap->buffer[j * bitmap->width + i];
                
            }
        }
        
        origin_x += slot->advance.x / 64;
        origin_y += slot->advance.y / 64;
        
    }
    
    bounds[0] -= 1;
    bounds[1] += 1;
    
    bounds[0] = ( bounds[0] > 0 ) ? bounds[0] : 0;
    bounds[1] = ( bounds[1] < buffer_size_x ) ? bounds[1] : buffer_size_x;

    if( base_line_flg ) {
        
        bounds[2] = 0;
        bounds[3] = buffer_size_y;
        
    } else {
        
        bounds[2] -= 0;
        bounds[3] += 2;
        
        bounds[2] = ( bounds[2] > 0 ) ? bounds[2] : 0;
        bounds[3] = ( bounds[3] < buffer_size_y ) ? bounds[3] : buffer_size_y;
        
    }
    
    long image_size_x, image_size_y;
    
    image_size_x = bounds[1] - bounds[0];
    image_size_y = bounds[3] - bounds[2];
    
    image = Eigen::Matrix< unsigned char, Eigen::Dynamic, Eigen::Dynamic >::Zero( image_size_y, image_size_x );
    
    for( i = 0, ix = bounds[0]; i < image_size_x; ++i, ++ix ) {
        for( j = 0, iy = bounds[2]; j < image_size_y; ++j, ++iy ) {
            image( j, i ) = buffer( ix, iy );
        }
    }
    
    return true;
}

void FontData::PrintOutlinePoint( std::vector< std::vector< std::vector< Eigen::Vector2d > > > *pOutlinePointList )
{
    for( auto group : *pOutlinePointList ) {
        for( auto list : group ) {
            for( auto point : list ) {
                std::cout << point[0] << ", " << point[1] << std::endl;
            }
        
            std::cout << std::endl;
        
        }
    }
}

bool FontData::CreateOutlinePoint( const wchar_t *text, std::vector< std::vector< std::vector< Eigen::Vector2d > > > *pOutlinePointList )
{
    pOutlinePointList->clear();
    
    OutlineFunction OutlineFunc;
    FT_Vector origin;
    
    origin.x = 0;
    origin.y = 0;
    
    for( const wchar_t *p = text; *p != '\0'; ++p ) {
        
        OutlineFunc.DeleteList();
        
        FT_Error error;
        FT_UInt index;
        
        index = FT_Get_Char_Index( m_Face, *p );
        error = FT_Load_Glyph( m_Face, index, FT_LOAD_NO_SCALE | FT_LOAD_NO_BITMAP );
    
        if( error )
            return false;
        
        FT_GlyphSlot slot = m_Face->glyph;
        FT_Outline outline = slot->outline;
        
        if( slot->format != FT_GLYPH_FORMAT_OUTLINE )
            return false;
        
        error = FT_Outline_Check( &outline );
        
        if( error )
            return false;
        
        FT_Outline_Translate( &outline, origin.x, origin.y );
        FT_Outline_Decompose( &outline, &OutlineFunc.m_OutlineFuncs, (void*)&OutlineFunc );
        
        origin.x += m_Face->glyph->advance.x;
        origin.y += m_Face->glyph->advance.y;
        
        OutlineFunc.CreateOutlinePoint();
        pOutlinePointList->push_back( OutlineFunc.m_PointGroupList );
        
    }
    
    return true;
}
