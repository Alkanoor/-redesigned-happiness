#include "generate_dodecahedron.hpp"
#include "convex_polyhedra_from_points.hpp"
#include "util.hpp"

int main()
{
    auto p = generate_polyhedra_from_points(generate_dodecahedron());
    print_endline(std::cout,p.points);
    print_sep(std::cout,p.faces,' ');

    return 0;
}
