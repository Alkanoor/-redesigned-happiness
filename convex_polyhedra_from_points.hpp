#ifndef CONVEX_POLYHEDRA_FROM_POINTS_HPP
#define CONVEX_POLYHEDRA_FROM_POINTS_HPP


#include "polyhedra.hpp"

#include <CGAL/convex_hull_3.h>
#include <CGAL/algorithm.h>


bool coplanar_handmade(const Point_3& p1, const Point_3& p2, const Point_3& p3, const Point_3& p4, double precision = 0.0001)
{
    Vector_3 v1 = p2-p1, v2 = p3-p1, v3 = p4-p1;
    auto v4 = CGAL::cross_product(v1,v2);
    double v = v3*v4;
    return fabs(v)<precision;
}


struct Plane_from_facet
{
    Polyhedron_3::Plane_3 operator()(Polyhedron_3::Facet& f)
    {
        Polyhedron_3::Halfedge_handle h = f.halfedge();
        return Polyhedron_3::Plane_3(h->vertex()->point(),
                                     h->next()->vertex()->point(),
                                     h->opposite()->vertex()->point());
    }
};


Polyhedra generate_polyhedra_from_points(const std::vector<std::array<double,3> >& vertices)
{

    std::vector<Point_3> points(vertices.size());
    for(int i=0;i<(int)vertices.size();i++)
        points[i] = Point_3(vertices[i][0],vertices[i][1],vertices[i][2]);

    Polyhedron_3 poly;
    CGAL::convex_hull_3(points.begin(), points.end(), poly);
    std::transform(poly.facets_begin(), poly.facets_end(), poly.planes_begin(), Plane_from_facet());

    for(Halfedge_iterator half=poly.halfedges_begin();half!=poly.halfedges_end();++half)
    {
        Point_3 v1 = half->vertex()->point();
        Point_3 v2 = half->next()->vertex()->point();
        Point_3 v3 = half->next()->next()->vertex()->point();
        Point_3 v4 = half->opposite()->vertex()->point();
        Point_3 v5 = half->opposite()->next()->vertex()->point();
        Point_3 v6 = half->opposite()->next()->next()->vertex()->point();

        if(coplanar_handmade(v1,v2,v3,v4)&&coplanar_handmade(v1,v2,v3,v5)&&coplanar_handmade(v1,v2,v3,v6)&&
                    coplanar_handmade(v1,v2,v4,v5)&&coplanar_handmade(v1,v2,v4,v6)&&coplanar_handmade(v1,v2,v5,v6)&&
                    coplanar_handmade(v1,v3,v4,v5)&&coplanar_handmade(v1,v3,v4,v6)&&coplanar_handmade(v1,v4,v5,v6)&&
                    coplanar_handmade(v2,v3,v4,v5)&&coplanar_handmade(v2,v3,v4,v6)&&coplanar_handmade(v2,v3,v5,v6)&&
                    coplanar_handmade(v2,v4,v5,v6)&&coplanar_handmade(v1,v3,v5,v6)&&coplanar_handmade(v3,v4,v5,v6))
        {
            poly.join_facet(half);
            half=poly.halfedges_begin();
        }
    }

    int cur = 0;
    for(auto it=poly.points_begin(); it!=poly.points_end(); it++, cur++)
            points[cur] = *it;

    cur = 0;
    std::vector<std::vector<int> > faces;
    for(Facet_iterator i=poly.facets_begin(); i!=poly.facets_end(); i++)
    {
        faces.push_back(std::vector<int>());
        Halfedge_facet_circulator j = i->facet_begin();
        do
        {faces[cur].push_back(std::distance(poly.vertices_begin(), j->vertex()));} while (++j != i->facet_begin());
        cur++;
    }

    return Polyhedra{points,faces};
}


#endif
