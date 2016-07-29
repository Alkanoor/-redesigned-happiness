#ifndef UTIL_HPP_INCLUDED
#define UTIL_HPP_INCLUDED


//pour afficher facilement des vecteurs avec retour à la ligne
template <typename T>
std::ostream& print_endline(std::ostream& ofs, const std::vector<T>& v)
{
	for(unsigned int i=0;i<v.size();i++)
        ofs<<v[i]<<std::endl;
	return ofs;
}

//pour afficher facilement des vecteurs avec séparateur choisi
template <typename T, typename U>
std::ostream& print_sep(std::ostream& ofs, const std::vector<T>& v, U sep)
{
	for(unsigned int i=0;i<v.size();i++)
        ofs<<v[i]<<sep;
	return ofs;
}

//pour afficher facilement des vecteurs avec séparateur choisi
template <typename T, typename U>
std::ostream& print_sep(std::ostream& ofs, const std::vector<std::vector<T> >& v, U sep)
{
	for(unsigned int i=0;i<v.size();i++)
        print_sep(ofs,v[i],sep)<<std::endl;
	return ofs;
}

//pour afficher facilement des vecteurs
template <typename T>
std::ostream& operator<<(std::ostream& ofs, const std::vector<T>& v)
{
	for(unsigned int i=0;i<v.size();i++)
        ofs<<v[i];
    ofs<<std::endl;
	return ofs;
}

//pour afficher facilement des map de pair
template <typename T, typename U, typename V>
std::ostream& operator<<(std::ostream& ofs, const std::map<T,std::pair<U,V> >& v)
{
	for(auto it=v.begin(); it!=v.end(); it++)
        ofs<<it->first<<" : ("<<it->second.first<<" "<<it->second.second<<")"<<std::endl;
	return ofs;
}

//pour afficher facilement des map
template <typename T, typename U>
std::ostream& operator<<(std::ostream& ofs, const std::map<T,U>& v)
{
	for(auto it=v.begin(); it!=v.end(); it++)
        ofs<<it->first<<" : "<<it->second<<std::endl;
	return ofs;
}

template <typename T, typename U>
std::map<U,std::vector<T> > reverse_map(const std::map<T,U>& m)
{
    std::map<U,std::vector<T> > ret;
    for(auto it=m.begin(); it!=m.end(); it++)
        ret[it->second].push_back(it->first);
    return ret;
}

//pour changer l'ordre des éléments dans une map
template <typename T, typename U>
std::map<T,U> oppose_in_map(const std::map<T,U>& v)
{
    std::map<T,U> cpy;
    for(auto it=v.begin(); it!=v.end(); it++)
        cpy[it->first] = -it->second;

	return cpy;
}

//pour afficher facilement des map de manière triée
template <typename T, typename U>
std::ostream& print_map_sorted(std::ostream& ofs, const std::map<T,U>& v, int number_elements = -1, bool oppose = false)
{
    std::map<U,std::vector<T> > reversed_map;
    if(oppose)
        reversed_map = reverse_map(oppose_in_map(v));
    else
        reversed_map = reverse_map(v);
    std::map<T,U> sorted_map;
    int i=0;
    for(auto it=reversed_map.begin(); it!=reversed_map.end() && (i<number_elements||number_elements<0); it++)
    {
        for(auto j : it->second)
            if(oppose)
                sorted_map[j] = -it->first;
            else
                sorted_map[j] = it->first;
        i += it->second.size();
    }
	ofs<<sorted_map;
	return ofs;
}

//pour convertir des vecteurs en string (ne présente pas un intérêt fou mais est utilisé dans le cpp associé)
template <typename T>
void to_string(const std::vector<T>& v, std::string& ret)
{
    ret.resize(v.size());
    for(unsigned int i=0;i<v.size();i++)
        ret[i] = (char)(v[i]);
}


#endif
