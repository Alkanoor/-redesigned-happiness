#ifndef POLYHEDRA_HPP_INCLUDED
#define POLYHEDRA_HPP_INCLUDED


#include "polyhedron_utils.hpp"
#include "util.hpp"

#include <ostream>
#include <vector>


struct Polyhedra
{
    std::vector<Point_3> points;
    std::vector<std::vector<int> > faces;
};

std::ostream& operator<< (std::ostream& os, const Polyhedra& p)
{
    print_endline(os,p.points[i]);
    print_sep(os,p.faces,' ');
    return os;
}

Polyhedra from_file(const std::string& path)
{
    std::vector<Point_3> points;
    std::vector<std::vector<int> > faces;

    int cur = 0;
    std::string tmp;
    bool put_faces = false;
    std::ifstream ifs(path,std::ios::in);

    while(std::getline(ifs,tmp))
    {
        if(tmp=="")
            put_faces = true;
        else
            if(!put_faces)
            {
                std::istringstream iss(tmp);
                double x, y, z;
                iss>>x>>y>>z;
                points.push_back(Point_3(x,y,z));
            }
            else
            {
                faces.push_back(std::vector<int>());
                std::istringstream iss(tmp);
                int index;

                while(iss>>index)
                    faces[cur].push_back(index);

                cur++;
            }
    }

    return Polyhedra({points,faces});
}


#endif
