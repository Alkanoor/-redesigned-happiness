#include "convex_polyhedra_from_points.hpp"


std::vector<std::string> get_paths(const std::string& directory)
{
    std::vector<std::string> ret;
    return ret;
}

std::vector<std::pair<Polyhedra,std::string> > create_stars(const Polyhedra& p)
{
    std::vector<std::pair<Polyhedra,std::string> > ret;
    return ret;
}

int main()
{
    std::vector<std::string> paths = get_paths("archimede_hashed");

    for(std::string path : paths)
    {
        Polyhedra p = from_file(path);
        std::string name = file(path).substr(0,s.find(":hashed"));

        std::vector<std::pair<Polyhedra,std::string> > shining = create_stars(p);

        for(auto it : shining)
            to_file(it->first,"shining/"+name+"-"+it->second);
    }
    return 0;
}
