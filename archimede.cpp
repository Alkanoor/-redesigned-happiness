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
    Polyhedra p = from_file(path);

    for(auto it=operations.begin(); it!=operations.end(); it++)
    {
        std::ofstream ofs("archimede/"+file(path)+"."+it->first,std::ios::out|std::ios::trunc);
        Polyhedra p2 = it->second(p);
        std::cout<<it->first<<std::endl;
        print_character(p2);
        print_endline(ofs,p2.points);
        ofs<<std::endl;
        print_sep(ofs,p2.faces,' ');

        std::ofstream ofs_json("archimede_json/"+file(path)+"."+it->first+".json",std::ios::out|std::ios::trunc);
        ofs_json<<"{\"vertices\":[";
        for(unsigned int i=0; i<p2.points.size(); i++)
            if(i+1<p2.points.size())
                ofs_json<<p2.points[i].x()<<","<<p2.points[i].y()<<","<<p2.points[i].z()<<",";
            else
                ofs_json<<p2.points[i].x()<<","<<p2.points[i].y()<<","<<p2.points[i].z();
        ofs_json<<"],\"faces\":[";
        unsigned int cur = 0;
        for(auto f : p2.faces)
        {
            ofs_json<<"[";
            for(unsigned int i=0; i<f.size(); i++)
                if(i+1<f.size())
                    ofs_json<<f[i]<<",";
                else
                    ofs_json<<f[i];
            if(cur+1<p2.faces.size())
                ofs_json<<"],";
            else
                ofs_json<<"]";
            cur++;
        }
        ofs_json<<"]}";
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

    double a = sqrt(CGAL::squared_distance(p.points[p.faces[0][0]],p.points[p.faces[0][1]]));

    for(unsigned int i=0;i<p.faces.size();i++)
    {
        unsigned int s = p.faces[i].size();
        double b = a/(2.0*(1.0+sin(M_PI*(0.5-1.0/(double)s))));
        for(unsigned int j=0;j<s;j++)
            if(!edges.count(p.faces[i][j])||edges[p.faces[i][j]].count(p.faces[i][(j+1)%s])==0)
            {
                edges[p.faces[i][j]].insert(p.faces[i][(j+1)%s]);
                edges[p.faces[i][(j+1)%s]].insert(p.faces[i][j]);
                Vector_3 cur = p.points[p.faces[i][(j+1)%s]]-p.points[p.faces[i][j]];
                cur = cur/sqrt(cur*cur);
                points.push_back(convert_to_array(p.points[p.faces[i][j]]+cur*b));
                points.push_back(convert_to_array(p.points[p.faces[i][j]]+cur*(a-b)));
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
            if(!edges.count(p.faces[i][j])||edges[p.faces[i][j]].count(p.faces[i][(j+1)%s])==0)
            {
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

    unsigned int tmp = p.faces[0].size();
    Vector_3 tmp_middle_face(0,0,0);
    for(unsigned int j=0;j<tmp;j++)
        tmp_middle_face = tmp_middle_face+(p.points[p.faces[0][j]]-Point_3(0,0,0));
    tmp_middle_face = tmp_middle_face/(double)tmp;

    double prev_a = CGAL::squared_distance(p.points[p.faces[0][0]],p.points[p.faces[0][1]]);
    double a = 0;

    Vector_3 tmp_middle_face_2(0,0,0);
    bool found = false;
    for(unsigned int i=1;i<p.faces.size();i++)
    {
        tmp_middle_face_2 = Vector_3(0,0,0);
        for(unsigned int j=0;j<p.faces[i].size();j++)
            if((p.faces[i][j]==p.faces[0][0]&&p.faces[i][(j+1)%p.faces[i].size()]==p.faces[0][1])
               ||(p.faces[i][j]==p.faces[0][1]&&p.faces[i][(j+1)%p.faces[i].size()]==p.faces[0][0]))
            {
                for(unsigned int k=0;k<p.faces[i].size();k++)
                    tmp_middle_face_2 = tmp_middle_face_2+(p.points[p.faces[i][k]]-Point_3(0,0,0));
                tmp_middle_face_2 = tmp_middle_face_2/(double)p.faces[i].size();

                double a_min = 0, a_max = 1000*prev_a;
                int n_max = 1000, n_iter = 0;
                while(!found&&n_iter<n_max)
                {
                    a = (a_min+a_max)/2.0;
                    Vector_3 normalized_dif_1 = tmp_middle_face-center;
                    normalized_dif_1 = normalized_dif_1/sqrt(normalized_dif_1*normalized_dif_1);
                    Vector_3 new_center_1 = center+normalized_dif_1*a;

                    Vector_3 normalized_dif_2 = tmp_middle_face_2-center;
                    normalized_dif_2 = normalized_dif_2/sqrt(normalized_dif_2*normalized_dif_2);
                    Vector_3 new_center_2 = center+normalized_dif_2*a;

                    Point_3 p_1 = vec_to_point(new_center_1+point_to_vec(p.points[p.faces[0][0]])-tmp_middle_face);
                    Point_3 p_2 = vec_to_point(new_center_2+point_to_vec(p.points[p.faces[0][0]])-tmp_middle_face_2);

                    double d = CGAL::squared_distance(p_1,p_2);

                    if(fabs(d-prev_a)<0.00001)
                    {
                        found = true;
                        break;
                    }
                    else if(d>prev_a)
                        a_max = a;
                    else
                        a_min = a;

                    n_iter++;
                }


                break;
            }
        if(found)
            break;
    }

    for(unsigned int i=0;i<p.faces.size();i++)
    {
        unsigned int s = p.faces[i].size();
        Vector_3 middle_face(0,0,0);
        for(unsigned int j=0;j<s;j++)
            middle_face = middle_face+(p.points[p.faces[i][j]]-Point_3(0,0,0));
        middle_face = middle_face/(double)s;
        Vector_3 normalized_dif = middle_face-center;
        normalized_dif = normalized_dif/sqrt(normalized_dif*normalized_dif);
        Vector_3 new_center = center+normalized_dif*a;

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
            }
            else
            {
                if(point_tags.count(p.faces[cur_face][j])&&point_tags[p.faces[cur_face][j]]!=1)
                {
                    std::cout<<"Should not happen in softening, face "<<cur_face<<std::endl;
                    exit(0);
                }
                point_tags[p.faces[cur_face][j]] = 1;
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
                    cur_face = i;
                    if(point_tags[p.faces[cur_face][found]]==0)
                        cur_modulo = found%2;
                    else
                        cur_modulo = 1-(found%2);

                    break;
                }
            }
    }

    std::cout<<"softened"<<std::endl;
    print_character(generate_polyhedra_from_points(points));
    return generate_polyhedra_from_points(points);
}

Polyhedra identity(const Polyhedra& p)
{return p;}

Polyhedra dual_truncated(const Polyhedra& p)
{return truncated(dual(p));}

Polyhedra omnitruncated(const Polyhedra& p)
{return biseauted(truncated(p));}

Polyhedra softened(const Polyhedra& p)
{return softened_on_omni(omnitruncated(p));}


int main()
{
    //operations["identity"] = std::bind(&identity,std::placeholders::_1);
    //operations["dual"] = std::bind(&dual,std::placeholders::_1);
    operations["truncated"] = std::bind(&truncated,std::placeholders::_1);
    //operations["rectified"] = std::bind(&rectified,std::placeholders::_1);
    //operations["biseauted"] = std::bind(&biseauted,std::placeholders::_1);
    operations["omnitruncated"] = std::bind(&omnitruncated,std::placeholders::_1);
    //operations["softened"] = std::bind(&softened,std::placeholders::_1);
    operations["dual_truncated"] = std::bind(&dual_truncated,std::placeholders::_1);

    std::cout<<"======CUBE======"<<std::endl;
    generate_and_save("regular/cube");
    std::cout<<"======DODECA======"<<std::endl;
    generate_and_save("regular/dodecahedron");
    std::cout<<"======ICOSA======"<<std::endl;
    generate_and_save("regular/icosahedron");
    std::cout<<"======TETRA======"<<std::endl;
    generate_and_save("regular/tetrahedron");
    std::cout<<"======OCTA======"<<std::endl;
    generate_and_save("regular/octahedron");

    return 0;
}
