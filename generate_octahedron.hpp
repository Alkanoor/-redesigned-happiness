#ifndef GENERATE_OCTAAHEDRON_HPP
#define GENERATE_OCTAAHEDRON_HPP


#include <vector>
#include <array>
#include <cmath>


std::vector<std::array<double,3> > generate_octahedron()
{
    std::vector<std::array<double,3> > vertices(6);

    double Pi = 3.141592653589793238462643383279502884197;

    double phiaa  = 0.0;
    double r = 1.0;
    double phia = Pi*phiaa/180.0;
    double the90 = Pi*90.0/180;
    vertices[0][0]=0.0;
    vertices[0][1]=0.0;
    vertices[0][2]=r;

    vertices[5][0]=0.0;
    vertices[5][1]=0.0;
    vertices[5][2]=-r;

    double the = 0.0;
    for(int i=1; i<5; i++)
    {
        vertices[i][0]=r*cos(the)*cos(phia);
        vertices[i][1]=r*sin(the)*cos(phia);
        vertices[i][2]=r*sin(phia);
        the = the+the90;
    }

    return vertices;
}


#endif
