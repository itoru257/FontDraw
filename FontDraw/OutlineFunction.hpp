/*************************************************
 * Copyright (c) 2016 Toru Ito
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php
 *************************************************/

#ifndef OutlineFunction_hpp
#define OutlineFunction_hpp

#include <stdio.h>
#include <string>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_BBOX_H

#include <Eigen/Core>
#include <Eigen/Geometry>

class OutlineFunction
{
public:
    struct OutlineData
    {
        std::string type;
        Eigen::Vector2d point;
        Eigen::Vector2d control1;
        Eigen::Vector2d control2;
        
        OutlineData()
        {
            type     = "";
            point    = Eigen::Vector2d::Zero();
            control1 = Eigen::Vector2d::Zero();
            control2 = Eigen::Vector2d::Zero();
        }
    };
    
public:
    OutlineFunction();
    ~OutlineFunction();
    
    static int FunctionMoveTo(const FT_Vector *to,
                              void *user );
    
    static int FunctionLineTo(const FT_Vector *to,
                              void *user );
    
    static int FunctionConicTo(const FT_Vector *control,
                               const FT_Vector *to,
                               void *user) ;
    
    static int FunctionCubicTo(const FT_Vector *controlOne,
                               const FT_Vector *controlTwo,
                               const FT_Vector *to,
                               void *user );
    
    void InitOutlineFuncs();
    void CreateOutlinePoint();
    void DeleteList();
    
    std::vector< std::vector< Eigen::Vector2d > >  m_PointGroupList;
    std::vector< OutlineData >  m_OutlineList;

    FT_Outline_Funcs  m_OutlineFuncs;
    
};

#endif /* OutlineFunction_hpp */
