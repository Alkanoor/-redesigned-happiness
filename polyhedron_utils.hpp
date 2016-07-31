#ifndef POLYHEDRON_UTILS_HPP
#define POLYHEDRON_UTILS_HPP


#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <array>


typedef CGAL::Exact_predicates_inexact_constructions_kernel  K;
typedef CGAL::Polyhedron_3<K>                     Polyhedron_3;
typedef K::Segment_3                              Segment_3;
typedef K::Vector_3                               Vector_3;
typedef K::Point_3                                Point_3;
typedef K::Plane_3                                Plane_3;
typedef Polyhedron_3::Halfedge_iterator           Halfedge_iterator;
typedef Polyhedron_3::Halfedge_handle             Halfedge_handle;
typedef Polyhedron_3::Facet_iterator              Facet_iterator;
typedef CGAL::Creator_uniform_3<double, Point_3>  PointCreator;
typedef Polyhedron_3::Halfedge_around_facet_circulator Halfedge_facet_circulator;


Point_3 convert_from_array(const std::array<double,3>& a)
{
    return Point_3(a[0],a[1],a[2]);
}

std::array<double,3> convert_to_array(const Point_3& p)
{
    return std::array<double,3>({p.x(),p.y(),p.z()});
}

std::array<double,3> convert_to_array(const Vector_3& p)
{
    return std::array<double,3>({p.x(),p.y(),p.z()});
}

Vector_3 point_to_vec(const Point_3& v)
{
    return Vector_3(v.x(),v.y(),v.z());
}

Point_3 vec_to_point(const Vector_3& v)
{
    return Point_3(v.x(),v.y(),v.z());
}


#endif
