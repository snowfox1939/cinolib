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
#include <cinolib/io/write_VTU.h>

#ifdef CINOLIB_USES_VTK
#include <vtkSmartPointer.h>
#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkIntArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridWriter.h>
#endif

namespace cinolib
{

#ifndef CINOLIB_USES_VTK

CINO_INLINE
void write_VTU(const char                *,
               const std::vector<double> &,
               const std::vector<u_int>  &,
               const std::vector<u_int>  &)
{
    std::cerr << "ERROR : VTK missing. Install VTK and recompile defining symbol CINOLIB_USES_VTK" << endl;
    exit(-1);
}

#else

CINO_INLINE
void write_VTU(const char                * filename,
               const std::vector<double> & xyz,
               const std::vector<u_int>  & tets,
               const std::vector<u_int>  & hexa)
{

    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
    vtkSmartPointer<vtkUnstructuredGrid>          grid   = vtkSmartPointer<vtkUnstructuredGrid>::New();
    vtkSmartPointer<vtkPoints>                    points = vtkSmartPointer<vtkPoints>::New();

    // generate some arrays that allow each element type to be viewed alone by thresholding
    //
    vtkSmartPointer<vtkIntArray> tetselector = vtkSmartPointer<vtkIntArray>::New();
    vtkSmartPointer<vtkIntArray> hexselector = vtkSmartPointer<vtkIntArray>::New();
    tetselector->SetName("tet_selector");
    hexselector->SetName("hex_selector");

    // write the vertex coordinates
    //
    for(size_t i=0; i<xyz.size(); i+=3)
    {
        points->InsertNextPoint(xyz[i+0], xyz[i+1], xyz[i+2]);
    }

    // write the tetrahedra
    //
    for(size_t i=0; i<tets.size(); i+=4)
    {
        vtkIdType pid[] = { tets[i+0], tets[i+1], tets[i+2], tets[i+3] };
        grid->InsertNextCell(VTK_TETRA, 4, pid);
        tetselector->InsertNextValue(1);
        hexselector->InsertNextValue(0);
    }

    // write the hexahedra
    //
    for(size_t i=0; i<hexa.size(); i+=8 )
    {
        vtkIdType pid[] = { hexa[i+0], hexa[i+1], hexa[i+2], hexa[i+3],
                            hexa[i+4], hexa[i+5], hexa[i+6], hexa[i+7] };
        grid->InsertNextCell(VTK_HEXAHEDRON, 8, pid);
        tetselector->InsertNextValue(0);
        hexselector->InsertNextValue(1);
    }

    // create the output mesh
    //
    grid->SetPoints(points);

    if (tets.size() > 0) grid->GetCellData()->AddArray(tetselector);
    if (hexa.size() > 0) grid->GetCellData()->AddArray(hexselector);

#if VTK_MAJOR_VERSION < 6
    writer->SetInput( grid );
#else
    writer->SetInputData(grid);
#endif
    writer->SetFileName(filename);
    writer->Write();
}
#endif

}
