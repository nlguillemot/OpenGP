#include <fstream> ///< write file
#include <iomanip> ///< precision
#include <OpenGP/Surface_mesh.h>

using namespace std;
using namespace opengp;

/// Takes a mesh and generates a source file representing that mesh
/// Produces: vertices, normals, render indexes
int main(int argc, char** argv){
    if(argc!=3){
        cout << "usage:" << endl << "baker icopill.obj icopill.h" << endl;
        return EXIT_FAILURE;
    }
    std::string input(argv[1]);
    std::string output(argv[2]);
    
    ///--- Load mesh
    Surface_mesh mesh;
    mesh.read(input);
    if(mesh.n_vertices()==0){
        cout << "Input mesh has 0 vertices" << endl;
        return EXIT_FAILURE;
    }
    
    /// File open for writing
    /// Any contents that existed in the file before it is open are discarded.
    std::ofstream file(output.c_str(), ios::out | ios::trunc);
    
    ///--- Create OpenGL triangle index buffer
    mesh.triangulate();
    std::vector<unsigned int> triangles;
    assert(mesh.is_triangle_mesh());
    triangles.clear();
    for(auto f: mesh.faces())
        for(auto v: mesh.vertices(f))
            triangles.push_back(v.idx());
    
    ///--- Compute normals
    mesh.update_vertex_normals();

    file << "namespace icopill{" << endl;
        ///--- Header
        file << "const unsigned int n_vpoint    = " << mesh.n_vertices() << ";" << endl;
        file << "const unsigned int n_vnormal   = " << mesh.n_vertices() << ";" << endl;
        file << "const unsigned int n_findex    = " << triangles.size() << ";" << endl;
        file << "const unsigned int n_triangles = " << mesh.n_faces() << ";" << endl;
        file << endl;
        
        ///--- Set precision of coordinates, show even if zero, with sign
        file << std::setprecision(4) << std::fixed << std::showpos;
        
        ///--- Vertex buffer
        auto vpoints = mesh.get_vertex_property<Vec3>("v:point");
        file << "const GLfloat vpoint[] = {" << endl;
            for(auto v: mesh.vertices())
            {
                Vec3 vp = vpoints[v];
                file << "    " << vp[0] << "," << vp[1] << "," << vp[2] << "," << endl;
            }
        file << "};" << endl;
        file << endl;
        
        ///--- Normal buffer   
        auto vnormals = mesh.get_vertex_property<Vec3>("v:normal");
        file << "const GLfloat vnormal[] = {" << endl;
            for(auto v: mesh.vertices()){
                Vec3 vp = vnormals[v];
                file << "    " << vp[0] << "," << vp[1] << "," << vp[2] << "," << endl;
            }
        file << "};" << endl;
        
        ///--- Index buffer
        file << std::noshowpos;
        file << "const unsigned int findex[] = {" << endl;
            for(auto f: mesh.faces()){
                file << "    ";
                for(auto v: mesh.vertices(f))
                    file << v.idx() << ","; 
                file << endl;
            }
                    
        file << "};" << endl;        
    file << "} // icopill::" << endl;
    

    
    file.close();
    return EXIT_SUCCESS;
}
