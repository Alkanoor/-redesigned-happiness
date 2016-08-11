#include "polyhedron_topology.hpp"


Polyhedron_topology::Polyhedron_topology(const std::vector<std::vector<int> >& faces)
{construct(faces);}

void Polyhedron_topology::construct(const std::vector<std::vector<int> >& faces)
{
    vertex_to_faces.clear();
    faces_neighbours.clear();

    for(int i=0; i<(int)faces.size(); i++)
        for(int j : faces[i])
            vertex_to_faces[j].push_back(i);

    for(int i=0; i<(int)faces.size(); i++)
        for(int j : faces[i])
            for(int k : vertex_to_faces[j])
                for(int l : faces[i])
                    if(j!=l)
                        for(int m : vertex_to_faces[l])
                            if(k!=i&&m!=i&&m==k)
                                faces_neighbours[i].insert(k);
}

const std::map<int,std::vector<int> >& Polyhedron_topology::get_vertex_to_faces() const
{return vertex_to_faces;}

const std::map<int,std::set<int> >& Polyhedron_topology::get_faces_neighbours() const
{return faces_neighbours;}

std::ostream& Polyhedron_topology::log(std::ostream& os) const
{
    for(auto i : vertex_to_faces)
    {
        os<<"Point : "<<i.first<<" => ";
        print_sep(os,i.second,' ');
        os<<std::endl;
    }
    os<<std::endl;
    for(auto i : faces_neighbours)
    {
        os<<"Face : "<<i.first<<" => ";
        for(auto j : i.second)
            os<<j<<' ';
        os<<std::endl;
    }
    os<<std::endl;

    return os;
}
