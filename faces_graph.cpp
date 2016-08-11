#include "faces_graph.hpp"

#include <iostream>


std::ostream& Graph::log(std::ostream& os) const
{
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
                colors[n] = 0;
                if(!itere_on_neighbours(n,spaces,cur_spaces+1,origin))
                    return false;
            }
        }

    return true;
}

bool Graph::make_graph(const std::vector<std::vector<int> >& faces, int index, int spaces, const std::map<int,std::set<int> >& extern_neighbours, Graph& result)
{
    result.n_colored = 0;
    for(unsigned int i=0; i<faces.size(); i++)
        if((int)faces[i].size()==index)
        {
            result.concerned.insert(i);
            for(auto j : extern_neighbours.at(i))
                result.neighbours[i].push_back(j);
        }

    for(unsigned int i : result.concerned)
        if(!result.colors.count(i))
        {
            result.colors[i] = 1;
            std::cout<<"n blackened = "<<i<<std::endl;
            result.n_colored++;
            if(!result.itere_on_neighbours(i,spaces,i))
                return false;
        }

    return true;
}


std::vector<std::vector<Graph> > Graph::make_graphs(const std::vector<std::vector<int> >& faces, const std::vector<int>& index, int max_spaces, const std::map<int,std::set<int> >& neighbours)
{
    std::vector<std::vector<Graph> > ret(index.size());

    for(unsigned int i=0; i<index.size(); i++)
    {
        std::set<int> hashes;
        for(int j=0; j<max_spaces; j++)
        {
            Graph tmp;
            if(Graph::make_graph(faces,index[i],j,neighbours,tmp))
            {
                if(!hashes.count(tmp.hash()))
                {
                    ret[i].push_back(tmp);
                    hashes.insert(tmp.hash());
                    std::cout<<"Graph added :"<<std::endl;
                    tmp.log(std::cout);
                }
                else
                    std::cout<<"There is already a "<<j<<" for n_aretes = "<<index[i]<<" available"<<std::endl;
            }
            else
                std::cout<<"Problem detected for "<<j<<" for n_aretes = "<<index[i]<<std::endl;
        }
    }

    return ret;
}
