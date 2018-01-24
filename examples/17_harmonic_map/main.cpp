#include <QApplication>
#include <cinolib/meshes/meshes.h>
#include <cinolib/gui/qt/glcanvas.h>
#include <cinolib/harmonic_map.h>
#include <cinolib/geometry/n_sided_poygon.h>
#include <cinolib/textures/textures.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    using namespace cinolib;

    QApplication a(argc, argv);

    // read a mesh from file
    std::string s = (argc==2) ? std::string(argv[1]) : std::string(DATA_PATH) + "/Laurana.obj";
    DrawableTrimesh<> m(s.c_str());

    // ordered list of boundary vertices
    std::vector<uint> boundary = m.get_ordered_boundary_vertices();

    // create parametric space (discrete unit circle with as many point as the boundary vertices)
    std::vector<vec3d> uv_boundary = n_sided_polygon(vec3d(0,0,0), boundary.size(), 1.0);

    // set potitional constraints for boundary vertices (map its boundary to the discrete circle)
    std::map<uint,vec3d> dirichlet_bcs;
    for(uint i=0; i<boundary.size(); ++i) dirichlet_bcs[boundary.at(i)] = uv_boundary.at(i);

    // solve for the interior vertices via harmonic map
    std::vector<vec3d> uv_map = harmonic_map_3d(m, dirichlet_bcs);

    // create a mesh of the uv parametric space
    DrawableTrimesh<> m_uv(uv_map, m.vector_polys());

    // copy uv coordinates to m (for texture visualization)
    for(uint vid=0; vid<m.num_verts(); ++vid) m.vert_data(vid).uvw = m_uv.vert(vid);

    // visualize original and parametric mesh in two separated windows
    GLcanvas gui;
    GLcanvas gui_uv;
    gui.show();
    gui_uv.show();
    m.show_wireframe(true);
    m.show_wireframe_transparency(0.4);
    m.show_texture2D(TEXTURE_2D_ISOLINES, 3.0);
    gui.push_obj(&m);
    m_uv.show_wireframe(true);
    m_uv.show_wireframe_transparency(0.4);
    m_uv.show_texture2D(TEXTURE_2D_ISOLINES, 3.0);
    gui_uv.push_obj(&m_uv);

    return a.exec();
}