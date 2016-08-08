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

std::vector<std::pair<Polyhedra,std::string> > create_stars(const Polyhedra& p)
{
    std::map<int,int> index = faces_to_map(p);
    
    std::vector<std::pair<Polyhedra,std::string> > ret;
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
