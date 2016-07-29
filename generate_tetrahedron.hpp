#ifndef GENERATE_TETRAHEDRON_HPP
#define GENERATE_TETRAHEDRON_HPP


#include <vector>
#include <array>
#include <cmath>


std::vector<std::array<double,3> > generate_tetrahedron()
{
    std::vector<std::array<double,3> > vertices(4);

    double Pi = 3.141592653589793238462643383279502884197;

    double phiaa  = -19.471220333;

    double r = 1.0;
    double phia = Pi*phiaa/180.0;
    double the120 = Pi*120.0/180.0;
    vertices[0][0] = 0.0;
    vertices[0][1] = 0.0;
    vertices[0][2] = r;
    double the = 0.0;
    for(int i=1; i<4; i++)
    {
        vertices[i][0]=r*cos(the)*cos(phia);
        vertices[i][1]=r*sin(the)*cos(phia);
        vertices[i][2]=r*sin(phia);
        the = the+the120;
    }

    return vertices;
}


#endif
