/*************************************************
 * Copyright (c) 2016 Toru Ito
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php
 *************************************************/

#include "FontData.hpp"
#include <iostream>

#include "OutlineFunction.hpp"

FontData::FontData()
{
    
}


FontData::~FontData()
{
    
}


bool FontData::Init( std::string fname )
{
    if( !InitLibrary() )
        return false;
    
    if( !InitFace( fname ) )
        return false;
    
    return true;
}


void FontData::Destroy()
{
    DestroyFace();
    DestroyLibrary();
}


bool FontData::InitLibrary()
{
    FT_Error error;
    
    error = FT_Init_FreeType( &m_Library );
    
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


bool FontData::CreateBitmap( const wchar_t *text, int pixel_size, Eigen::Matrix< unsigned char, Eigen::Dynamic, Eigen::Dynamic >& image )
{
    FT_Set_Pixel_Sizes( m_Face, 0, pixel_size );
    
    long x, y, ix, iy, w, h, y_min, y_max;
    long i, j;
    long buffer_size_x, buffer_size_y;
    long origin_x, origin_y;
    long bounds[4];
    
    buffer_size_x = pixel_size * ( static_cast< unsigned char >( wcslen( text ) ) + 2 ) + 2;
    buffer_size_y = pixel_size * 2 + 2;
    
    Eigen::Matrix< unsigned char, Eigen::Dynamic, Eigen::Dynamic > buffer;
    buffer = Eigen::Matrix< unsigned char, Eigen::Dynamic, Eigen::Dynamic >::Zero( buffer_size_x, buffer_size_y );
    
    bounds[0] = buffer_size_x;
    bounds[1] = 0;
    bounds[2] = buffer_size_y;
    bounds[3] = 0;
    
    origin_x = 2;
    origin_y = 2;
    
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
        y = origin_y + slot->bitmap_top;
        
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
                
                if( iy >= buffer_size_y || iy < 0 )
                    continue;
                
                buffer( ix, buffer_size_y - iy ) = bitmap->buffer[j * bitmap->width + i];
                
            }
        }
        
        origin_x += slot->advance.x / 64;
        origin_y += slot->advance.y / 64;
        
    }
    
    bounds[0] -= 1;
    bounds[1] += 1;
    
    bounds[2] -= 0;
    bounds[3] += 2;
    
    bounds[0] = ( bounds[0] > 0 ) ? bounds[0] : 0;
    bounds[1] = ( bounds[1] < buffer_size_x ) ? bounds[1] : buffer_size_x;
    bounds[2] = ( bounds[2] > 0 ) ? bounds[2] : 0;
    bounds[3] = ( bounds[3] < buffer_size_y ) ? bounds[3] : buffer_size_y;
    
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


bool FontData::CreateBitmap( const wchar_t *text, int pixel_size, double r, double g, double b, BitmapData *bitmap )
{
    Eigen::Matrix< unsigned char, Eigen::Dynamic, Eigen::Dynamic > data;
    if( !this->CreateBitmap( text, pixel_size, data ))
        return false;
    
    bitmap->width  = (int)data.cols();
    bitmap->height = (int)data.rows();
    
    if( ( bitmap->buffer = (unsigned char *)calloc( bitmap->width * bitmap->height * 4, sizeof(unsigned char) ) ) == NULL )
        return false;
    
    double color;
    long   i, j, p;
    
    for( i = 0; i < bitmap->height; ++i ) {
        for( j = 0; j < bitmap->width; ++j ) {
        
            p = ( j + i * bitmap->width ) * 4;
            
            color = (double)data( i, j );
            bitmap->buffer[p+0] = (unsigned char)( color * r );
            bitmap->buffer[p+1] = (unsigned char)( color * g );
            bitmap->buffer[p+2] = (unsigned char)( color * b );
            bitmap->buffer[p+3] = data( i, j );
            
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
