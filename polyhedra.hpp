#ifndef POLYHEDRA_HPP_INCLUDED
#define POLYHEDRA_HPP_INCLUDED


#include "polyhedron_utils.hpp"


struct Polyhedra
{
    std::vector<Point_3> points;
    std::vector<std::vector<int> > faces;
};


#endif
