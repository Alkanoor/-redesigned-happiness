#include "generate_dodecahedron.hpp"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/point_generators_3.h>
#include <CGAL/convex_hull_3.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/algorithm.h>
#include <CGAL/Simple_cartesian.h>


typedef CGAL::Inexact_predicates_inexact_constructions_kernel  K;
typedef CGAL::Polyhedron_3<K>                     Polyhedron_3;
typedef K::Segment_3                              Segment_3;
typedef K::Point_3                                Point_3;
typedef K::Plane_3                                Plane_3;
typedef Polyhedron_3::Facet_iterator              Facet_iterator;
typedef Polyhedron_3::Halfedge_iterator           Halfedge_iterator;
typedef Polyhedron_3::Halfedge_handle Halfedge_handle;
typedef CGAL::Creator_uniform_3<double, Point_3>  PointCreator;
typedef Polyhedron_3::Halfedge_around_facet_circulator Halfedge_facet_circulator;


struct Plane_from_facet
{
    Polyhedron_3::Plane_3 operator()(Polyhedron_3::Facet& f)
    {
        Polyhedron_3::Halfedge_handle h = f.halfedge();
        return Polyhedron_3::Plane_3( h->vertex()->point(),
                                    h->next()->vertex()->point(),
                                    h->opposite()->vertex()->point());
    }
};

