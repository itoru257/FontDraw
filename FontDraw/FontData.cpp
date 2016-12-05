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
