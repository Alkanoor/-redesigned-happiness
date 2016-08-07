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
        Polyhedra p2 = it->second(p);
        to_file(p2,"archimede/"+file(path)+"."+it->first);
        std::cout<<it->first<<std::endl;
        print_character(p2);

        to_file_json(p2,"archimede_json/"+file(path)+"."+it->first+".json");
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

Polyhedra biseauted_chosen_faces(const Polyhedra& p, const std::vector<int>& good_faces)
{
    std::vector<std::array<double,3> > points;

    Vector_3 center(0,0,0);
    for(unsigned int i=0;i<p.points.size();i++)
        center = center+(p.points[i]-Point_3(0,0,0));
    center = center/(double)p.points.size();

    unsigned int tmp = p.faces[good_faces[0]].size();
    Vector_3 tmp_middle_face(0,0,0);
    for(unsigned int j=0;j<tmp;j++)
        tmp_middle_face = tmp_middle_face+(p.points[p.faces[good_faces[0]][j]]-Point_3(0,0,0));
    tmp_middle_face = tmp_middle_face/(double)tmp;

    double prev_a = CGAL::squared_distance(p.points[p.faces[good_faces[0]][0]],p.points[p.faces[good_faces[0]][1]]);
    double a = 0;

    Vector_3 tmp_middle_face_2(0,0,0);
    bool found = false;
    for(unsigned int i=1;i<good_faces.size();i++)
    {
        tmp_middle_face_2 = Vector_3(0,0,0);
        for(unsigned int j=0;j<p.faces[good_faces[i]].size()&&!found;j++)
            for(unsigned int k=0;k<p.faces[good_faces[0]].size()&&!found;k++)
                if((p.faces[good_faces[i]][j]==p.faces[good_faces[0]][k]&&p.faces[good_faces[i]][(j+1)%p.faces[good_faces[i]].size()]==p.faces[good_faces[0]][(k+1)%p.faces[good_faces[0]].size()])
                   ||(p.faces[good_faces[i]][j]==p.faces[good_faces[0]][(k+1)%p.faces[good_faces[0]].size()]&&p.faces[good_faces[i]][(j+1)%p.faces[good_faces[i]].size()]==p.faces[good_faces[0]][k]))
                {
                    for(unsigned int k=0;k<p.faces[good_faces[i]].size();k++)
                        tmp_middle_face_2 = tmp_middle_face_2+(p.points[p.faces[good_faces[i]][k]]-Point_3(0,0,0));
                    tmp_middle_face_2 = tmp_middle_face_2/(double)p.faces[good_faces[i]].size();

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

                        Point_3 p_1 = vec_to_point(new_center_1+point_to_vec(p.points[p.faces[good_faces[0]][0]])-tmp_middle_face);
                        Point_3 p_2 = vec_to_point(new_center_2+point_to_vec(p.points[p.faces[good_faces[0]][0]])-tmp_middle_face_2);

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

    for(unsigned int i=0;i<good_faces.size();i++)
    {
        unsigned int s = p.faces[good_faces[i]].size();
        Vector_3 middle_face(0,0,0);
        for(unsigned int j=0;j<s;j++)
            middle_face = middle_face+(p.points[p.faces[good_faces[i]][j]]-Point_3(0,0,0));
        middle_face = middle_face/(double)s;
        Vector_3 normalized_dif = middle_face-center;
        normalized_dif = normalized_dif/sqrt(normalized_dif*normalized_dif);
        Vector_3 new_center = center+normalized_dif*a;

        for(unsigned int j=0;j<s;j++)
        {
            Vector_3 dif = point_to_vec(p.points[p.faces[good_faces[i]][j]])-middle_face;
            points.push_back(convert_to_array(new_center+dif));
        }
    }

    return generate_polyhedra_from_points(points);
}

Polyhedra biseauted(const Polyhedra& p)
{
    std::vector<int> good_faces(p.faces.size());
    for(unsigned int i=0;i<good_faces.size();i++)
        good_faces[i] = i;

    return biseauted_chosen_faces(p,good_faces);
}

std::vector<int> matching(const Polyhedra& p1, const Polyhedra& p2)
{
    std::vector<int> ret;

    std::vector<Vector_3> middles_p1(p1.faces.size());
    for(unsigned int i=0;i<p1.faces.size();i++)
    {
        unsigned int s = p1.faces[i].size();
        Vector_3 middle_face(0,0,0);
        for(unsigned int j=0;j<s;j++)
            middle_face = middle_face+(p1.points[p1.faces[i][j]]-Point_3(0,0,0));
        middles_p1[i] = middle_face/(double)s;
    }

    for(unsigned int i=0;i<p2.faces.size();i++)
    {
        unsigned int s = p2.faces[i].size();
        Vector_3 middle_face(0,0,0);
        for(unsigned int j=0;j<s;j++)
            middle_face = middle_face+(p2.points[p2.faces[i][j]]-Point_3(0,0,0));
        middle_face = middle_face/(double)s;

        for(auto j : middles_p1)
            if(CGAL::squared_distance(vec_to_point(middle_face),vec_to_point(j))<0.000001)
            {
                ret.push_back(i);
                break;
            }
    }

    return ret;
}

Polyhedra identity(const Polyhedra& p)
{return p;}

Polyhedra dual_truncated(const Polyhedra& p)
{return truncated(dual(p));}

Polyhedra omnitruncated(const Polyhedra& p)
{
    Polyhedra truncated_p = truncated(p);
    return biseauted_chosen_faces(truncated_p,matching(p,truncated_p));
}

double measure(double a, double b, double c)
{
    return (fabs(a-b)+fabs(a-c)+fabs(b-c))/(a+b+c);
}

std::pair<double,double> get_2_minorants(const std::vector<double>& d)
{
    std::pair<double,double> ret;
    ret.first = d[0];
    ret.second = d[1];
    if(ret.first>ret.second)
    {
        ret.first = d[1];
        ret.second = d[0];
    }

    for(unsigned int i=2;i<d.size();i++)
        if(d[i]<ret.first)
        {
            ret.second = ret.first;
            ret.first = d[i];
        }
        else if(d[i]<ret.second)
            ret.second = d[i];

    return ret;
}

std::pair<double,double> eval(double theta, double s, const Vector_3& middle1, const Vector_3& middle2, const Vector_3& v1_x, const Vector_3& v1_y, const Vector_3& v2_x, const Vector_3& v2_y, double center_angle)
{
    Point_3 t1 = vec_to_point(middle1+s*cos(theta)*v1_x+s*sin(theta)*v1_y);
    Point_3 t2 = vec_to_point(middle1+s*cos(theta-center_angle)*v1_x+s*sin(theta-center_angle)*v1_y);
    Point_3 t3 = vec_to_point(middle1+s*cos(theta+center_angle)*v1_x+s*sin(theta+center_angle)*v1_y);
    Point_3 t4 = vec_to_point(middle2+s*cos(-theta+center_angle)*v2_x+s*sin(-theta+center_angle)*v2_y);

    std::vector<double> d;
    d.push_back(sqrt(CGAL::squared_distance(t1,t4)));
    d.push_back(sqrt(CGAL::squared_distance(t2,t4)));
    d.push_back(sqrt(CGAL::squared_distance(t3,t4)));

    return get_2_minorants(d);
}

Point_3 transform(double theta, double s, const Vector_3& middle, const Vector_3& v_x, const Vector_3& v_y)
{
    return vec_to_point(middle+s*cos(theta)*v_x+s*sin(theta)*v_y);
}

std::pair<double,double> find_best_theta_grid(double begin_theta, double end_theta, double begin_s, double end_s, double prec, int subdivides, double& s, double& theta, const Vector_3& middle1, const Vector_3& middle2, const Vector_3& v1_x, const Vector_3& v1_y, const Vector_3& v2_x, const Vector_3& v2_y, double center_angle, double a)
{
    if(fabs(end_theta-begin_theta)<prec)
        return eval((begin_theta+end_theta)/2.0,(begin_s+end_s)/2.0,middle1,middle2,v1_x,v1_y,v2_x,v2_y,center_angle);

    double mini = -1;
    for(double s2=begin_s;s2<=end_s;s2+=(end_s-begin_s)/(double)subdivides)
        for(double theta2=begin_theta;theta2<=end_theta;theta2+=(end_theta-begin_theta)/(double)subdivides)
        {
            std::pair<double,double> tmp = eval(theta2, s2, middle1, middle2, v1_x, v1_y, v2_x, v2_y, center_angle);
            double m = measure(tmp.first, tmp.second, s2*a);
            if(m<mini||mini<0)
            {
                mini = m;
                s = s2;
                theta = theta2;
            }
        }

    double a1 = theta-(end_theta-begin_theta)/(double)subdivides;
    double a2 = theta+(end_theta-begin_theta)/(double)subdivides;
    double b1 = s-(end_s-begin_s)/(double)subdivides;
    double b2 = s+(end_s-begin_s)/(double)subdivides;
    if(a1<0)
        a1 = 0;
    if(a2>center_angle)
        a2 = center_angle;
    if(b1<0.1)
        b1 = 0.1;
    if(b2>1)
        b2 = 1;
    return find_best_theta_grid(a1,a2,b1,b2,prec,subdivides,
                                s,theta,middle1,middle2,v1_x,v1_y,v2_x,v2_y,center_angle,a);
}


Polyhedra softened(const Polyhedra& p)
{
    std::vector<std::array<double,3> > points;

    Vector_3 center(0,0,0);
    for(unsigned int i=0;i<p.points.size();i++)
        center = center+(p.points[i]-Point_3(0,0,0));
    center = center/(double)p.points.size();

    Vector_3 middle1(0,0,0), middle2(0,0,0);
    Vector_3 v1_x(0,0,0), v1_y(0,0,0), v2_x(0,0,0), v2_y(0,0,0);

    double a = sqrt(CGAL::squared_distance(p.points[p.faces[0][0]],p.points[p.faces[0][1]]));
    double center_angle = 2.0*M_PI/(double)p.faces[0].size();

    Vector_3 tmp_middle_face_2(0,0,0);
    bool found = false;
    for(unsigned int i=1;i<p.faces.size()&&!found;i++)
    {
        middle2 = Vector_3(0,0,0);
        for(unsigned int j=0;j<p.faces[i].size()&&!found;j++)
            if((p.faces[i][j]==p.faces[0][0]&&p.faces[i][(j+1)%p.faces[i].size()]==p.faces[0][1])
               ||(p.faces[i][j]==p.faces[0][1]&&p.faces[i][(j+1)%p.faces[i].size()]==p.faces[0][0]))
            {
                for(unsigned int l=0;l<p.faces[0].size();l++)
                    middle1 = middle1+(p.points[p.faces[0][l]]-Point_3(0,0,0));
                middle1 = middle1/(double)p.faces[0].size();

                for(unsigned int l=0;l<p.faces[i].size();l++)
                    middle2 = middle2+(p.points[p.faces[i][l]]-Point_3(0,0,0));
                middle2 = middle2/(double)p.faces[i].size();

                v1_x = point_to_vec(p.points[p.faces[0][0]])-middle1;
                v1_y = point_to_vec(p.points[p.faces[0][1]])-middle1;
                v2_x = point_to_vec(p.points[p.faces[0][0]])-middle2;
                v2_y = point_to_vec(p.points[p.faces[0][1]])-middle2;
                v1_y = v1_y-(v1_x*v1_y)*v1_x/(v1_x*v1_x);
                v1_y = v1_y/sqrt((v1_y*v1_y)/(v1_x*v1_x));
                v2_y = v2_y-(v2_x*v2_y)*v2_x/(v2_x*v2_x);
                v2_y = v2_y/sqrt((v2_y*v2_y)/(v2_x*v2_x));

                found = true;
            }
    }

    double s = 0, theta = 0;
    find_best_theta_grid(0,center_angle,0.1,0.9,0.00001,10,s,theta,middle1,middle2,v1_x,v1_y,v2_x,v2_y,center_angle,a);

    for(unsigned int i=0;i<p.faces.size();i++)
    {
        Vector_3 middle(0,0,0);
        for(unsigned int l=0;l<p.faces[i].size();l++)
            middle = middle+(p.points[p.faces[i][l]]-Point_3(0,0,0));
        middle = middle/(double)p.faces[i].size();

        for(unsigned int l=0;l<p.faces[i].size();l++)
        {
            v1_x = point_to_vec(p.points[p.faces[i][l]])-middle;
            v1_y = point_to_vec(p.points[p.faces[i][(l+1)%p.faces[i].size()]])-middle;
            v1_y = v1_y-(v1_x*v1_y)*v1_x/(v1_x*v1_x);
            v1_y = v1_y/sqrt((v1_y*v1_y)/(v1_x*v1_x));

            points.push_back(convert_to_array(transform(theta,s,middle,v1_x,v1_y)));
        }
    }

    return generate_polyhedra_from_points(points);
}


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
