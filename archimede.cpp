#include "convex_polyhedra_from_points.hpp"
#include "generate_dodecahedron.hpp"
#include "generate_icosahedron.hpp"
#include "generate_tetrahedron.hpp"
#include "generate_octahedron.hpp"
#include "generate_cube.hpp"
#include "util.hpp"

#include <functional>
#include <fstream>
#include <sstream>


std::map<std::string,std::function<Polyhedra(Polyhedra)> > operations;

std::string file(const std::string& in)
{
    std::string res;
    int i = (int)in.size()-1;
    while(i>0&&in[i]!='/')
    {
        res = in[i]+res;
        i--;
    }
    return res;
}

void generate_and_save(const std::string& path)
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
        std::cout<<tmp<<std::endl;
    }

    Polyhedra p{points,faces};

    for(auto it=operations.begin(); it!=operations.end(); it++)
    {
        std::ofstream ofs("archimede/"+file(path)+"."+it->first,std::ios::out|std::ios::trunc);
        Polyhedra p2 = it->second(p);
        print_endline(ofs,p2.points);
        ofs<<std::endl;
        print_sep(ofs,p2.faces,' ');
    }
}

Polyhedra identity(const Polyhedra& p)
{return p;}

Polyhedra dual(const Polyhedra& p)
{
    std::vector<std::array<double,3> > points(p.faces.size());

    for(unsigned int i=0;i<p.faces.size();i++)
    {
        std::array<double,3> cur = {0,0,0};
        for(unsigned int j=0;j<p.faces[i].size();j++)
        {
            cur[0] += p.points[p.faces[i][j]].x()/(double)p.faces[i].size();
            cur[1] += p.points[p.faces[i][j]].y()/(double)p.faces[i].size();
            cur[2] += p.points[p.faces[i][j]].z()/(double)p.faces[i].size();
        }
        points[i] = cur;
    }
    return generate_polyhedra_from_points(points);
}

Polyhedra truncated(const Polyhedra& p)
{
    std::vector<std::array<double,3> > points;
    std::map<int,std::set<int> > edges;

    for(unsigned int i=0;i<p.faces.size();i++)
    {
        for(unsigned int j=0;j<p.faces[i].size();j++)
        {
            
        }
    }
    return generate_polyhedra_from_points(points);
}

Polyhedra rectified(const Polyhedra& p)
{return p;}

Polyhedra biseauted(const Polyhedra& p)
{return p;}

Polyhedra omnitruncated(const Polyhedra& p)
{return p;}

Polyhedra softened(const Polyhedra& p)
{return p;}

Polyhedra dual_truncated(const Polyhedra& p)
{return p;}

int main()
{
    operations["identity"] = std::bind(&identity,std::placeholders::_1);
    operations["dual"] = std::bind(&dual,std::placeholders::_1);
    operations["truncated"] = std::bind(&truncated,std::placeholders::_1);
    operations["rectified"] = std::bind(&rectified,std::placeholders::_1);
    operations["biseauted"] = std::bind(&biseauted,std::placeholders::_1);
    operations["omnitruncated"] = std::bind(&omnitruncated,std::placeholders::_1);
    operations["softened"] = std::bind(&softened,std::placeholders::_1);
    operations["dual_truncated"] = std::bind(&dual_truncated,std::placeholders::_1);

    generate_and_save("regular/dodecahedron");
    generate_and_save("regular/icosahedron");
    generate_and_save("regular/tetrahedron");
    generate_and_save("regular/octahedron");
    generate_and_save("regular/cube");

    return 0;
}
