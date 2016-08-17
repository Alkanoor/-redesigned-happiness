#include "faces_graph.hpp"

#include <iostream>


std::ostream& Graph::log(std::ostream& os) const
{
    os<<std::endl<<"==================="<<std::endl;
    os<<n_colored<<" colored !"<<std::endl<<std::endl;
    for(auto it : concerned)
    {
        os<<it<<" => ";
        print_sep(os,neighbours.find(it)->second,' ');
        os<<std::endl;
    }
    os<<std::endl;
    for(auto it : concerned)
        os<<it<<" => "<<colors.find(it)->second<<std::endl;
    return os;
}

bool Graph::colored_neighbourhood(int index, int spaces, int cur_spaces, std::set<int>& cur_marked)
{
    if(cur_spaces>=spaces)
        return false;

    for(int n : neighbours[index])
        if(!cur_marked.count(n))
        {
            cur_marked.insert(n);
            if(concerned.count(n))
            {
                if(colors.count(n)&&colors[n])
                    return true;
                if(colored_neighbourhood(n,spaces,cur_spaces+1,cur_marked))
                    return true;
            }
        }

    return false;
}

bool Graph::colored_neighbourhood(int index, int spaces)
{
    std::set<int> cur_marked;
    cur_marked.insert(index);
    return colored_neighbourhood(index,spaces,0,cur_marked);
}

bool Graph::itere_on_neighbours(int index, int spaces, int origin, int cur_spaces)
{
    if(cur_spaces>=spaces)
        return true;

    for(int n : neighbours[index])
        if(concerned.count(n))
        {
            if(n!=origin&&colors.count(n)&&colors[n])
                return false;
            else
            {
                if(!colors.count(n))
                    colors[n] = 0;
                if(!itere_on_neighbours(n,spaces,origin,cur_spaces+1))
                    return false;
            }
        }

    return true;
}

bool Graph::make_graph(const std::vector<std::vector<int> >& faces, int index, int spaces, const std::map<int,std::set<int> >& extern_neighbours, int face_origin, Graph& result)
{
    result.n_colored = 0;
    result.min_colored = -1;
    for(unsigned int i=0; i<faces.size(); i++)
        if((int)faces[i].size()==index)
        {
            result.concerned.insert(i);
            for(auto j : extern_neighbours.at(i))
                result.neighbours[i].push_back(j);
        }

    for(unsigned int i=0; i<faces.size(); i++)
        if((int)faces[i].size()==index)
        {
            bool alone = true;
            for(auto j : extern_neighbours.at(i))
                if(result.concerned.count(j))
                    alone = false;
            if(alone)
                result.alone_faces.insert(i);
        }

    bool found = true;
    while(found)
    {
        found = false;
        for(int i : result.concerned)
            if(!result.colors.count(i)&&(result.alone_faces.count(i)||((result.colored_neighbourhood(i,spaces+1)||(!result.colors.size()&&i==face_origin)))))
            {
                found = true;
                result.colors[i] = 1;
                if(i<result.min_colored||result.min_colored<0)
                    result.min_colored = i;

                result.n_colored++;
                if(!result.itere_on_neighbours(i,spaces,i))
                    return false;
            }
    }

    for(unsigned int i : result.concerned)
        if((result.alone_faces.count(i)&&spaces)||(!result.alone_faces.count(i)&&!result.colored_neighbourhood(i,spaces+1)))
            return false;

    return true;
}


std::vector<std::vector<Graph> > Graph::make_graphs(const std::vector<std::vector<int> >& faces, const std::vector<int>& index, int max_spaces, const std::map<int,std::set<int> >& neighbours)
{
    std::vector<std::vector<Graph> > ret(index.size());

    for(unsigned int i=0; i<index.size(); i++)
    {
        std::set<int> hashes;
        for(int j=0; j<max_spaces; j++)
            for(unsigned int k=0; k<faces.size(); k++)
                if((int)faces[k].size()==index[i])
                {
                    Graph tmp;
                    if(Graph::make_graph(faces,index[i],j,neighbours,k,tmp))
                        if(!hashes.count(tmp.hash()))
                        {
                            ret[i].push_back(tmp);
                            hashes.insert(tmp.hash());
                        }
                }
    }

    return ret;
}

std::vector<std::vector<Graph> > Graph::keep_essential(const std::vector<std::vector<Graph> >& origin, int to_keep)
{
    std::vector<std::vector<Graph> > ret(origin.size());
    for(unsigned int i=0; i<origin.size(); i++)
    {
        std::map<int,int> hashes;
        for(auto g : origin[i])
        {
            if(hashes.count(g.hash()&255)&&hashes[g.hash()&255]<to_keep)
            {
                hashes[g.hash()&255]++;
                ret[i].push_back(g);
            }
            else if(!hashes.count(g.hash()&255))
            {
                hashes[g.hash()&255] = 1;
                ret[i].push_back(g);
            }
        }
    }

    return ret;
}
