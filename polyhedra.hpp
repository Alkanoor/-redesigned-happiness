#ifndef POLYHEDRA_HPP_INCLUDED
#define POLYHEDRA_HPP_INCLUDED


#include "polyhedron_utils.hpp"
#include "util.hpp"

#include <fstream>


struct Polyhedra
{
    std::vector<Point_3> points;
    std::vector<std::vector<int> > faces;
};

std::ostream& operator<< (std::ostream& os, const Polyhedra& p)
{
    print_endline(os,p.points);
    print_sep(os,p.faces,' ');
    return os;
}

void print_character(const Polyhedra& p)
{
    std::map<int,int> number_of_faces;
    for(unsigned int i=0;i<p.faces.size();i++)
    {
        if(number_of_faces.count(p.faces[i].size()))
            number_of_faces[p.faces[i].size()]++;
        else
            number_of_faces[p.faces[i].size()] = 1;
        for(unsigned int j=0;j<p.faces[i].size();j++)
            std::cout<<sqrt(CGAL::squared_distance(p.points[p.faces[i][j]],p.points[p.faces[i][(j+1)%p.faces[i].size()]]))<<std::endl;
    }
    for(auto it=number_of_faces.begin();it!=number_of_faces.end();it++)
        std::cout<<it->second<<" faces of "<<it->first<<" aretes"<<std::endl;
    std::cout<<std::endl;
}

Vector_3 get_center(const std::vector<Point_3>& points)
{
    Vector_3 center(0,0,0);
    for(unsigned int i=0;i<points.size();i++)
        center = center+(points[i]-Point_3(0,0,0));
    center = center/(double)points.size();
    return center;
}

Polyhedra normalize(const Polyhedra& p, double norm = 1)
{
    Vector_3 center = get_center(p.points);

    std::vector<Point_3> points = p.points;
    for(unsigned int i=0;i<points.size();i++)
    {
        Vector_3 p = point_to_vec(points[i])-center;
        p = p/sqrt(p*p);
        points[i] = vec_to_point(center+p);
    }

    return {points,p.faces};
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

std::map<int,int> faces_to_map(const std::vector<std::vector<int> >& index)
{
    std::map<int,int> corresp;

    for(unsigned int i=0; i<index.size(); i++)
        if(corresp.count(index[i].size()))
            corresp[index[i].size()]++;
        else
            corresp[index[i].size()] = 1;

    return corresp;
}

std::string create_hash(const std::vector<std::vector<int> >& index)
{
    std::string ret;
    std::map<int,int> corresp = faces_to_map(index);

    unsigned int i = 0;
    for(auto it=corresp.begin(); it!=corresp.end(); it++, i++)
    {
        std::ostringstream oss1;
        std::ostringstream oss2;
        oss1<<it->first;
        oss2<<it->second;
        if(i+1<corresp.size())
            ret += oss1.str()+"_"+oss2.str()+"_";
        else
            ret += oss1.str()+"_"+oss2.str();
    }

    return ret;
}

void to_file(const Polyhedra& p, const std::string& path)
{
    std::ofstream ofs(path,std::ios::out|std::ios::trunc);

    print_endline(ofs,p.points);
    ofs<<std::endl;
    print_sep(ofs,p.faces,' ');
}

void to_file_json(const Polyhedra& p, const std::string& path)
{
    std::ofstream ofs_json(path,std::ios::out|std::ios::trunc);
    ofs_json<<"{\"vertices\":[";
    for(unsigned int i=0; i<p.points.size(); i++)
        if(i+1<p.points.size())
            ofs_json<<p.points[i].x()<<","<<p.points[i].y()<<","<<p.points[i].z()<<",";
        else
            ofs_json<<p.points[i].x()<<","<<p.points[i].y()<<","<<p.points[i].z();
    ofs_json<<"],\"faces\":[";
    unsigned int cur = 0;
    for(auto f : p.faces)
    {
        ofs_json<<"[";
        for(unsigned int i=0; i<f.size(); i++)
            if(i+1<f.size())
                ofs_json<<f[i]<<",";
            else
                ofs_json<<f[i];
        if(cur+1<p.faces.size())
            ofs_json<<"],";
        else
            ofs_json<<"]";
        cur++;
    }
    ofs_json<<"]}";
}

void to_file_obj(const Polyhedra& p, const std::string& path)
{
    std::ofstream ofs_obj(path,std::ios::out|std::ios::trunc);
    ofs_obj<<"# Polyhedra created with love"<<std::endl<<std::endl;
    ofs_obj<<"g My_Stardust"<<std::endl<<std::endl;
    for(unsigned int i=0; i<p.points.size(); i++)
        ofs_obj<<"v "<<std::setprecision(7)<<p.points[i].x()<<" "<<p.points[i].y()<<" "<<p.points[i].z()<<std::endl;
    ofs_obj<<std::endl;
    for(auto f : p.faces)
    {
        ofs_obj<<"f ";
        for(unsigned int i=0; i<f.size(); i++)
            ofs_obj<<f[i]+1<<" ";
        ofs_obj<<std::endl;
    }
}

void to_file_model(const Polyhedra& p, const std::string& path)
{
    std::ofstream ofs_model(path,std::ios::out|std::ios::trunc|std::ios::binary);
    std::ifstream ifs_model("model.mqo",std::ios::out|std::ios::binary);

    bool ok = false;
    std::string tmp1, tmp2;
    while(std::getline(ifs_model,tmp1))
    {
        if(ok)
            ofs_model<<tmp2<<std::endl;
        tmp2 = tmp1;
        ok = true;
    }

    ofs_model<<"	vertex "<<p.points.size()<<" {"<<std::endl;
    for(unsigned int i=0; i<p.points.size(); i++)
        ofs_model<<"		"<<p.points[i].x()<<" "<<p.points[i].y()<<" "<<p.points[i].z()<<std::endl;
    ofs_model<<"	}"<<std::endl;
    ofs_model<<"	face "<<p.faces.size()<<" {"<<std::endl;

    unsigned int cur = 0;
    for(auto f : p.faces)
    {
        ofs_model<<"		"<<f.size()<<" V(";
        for(unsigned int i=0; i<f.size(); i++)
            if(i+1<f.size())
                ofs_model<<f[i]<<" ";
            else
                ofs_model<<f[i];
        ofs_model<<") M("<<cur<<")"<<std::endl;
        cur++;
    }
    ofs_model<<"	}"<<std::endl;
    ofs_model<<"}"<<std::endl<<"Eof";
}


#endif
