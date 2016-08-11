#ifndef POLYHEDRON_TOPOLOGY_HPP
#define POLYHEDRON_TOPOLOGY_HPP


#include "util.hpp"

#include <set>


class Polyhedron_topology
{
    public:
        Polyhedron_topology(const std::vector<std::vector<int> >& faces);

        void construct(const std::vector<std::vector<int> >& faces);

        const std::map<int,std::vector<int> >& get_vertex_to_faces() const;
        const std::map<int,std::set<int> >& get_faces_neighbours() const;

        std::ostream& log(std::ostream& os) const;

    private:
        std::map<int,std::vector<int> > vertex_to_faces;
        std::map<int,std::set<int> > faces_neighbours;
};


#endif
