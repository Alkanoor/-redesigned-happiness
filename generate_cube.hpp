#ifndef GENERATE_CUBE_HPP
#define GENERATE_CUBE_HPP


#include <vector>
#include <array>
#include <cmath>


std::vector<std::array<double,3> > generate_cube()
{
    std::vector<std::array<double,3> > vertices(8);

    double Pi = 3.141592653589793238462643383279502884197;

    double phiaa = 35.264391;

    double r = 1.0;
    double phia = Pi*phiaa/180.0;
    double phib = -phia;
    double the90 = Pi*90.0/180.0;
    double the = 0.0;
    for(int i=0; i<4; i++)
    {
        vertices[i][0]=r*cos(the)*cos(phia);
        vertices[i][1]=r*sin(the)*cos(phia);
        vertices[i][2]=r*sin(phia);
        the = the+the90;
    }
    the=0.0;
    for(int i=4; i<8; i++)
    {
        vertices[i][0]=r*cos(the)*cos(phib);
        vertices[i][1]=r*sin(the)*cos(phib);
        vertices[i][2]=r*sin(phib);
        the = the+the90;
    }

    return vertices;
}


#endif
