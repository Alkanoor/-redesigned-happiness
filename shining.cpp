#include "convex_polyhedra_from_points.hpp"
#include "polyhedron_topology.hpp"
#include "faces_graph.hpp"

#include <dirent.h>


std::vector<std::string> get_paths(const std::string& directory)
{
    std::vector<std::string> ret;

    DIR *dir;
    struct dirent *ent;
    if((dir = opendir (directory.c_str())) != NULL)
    {
        while((ent = readdir(dir)) != NULL)
            ret.push_back(std::string(ent->d_name));
        closedir(dir);
    }
    else
        std::cerr<<"Could not open dir "<<directory<<std::endl;

    return ret;
}


void itere(unsigned int cur_number_faces, const std::vector<bool>& use, const std::vector<std::vector<Graph> >& graphs,
           const std::vector<Point_3>& model_points, const Point_3& center, const std::vector<std::vector<int> >& model_faces,
           const std::vector<Point_3>& points, const std::vector<std::vector<int> >& faces, const std::vector<int>& index, std::vector<Polyhedra>& results, double ratio=2)
{
    if(cur_number_faces>=use.size())
    {
        results.push_back({points,faces});
        return;
    }

    if(use[cur_number_faces])
    {
        for(auto g : graphs[cur_number_faces])
        {
            std::vector<Point_3> tmp_points = points;
            if(!tmp_points.size())
                tmp_points = model_points;
            std::vector<std::vector<int> > tmp_faces = faces;
            std::map<unsigned int, int> colors = g.get_colors();
            for(auto it=colors.begin(); it!=colors.end(); it++)
            {
                if(it->second)
                {
                    unsigned int s = model_faces[it->first].size();
                    std::vector<int> face(3);
                    Vector_3 face_points(0,0,0);
                    for(unsigned int j=0; j<s; j++)
                        face_points = face_points+point_to_vec(model_points[model_faces[it->first][j]]);
                    face_points = face_points/(double)s;
                    Vector_3 direction = face_points-point_to_vec(center);
                    tmp_points.push_back(vec_to_point(point_to_vec(center)+direction*ratio));
                    for(unsigned int j=0; j<s; j++)
                    {
                        face[0] = model_faces[it->first][j];
                        face[1] = model_faces[it->first][(j+1)%s];
                        face[2] = tmp_points.size()-1;
                        tmp_faces.push_back(face);
                    }
                }
                else
                    tmp_faces.push_back(model_faces[it->first]);
            }
            itere(cur_number_faces+1, use, graphs, model_points, center, model_faces, tmp_points, tmp_faces, index, results);
        }
    }
    else
    {
        auto tmp_faces = faces;
        for(auto f : model_faces)
            if((int)f.size()==index[cur_number_faces])
                tmp_faces.push_back(f);

        itere(cur_number_faces+1, use, graphs, model_points, center, model_faces, points, tmp_faces, index, results);
    }
}


std::vector<std::pair<Polyhedra,std::string> > create_stars(const Polyhedra& p, int max_spaces = 5)
{
    std::map<int,int> n_of_index = faces_to_map(p.faces);
    int n = (int)n_of_index.size();
    std::vector<int> index(n);

    auto it = n_of_index.begin();
    for(int i=0; i<n; i++,it++)
        index[i] = it->first;

    Polyhedron_topology topo(p.faces);

    std::map<int,std::set<int> > faces_neighbours = topo.get_faces_neighbours();

    std::vector<std::vector<Graph> > graphs = Graph::keep_essential(Graph::make_graphs(p.faces,index,max_spaces,faces_neighbours));
    std::vector<std::pair<Polyhedra,std::string> > ret;
    std::vector<bool> use(n);

    for(int i=1;i<(1<<n);i++)
    {
        std::string base_hash;
        for(int j=0;j<n;j++)
            if(i&(1<<j))
            {
                use[j] = true;
                base_hash += '1';
            }
            else
            {
                use[j] = false;
                base_hash += '0';
            }

        std::vector<Polyhedra> polyhedrons;
        std::vector<Point_3> points_cpy = p.points;
        std::vector<std::vector<int> > index_cpy = p.faces;

        itere(0,use,graphs,points_cpy,vec_to_point(get_center(points_cpy)),index_cpy,
                std::vector<Point_3>(),std::vector<std::vector<int> >(),index,polyhedrons);

        for(unsigned int j=0;j<polyhedrons.size();j++)
        {
            std::ostringstream oss;
            oss<<j;
            ret.push_back(std::pair<Polyhedra,std::string>(polyhedrons[j],base_hash+"-"+oss.str()));
        }
    }

    return ret;
}

//3:20:4:30:5:12-101-0
int main()
{
    std::vector<std::string> paths = get_paths("archimede_hashed");

    for(std::string path : paths)
    {
        std::cout<<path<<std::endl;
        Polyhedra p = from_file("archimede_hashed/"+path);
        std::cout<<p<<std::endl;
        if(p.points.size())
        {
            std::string name = file(path).substr(0,path.find(".hashed"));

            std::vector<std::pair<Polyhedra,std::string> > shining = create_stars(p);

            for(auto it : shining)
                to_file_json(it.first,"shining/"+name+"-"+it.second+".json");
        }
    }
    return 0;
}
