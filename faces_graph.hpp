#ifndef FACES_GRAPH_HPP
#define FACES_GRAPH_HPP


#include "util.hpp"

#include <set>


class Graph
{
    public:
        Graph() : n_colored(0) {}

        std::ostream& log(std::ostream& os) const;

        bool itere_on_neighbours(int index, int spaces, int origin, int cur_spaces = 0);

        static bool make_graph(const std::vector<std::vector<int> >& faces, int index, int spaces, const std::map<int,std::set<int> >& neighbours, Graph& result);
        static std::vector<std::vector<Graph> > make_graphs(const std::vector<std::vector<int> >& faces, const std::vector<int>& index, int max_spaces, const std::map<int,std::set<int> >& neighbours);

        int hash() const
        {return n_colored;}

    private:
        int n_colored;
        std::set<unsigned int> concerned;
        std::map<unsigned int,int> colors;
        std::map<unsigned int, std::vector<unsigned int> > neighbours;
};


#endif
