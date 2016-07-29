#ifndef GENERATE_ICOSAEDRON_HPP
#define GENERATE_ICOSAEDRON_HPP


#include <vector>
#include <array>
#include <cmath>


std::vector<std::array<double,3> > generate_icosahedron()
{
    std::vector<std::array<double,3> > vertices(12);

    double Pi = 3.141592653589793238462643383279502884197;

    double phiaa  = 26.56505;
    double r = 1.0;
    double phia = Pi*phiaa/180.0;
    double theb = Pi*36.0/180.0;
    double the72 = Pi*72.0/180;
    vertices[0][0]=0.0;
    vertices[0][1]=0.0;
    vertices[0][2]=r;
    vertices[11][0]=0.0;
    vertices[11][1]=0.0;
    vertices[11][2]=-r;
    double the = 0.0;
    for(int i=1; i<6; i++)
    {
        vertices[i][0]=r*cos(the)*cos(phia);
        vertices[i][1]=r*sin(the)*cos(phia);
        vertices[i][2]=r*sin(phia);
        the = the+the72;
    }
    the=theb;
    for(int i=6; i<11; i++)
    {
        vertices[i][0]=r*cos(the)*cos(-phia);
        vertices[i][1]=r*sin(the)*cos(-phia);
        vertices[i][2]=r*sin(-phia);
        the = the+the72;
    }

    return vertices;
}


#endif
