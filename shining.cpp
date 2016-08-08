#include "convex_polyhedra_from_points.hpp"

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

std::vector<std::pair<Polyhedra,std::string> > create_stars(const Polyhedra& p, int max_spaces = 5)
{
    std::map<int,int> index = faces_to_map(p.faces);
    int n = (int)index.size();

    std::vector<std::pair<Polyhedra,std::string> > ret;
    std::vector<std::vector<Graph> > graphs(n);
    std::vector<bool> use(n);

    for(int i=1;i<(1<<n);i++)
    {
        std::string base_hash;
        for(int j=0;j<n;j++)
            if(i&(1<<j))
            {
                use[j] = true;
                graphs[j] = make_graphs(p.faces,j,ok,max_spaces);
                base_hash += '1';
            }
            else
            {
                use[j] = false;
                base_hash += '0';
            }

        std::vector<Polyhedra> polyhedrons;
        std::vector<Point_3> points_cpy;
        std::vector<std::vector<int> > index_cpy;
        itere(0,use,graphs,points_cpy,index_cpy,polyhedrons);

        for(unsigned int j=0;j<polyhedrons.size();j++)
        {
            std::ostringstream oss;
            oss<<j;
            ret.push_back(std::pair<Polyhedra,std::string>(polyhedrons[j],base_hash+"-"+oss.str()));
        }
    }

    return ret;
}

int main()
{
    std::vector<std::string> paths = get_paths("archimede_hashed");

    for(std::string path : paths)
    {
        std::cout<<path<<std::endl;
        Polyhedra p = from_file("archimede_hashed/"+path);
        std::cout<<p<<std::endl;
        std::string name = file(path).substr(0,path.find(".hashed"));

        std::vector<std::pair<Polyhedra,std::string> > shining = create_stars(p);

        for(auto it : shining)
            to_file(it.first,"shining/"+name+"-"+it.second);
    }
    return 0;
}
