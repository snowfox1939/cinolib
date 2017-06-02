/*********************************************************************************
*  Copyright(C) 2016: Marco Livesu                                               *
*  All rights reserved.                                                          *
*                                                                                *
*  This file is part of CinoLib                                                  *
*                                                                                *
*  CinoLib is dual-licensed:                                                     *
*                                                                                *
*   - For non-commercial use you can redistribute it and/or modify it under the  *
*     terms of the GNU General Public License as published by the Free Software  *
*     Foundation; either version 3 of the License, or (at your option) any later *
*     version.                                                                   *
*                                                                                *
*   - If you wish to use it as part of a commercial software, a proper agreement *
*     with the Author(s) must be reached, based on a proper licensing contract.  *
*                                                                                *
*  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE       *
*  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.     *
*                                                                                *
*  Author(s):                                                                    *
*                                                                                *
*     Marco Livesu (marco.livesu@gmail.com)                                      *
*     http://pers.ge.imati.cnr.it/livesu/                                        *
*                                                                                *
*     Italian National Research Council (CNR)                                    *
*     Institute for Applied Mathematics and Information Technologies (IMATI)     *
*     Via de Marini, 6                                                           *
*     16149 Genoa,                                                               *
*     Italy                                                                      *
**********************************************************************************/
#include <cinolib/io/write_OBJ.h>
#include <cinolib/colors.h>


#include <iostream>
#include <algorithm>

namespace cinolib
{


CINO_INLINE
void write_OBJ(const char                * filename,
               const std::vector<double> & xyz,
               const std::vector<u_int>  & tri,
               const std::vector<u_int>  & quad)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "w");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_OBJ() : couldn't open input file " << filename << endl;
        exit(-1);
    }

    for(size_t i=0; i<xyz.size(); i+=3)
    {
        // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        fprintf(fp, "v %.17g %.17g %.17g\n", xyz[i], xyz[i+1], xyz[i+2]);
    }

    for(size_t i=0; i<tri.size(); i+=3)
    {
        fprintf(fp, "f %d %d %d\n", tri[i] + 1, tri[i+1] + 1, tri[i+2] + 1);
    }

    for(size_t i=0; i<quad.size(); i+=4)
    {
        fprintf(fp, "f %d %d %d %d\n", quad[i] + 1, quad[i+1] + 1, quad[i+2] + 1, quad[i+3] + 1);
    }

    fclose(fp);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void write_OBJ(const char                           * filename,
               const std::vector<double>            & xyz,
               const std::vector<std::vector<uint>> & poly)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "w");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_OBJ() : couldn't open input file " << filename << endl;
        exit(-1);
    }

    for(uint i=0; i<xyz.size(); i+=3)
    {
        // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        fprintf(fp, "v %.17g %.17g %.17g\n", xyz[i], xyz[i+1], xyz[i+2]);
    }

    for(auto p : poly)
    {
        fprintf(fp, "f ");
        for(uint vid : p) fprintf(fp, "%d ", vid+1);
        fprintf(fp, "\n");
    }

    fclose(fp);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void write_OBJ(const char                * filename,
               const std::vector<double> & xyz,
               const std::vector<u_int>  & tri,
               const std::vector<u_int>  & quad,
               const std::vector<int>    & labels)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    std::string mtl_filename(filename);
    mtl_filename.resize(mtl_filename.size()-4);
    mtl_filename.append(".mtu");

    FILE *f_mtl = fopen(mtl_filename.c_str(), "w");
    FILE *f_obj = fopen(filename, "w");

    if(!f_obj || !f_mtl)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_OBJ() : couldn't open input file " << filename << endl;
        exit(-1);
    }

    int  min    = *std::min_element(labels.begin(), labels.end());
    int  max    = *std::max_element(labels.begin(), labels.end());
    int  delta  = max - min;

    for(int l=min; l<=max; ++l)
    {
        float rgb[3];
        scattered_color(delta, l, rgb);
        fprintf(f_mtl, "newmtl label_%d\nKd %f %f %f\n", l, rgb[0], rgb[1], rgb[2]);
    }

    fprintf(f_obj, "mtllib %s\n", mtl_filename.c_str());

    for(size_t i=0; i<xyz.size(); i+=3)
    {
        // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        fprintf(f_obj, "v %.17g %.17g %.17g\n", xyz[i], xyz[i+1], xyz[i+2]);
    }

    for(size_t i=0; i<tri.size(); i+=3)
    {
        fprintf(f_obj, "usemtl label_%d\n", labels.at(i/3));
        fprintf(f_obj, "f %d %d %d\n", tri[i] + 1, tri[i+1] + 1, tri[i+2] + 1);
    }

    for(size_t i=0; i<quad.size(); i+=4)
    {
        fprintf(f_obj, "usemtl label_%d\n", labels.at(i/4));
        fprintf(f_obj, "f %d %d %d %d\n", quad[i] + 1, quad[i+1] + 1, quad[i+2] + 1, quad[i+3] + 1);
    }

    fclose(f_obj);
    fclose(f_mtl);
}

}
