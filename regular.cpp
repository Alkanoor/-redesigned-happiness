#include "convex_polyhedra_from_points.hpp"
#include "generate_dodecahedron.hpp"
#include "generate_icosahedron.hpp"
#include "generate_tetrahedron.hpp"
#include "generate_octahedron.hpp"
#include "generate_cube.hpp"
#include "util.hpp"

#include <functional>
#include <fstream>


void generate_and_save(const std::string& object_name, const std::string& root_raw, const std::string& root_obj, const std::function<std::vector<std::array<double,3> >(void)>& f)
{
    std::ofstream ofs(root_raw+"/"+object_name,std::ios::out|std::ios::trunc);
    auto p = generate_polyhedra_from_points(f());
    print_endline(ofs,p.points);
    ofs<<std::endl;
    print_sep(ofs,p.faces,' ');

    to_file_obj(p,root_obj+"/"+object_name+".obj");
}

int main()
{
    generate_and_save("dodecahedron","regular","regular_obj",std::bind(&generate_dodecahedron));
    generate_and_save("icosahedron","regular","regular_obj",std::bind(&generate_icosahedron));
    generate_and_save("tetrahedron","regular","regular_obj",std::bind(&generate_tetrahedron));
    generate_and_save("octahedron","regular","regular_obj",std::bind(&generate_octahedron));
    generate_and_save("cube","regular","regular_obj",std::bind(&generate_cube));

    return 0;
}