int main()
{
    std::vector<std::array<double,3> > vertices = generate_dodecahedron();

    std::vector<Point_3> points(vertices.size());
    for(int i=0;i<(int)vertices.size();i++)
        points[i] = Point_3(vertices[i][0],vertices[i][1],vertices[i][2]);

    Polyhedron_3 poly;

    CGAL::convex_hull_3(points.begin(), points.end(), poly);
    std::cout << "The convex hull contains " << poly.size_of_vertices() << " vertices" << std::endl;

    std::transform(poly.facets_begin(), poly.facets_end(), poly.planes_begin(), Plane_from_facet());

    CGAL::set_ascii_mode(std::cout);
    std::cout << "OFF" << std::endl << poly.size_of_vertices() << ' '
              << poly.size_of_facets() << " 0" << std::endl;
    std::copy( poly.points_begin(), poly.points_end(),
               std::ostream_iterator<Point_3>( std::cout, "\n"));
    for (  Facet_iterator i = poly.facets_begin(); i != poly.facets_end(); ++i) {
        Halfedge_facet_circulator j = i->facet_begin();
        // Facets in polyhedral surfaces are at least triangles.
        CGAL_assertion( CGAL::circulator_size(j) >= 3);
        std::cout << CGAL::circulator_size(j) << ' ';
        do {
            std::cout << ' ' << std::distance(poly.vertices_begin(), j->vertex());
        } while ( ++j != i->facet_begin());
        std::cout << std::endl;
    }

    for(Halfedge_iterator half=poly.halfedges_begin();half!=poly.halfedges_end();++half)
    {
        Point_3 v1 = half->vertex()->point();
        Point_3 v2 = half->next()->vertex()->point();
        Point_3 v3 = half->next()->next()->vertex()->point();
        Point_3 v4 = half->opposite()->vertex()->point();
        Point_3 v5 = half->opposite()->next()->vertex()->point();
        Point_3 v6 = half->opposite()->next()->next()->vertex()->point();

        /*std::cout<<"Cur point"<<std::endl;
        std::cout<<half->vertex()->point()<<std::endl;
        std::cout<<"Cur point next"<<std::endl;
        std::cout<<half->next()->vertex()->point()<<std::endl;
        std::cout<<half->next()->next()->vertex()->point()<<std::endl;
        std::cout<<half->next()->next()->next()->vertex()->point()<<std::endl;
        std::cout<<"Cur point opposite"<<std::endl;
        std::cout<<half->opposite()->vertex()->point()<<std::endl;
        std::cout<<v1<<" "<<v2<<" "<<v3<<" "<<v4<<std::endl;
        */
        std::cout<<v1<<" ; "<<v2<<" ; "<<v3<<" ; "<<v4<<" ; "<<v5<<" ; "<<v6<<std::endl;
        std::cout<<(CGAL::coplanar(v1,v2,v3,v4)&&CGAL::coplanar(v1,v2,v3,v5)&&CGAL::coplanar(v1,v2,v3,v6)&&
                    CGAL::coplanar(v1,v2,v4,v5)&&CGAL::coplanar(v1,v2,v4,v6)&&CGAL::coplanar(v1,v2,v5,v6)&&
                    CGAL::coplanar(v1,v3,v4,v5)&&CGAL::coplanar(v1,v3,v4,v6)&&CGAL::coplanar(v1,v4,v5,v6)&&
                    CGAL::coplanar(v2,v3,v4,v5)&&CGAL::coplanar(v2,v3,v4,v6)&&CGAL::coplanar(v2,v3,v5,v6)&&
                    CGAL::coplanar(v2,v4,v5,v6)&&CGAL::coplanar(v1,v3,v5,v6)&&CGAL::coplanar(v3,v4,v5,v6))<<std::endl;
        if(CGAL::coplanar(v1,v2,v3,v4)&&CGAL::coplanar(v1,v2,v3,v5)&&CGAL::coplanar(v1,v2,v3,v6)&&
                    CGAL::coplanar(v1,v2,v4,v5)&&CGAL::coplanar(v1,v2,v4,v6)&&CGAL::coplanar(v1,v2,v5,v6)&&
                    CGAL::coplanar(v1,v3,v4,v5)&&CGAL::coplanar(v1,v3,v4,v6)&&CGAL::coplanar(v1,v4,v5,v6)&&
                    CGAL::coplanar(v2,v3,v4,v5)&&CGAL::coplanar(v2,v3,v4,v6)&&CGAL::coplanar(v2,v3,v5,v6)&&
                    CGAL::coplanar(v2,v4,v5,v6)&&CGAL::coplanar(v1,v3,v5,v6)&&CGAL::coplanar(v3,v4,v5,v6))
        {

            auto removeMe = half;
            while (CGAL::circulator_size(removeMe->vertex_begin()) < 3)							// Mover handle to beginning of linestring
                removeMe = removeMe->next();

            std::cout << "\rFacets   before/after:  -> "<< poly.size_of_facets()<<". ("<<poly.size_of_halfedges()<<"%)";

            while (CGAL::circulator_size(removeMe->opposite()->vertex_begin()) < 3)			// Join vertexes until at the other end of linestring
                if (removeMe->facet_degree()>3 && removeMe->opposite()->facet_degree()>3)
                    removeMe = (poly.join_vertex(removeMe))->next()->opposite();
                else																		// One of the faces turned into a triangle ->remove center vertex
                    break;
            if (CGAL::circulator_size(removeMe->opposite()->vertex_begin()) < 3)			// Remove remained of the border
                poly.erase_center_vertex(removeMe->opposite());								// if two segments remain remove center point
            else
                poly.join_facet(removeMe);														// if one segment remains join facets

            //half = poly.join_facet(half);
            half=poly.halfedges_begin();

            for (  Facet_iterator i = poly.facets_begin(); i != poly.facets_end(); ++i) {
                Halfedge_facet_circulator j = i->facet_begin();
                // Facets in polyhedral surfaces are at least triangles.
                CGAL_assertion( CGAL::circulator_size(j) >= 3);
                std::cout << CGAL::circulator_size(j) << ' ';
                do {
                    std::cout << ' ' << std::distance(poly.vertices_begin(), j->vertex());
                } while ( ++j != i->facet_begin());
                std::cout << std::endl;
            }
        }
    }


    CGAL::set_ascii_mode(std::cout);
    std::cout << "OFF" << std::endl << poly.size_of_vertices() << ' '
              << poly.size_of_facets() << " 0" << std::endl;
    std::copy( poly.points_begin(), poly.points_end(),
               std::ostream_iterator<Point_3>( std::cout, "\n"));
    for (  Facet_iterator i = poly.facets_begin(); i != poly.facets_end(); ++i) {
        Halfedge_facet_circulator j = i->facet_begin();
        // Facets in polyhedral surfaces are at least triangles.
        CGAL_assertion( CGAL::circulator_size(j) >= 3);
        std::cout << CGAL::circulator_size(j) << ' ';
        do {
            std::cout << ' ' << std::distance(poly.vertices_begin(), j->vertex());
        } while ( ++j != i->facet_begin());
        std::cout << std::endl;
    }


    return 0;
}
