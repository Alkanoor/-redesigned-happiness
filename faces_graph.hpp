#ifndef FACES_GRAPH_HPP
#define FACES_GRAPH_HPP


#include "util.hpp"

#include <set>


class Graph
{
    public:
        std::ostream& log(std::ostream& os) const;

        bool itere_on_neighbours(int index, int spaces, int origin, int cur_spaces = 0);

        bool colored_neighbourhood(int index, int spaces, int cur_spaces, std::set<int>& cur_marked);
        bool colored_neighbourhood(int index, int spaces);

        const std::map<unsigned int, int>& get_colors() const
        {return colors;}

        static bool make_graph(const std::vector<std::vector<int> >& faces, int index, int spaces, const std::map<int,std::set<int> >& neighbours, int face_origin, Graph& result);
        static std::vector<std::vector<Graph> > make_graphs(const std::vector<std::vector<int> >& faces, const std::vector<int>& index, int max_spaces, const std::map<int,std::set<int> >& neighbours);
        static std::vector<std::vector<Graph> > keep_essential(const std::vector<std::vector<Graph> >& origin, int to_keep=1);

        int hash() const
        {return n_colored+256*min_colored;}

    private:
        int n_colored;
        int min_colored;
        std::set<unsigned int> concerned;
        std::set<unsigned int> alone_faces;
        std::map<unsigned int, int> colors;
        std::map<unsigned int, std::vector<unsigned int> > neighbours;
};


#endif
