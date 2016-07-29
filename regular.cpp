#include "convex_polyhedra_from_points.hpp"
#include "generate_dodecahedron.hpp"
#include "generate_icosahedron.hpp"
#include "generate_tetrahedron.hpp"
#include "generate_octahedron.hpp"
#include "generate_cube.hpp"
#include "util.hpp"

#include <functional>
#include <fstream>


void generate_and_save(const std::string& path, const std::function<std::vector<std::array<double,3> >(void)>& f)
{
    std::ofstream ofs(path,std::ios::out|std::ios::trunc);
    auto p = generate_polyhedra_from_points(f());
    print_endline(ofs,p.points);
    ofs<<std::endl;
    print_sep(ofs,p.faces,' ');
}

int main()
{
    generate_and_save("regular/dodecahedron",std::bind(&generate_dodecahedron));
    generate_and_save("regular/icosahedron",std::bind(&generate_icosahedron));
    generate_and_save("regular/tetrahedron",std::bind(&generate_tetrahedron));
    generate_and_save("regular/octahedron",std::bind(&generate_octahedron));
    generate_and_save("regular/cube",std::bind(&generate_cube));

    return 0;
}
