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
        unsigned int s = p.faces[i].size();
        for(unsigned int j=0;j<s;j++)
        {
            if(edges.count(p.faces[i][j])&&edges[p.faces[i][j]].count(p.faces[i][(j+1)%s])!=0)
                break;
            edges[p.faces[i][j]].insert(p.faces[i][(j+1)%s]);
            edges[p.faces[i][(j+1)%s]].insert(p.faces[i][j]);
            Vector_3 cur = p.points[p.faces[i][(j+1)%s]]-p.points[p.faces[i][j]];
            points.push_back(convert_to_array(p.points[p.faces[i][j]]+cur/3));
            points.push_back(convert_to_array(p.points[p.faces[i][j]]+cur*2/3));
        }
    }

    return generate_polyhedra_from_points(points);
}

Polyhedra rectified(const Polyhedra& p)
{
    std::vector<std::array<double,3> > points;
    std::map<int,std::set<int> > edges;

    for(unsigned int i=0;i<p.faces.size();i++)
    {
        unsigned int s = p.faces[i].size();
        for(unsigned int j=0;j<s;j++)
        {
            if(edges.count(p.faces[i][j])&&edges[p.faces[i][j]].count(p.faces[i][(j+1)%s])!=0)
                break;
            edges[p.faces[i][j]].insert(p.faces[i][(j+1)%s]);
            edges[p.faces[i][(j+1)%s]].insert(p.faces[i][j]);
            Vector_3 cur = p.points[p.faces[i][(j+1)%s]]-p.points[p.faces[i][j]];
            points.push_back(convert_to_array(p.points[p.faces[i][j]]+cur/2));
        }
    }
    return generate_polyhedra_from_points(points);
}

Polyhedra biseauted(const Polyhedra& p)
{
    std::vector<std::array<double,3> > points;

    Vector_3 center(0,0,0);

    for(unsigned int i=0;i<p.points.size();i++)
        center = center+(p.points[i]-Point_3(0,0,0));
    center = center/(double)p.points.size();
    double a = sqrt(CGAL::squared_distance(p.points[p.faces[0][0]],p.points[p.faces[0][1]]))/sqrt(2.0);

    for(unsigned int i=0;i<p.faces.size();i++)
    {
        unsigned int s = p.faces[i].size();
        Vector_3 middle_face(0,0,0);
        for(unsigned int j=0;j<s;j++)
            middle_face = middle_face+(p.points[p.faces[i][j]]-Point_3(0,0,0));
        middle_face = middle_face/(double)s;
        Vector_3 normalized_dif = middle_face-center;
        normalized_dif = normalized_dif/sqrt(normalized_dif.squared_length());
        Vector_3 new_center = middle_face+normalized_dif*a;

        for(unsigned int j=0;j<s;j++)
        {
            Vector_3 dif = point_to_vec(p.points[p.faces[i][j]])-middle_face;
            points.push_back(convert_to_array(new_center+dif));
        }
    }

    return generate_polyhedra_from_points(points);
}

Polyhedra softened_on_omni(const Polyhedra& p)
{
    std::vector<std::array<double,3> > points;
    std::map<int,char> point_tags;
    std::set<int> done_faces;

    int s;
    int cur_face = 0;
    int cur_modulo = 0;
    while(done_faces.size()<p.faces.size())
    {
        s = p.faces[cur_face].size();
        std::cout<<"New face ======================= "<<s<<std::endl;
        done_faces.insert(cur_face);
        for(int j=0;j<s;j++)
            if(j%2==cur_modulo)
            {
                if(point_tags.count(p.faces[cur_face][j])&&point_tags[p.faces[cur_face][j]]!=0)
                {
                    std::cout<<"Should not happen in softening, face "<<cur_face<<std::endl;
                    exit(0);
                }
                point_tags[p.faces[cur_face][j]] = 0;
                points.push_back(convert_to_array(p.points[p.faces[cur_face][j]]));
                std::cout<<"added "<<p.faces[cur_face][j]<<" "<<(int)point_tags[p.faces[cur_face][j]]<<std::endl;
            }
            else
            {
                if(point_tags.count(p.faces[cur_face][j])&&point_tags[p.faces[cur_face][j]]!=1)
                {
                    std::cout<<"Should not happen in softening, face "<<cur_face<<std::endl;
                    exit(0);
                }
                point_tags[p.faces[cur_face][j]] = 1;
                std::cout<<"no "<<p.faces[cur_face][j]<<" "<<(int)point_tags[p.faces[cur_face][j]]<<std::endl;
            }

        for(unsigned int i=0;i<p.faces.size();i++)
            if(!done_faces.count(i))
            {
                int found = -1;
                s = p.faces[i].size();
                for(int j=0;j<s;j++)
                    if(point_tags.count(p.faces[i][j]))
                    {
                        found = j;
                        break;
                    }

                if(found>=0)
                {
                    std::cout<<"Super found ! "<<i<<" "<<found<<" "<<(int)point_tags[p.faces[cur_face][found]]<<std::endl;
                    cur_face = i;
                    if(point_tags[p.faces[cur_face][found]]==0)
                        cur_modulo = found%2;
                    else
                        cur_modulo = 1-(found%2);

                    break;
                }
            }
    }

    return generate_polyhedra_from_points(points);
}

Polyhedra identity(const Polyhedra& p)
{return p;}

Polyhedra dual_truncated(const Polyhedra& p)
{return truncated(dual(p));}

Polyhedra omnitruncated(const Polyhedra& p)
{return truncated(biseauted(p));}

Polyhedra softened(const Polyhedra& p)
{return softened_on_omni(omnitruncated(p));}


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

    generate_and_save("regular/cube");
    generate_and_save("regular/dodecahedron");
    generate_and_save("regular/icosahedron");
    generate_and_save("regular/tetrahedron");
    generate_and_save("regular/octahedron");

    return 0;
}
