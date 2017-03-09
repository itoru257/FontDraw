/*************************************************
 * Copyright (c) 2016 Toru Ito
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php
 *************************************************/

#include "OutlineFunction.hpp"

OutlineFunction::OutlineFunction()
{
    InitOutlineFuncs();
}


OutlineFunction::~OutlineFunction()
{
    
}

void OutlineFunction::InitOutlineFuncs()
{
    m_OutlineFuncs.move_to  = OutlineFunction::FunctionMoveTo;
    m_OutlineFuncs.line_to  = OutlineFunction::FunctionLineTo;
    m_OutlineFuncs.cubic_to = OutlineFunction::FunctionCubicTo;
    m_OutlineFuncs.conic_to = OutlineFunction::FunctionConicTo;
    
    m_OutlineFuncs.shift = 0;
    m_OutlineFuncs.delta = 0;
}


int OutlineFunction::FunctionMoveTo(const FT_Vector *to,
                             void *user)
{
    OutlineFunction *self = static_cast<OutlineFunction *>(user);
    OutlineData data;
    
    data.type = "MoveTo";
    data.point[0] = (double)to->x;
    data.point[1] = (double)to->y;
    
    self->m_OutlineList.push_back( data );
    
    return 0;
}


int OutlineFunction::FunctionLineTo(const FT_Vector *to,
                             void *user)
{
    OutlineFunction *self = static_cast<OutlineFunction *>(user);
    OutlineData data;
    
    data.type = "LineTo";
    data.point[0] = (double)to->x;
    data.point[1] = (double)to->y;
    
    self->m_OutlineList.push_back( data );
    
    return 0;
}


int OutlineFunction::FunctionConicTo(const FT_Vector *control,
                              const FT_Vector *to,
                              void *user)
{
    OutlineFunction *self = static_cast<OutlineFunction *>(user);
    OutlineData data;
    
    data.type = "ConicTo";
    
    data.control1[0] = (double)control->x;
    data.control1[1] = (double)control->y;
    
    data.point[0] = (double)to->x;
    data.point[1] = (double)to->y;
    
    self->m_OutlineList.push_back( data );
    
    return 0;
}


int OutlineFunction::FunctionCubicTo(const FT_Vector *controlOne,
                              const FT_Vector *controlTwo,
                              const FT_Vector *to,
                              void *user)
{
    OutlineFunction *self = static_cast<OutlineFunction *>(user);
    OutlineData data;
    
    data.type = "CubicTo";
    
    data.control1[0] = (double)controlOne->x;
    data.control1[1] = (double)controlOne->y;
    
    data.control2[0] = (double)controlTwo->x;
    data.control2[1] = (double)controlTwo->y;
    
    data.point[0] = (double)to->x;
    data.point[1] = (double)to->y;
    
    self->m_OutlineList.push_back( data );
    
    return 0;
}


void OutlineFunction::DeleteList()
{
    m_OutlineList.clear();
    m_PointGroupList.clear();
}


void OutlineFunction::CreateOutlinePoint()
{
    std::vector< Eigen::Vector2d > points;
    Eigen::Vector2d start, point;
    double t, p0, p1, p2, p3;
    int i;
    int numSplit = 5; //分割数
    
    for( auto outline : m_OutlineList ) {
        
        if( outline.type == "MoveTo" ) {
            
            if( !points.empty() ) {
                m_PointGroupList.push_back( points );
            }
            
            points.clear();
            points.push_back( outline.point );
            
        } else if( outline.type == "LineTo" ) {
            
            points.push_back( outline.point );
            
        } else if( outline.type == "ConicTo" && !points.empty() ) {
            
            start = points.back();
            
            for( i = 1; i <= numSplit; ++i ) {
                
                t = (double)i / (double)numSplit;
                
                p0 = ( 1.0 - t ) * ( 1.0 - t );
                p1 = 2.0 * t * ( 1.0 - t );
                p2 = t * t;
                
                point = start * p0;
                point += outline.control1 * p1;
                point += outline.point * p2;
                
                points.push_back( point );
                
            }
            
        } else if( outline.type == "CubicTo" && !points.empty() ) {
            
            start = points.back();
            
            for( i = 1; i <= numSplit; ++i ) {
                
                t = (double)i / (double)numSplit;
                
                p0 = ( 1.0 - t ) * ( 1.0 - t ) * ( 1.0 - t );
                p1 = 3.0 * t * ( 1.0 - t ) * ( 1.0 - t );
                p2 = 3.0 * t * t * ( 1.0 - t );
                p3 = t * t * t;
                
                point = start * p0;
                point += outline.control1 * p1;
                point += outline.control2 * p2;
                point += outline.point * p3;
                
                points.push_back( point );
                
            }
            
        }
        
    }
    
    if( !points.empty() ) {
        m_PointGroupList.push_back( points );
    }
}
