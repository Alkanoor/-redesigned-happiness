#ifndef GENERATE_DODECAHEDRON_HPP
#define GENERATE_DODECAHEDRON_HPP


#include <vector>
#include <array>
#include <cmath>


std::vector<std::array<double,3> > generate_dodecahedron()
{
    std::vector<std::array<double,3> > vertices(20);

    double Pi = 3.141592653589793238462643383279502884197;

    double phiaa = 52.62263590;
    double phibb = 10.81231754;

    double r = 1.0;
    double phia = Pi*phiaa/180.0;
    double phib = Pi*phibb/180.0;
    double phic = Pi*(-phibb)/180.0;
    double phid = Pi*(-phiaa)/180.0;
    double the72 = Pi*72.0/180;
    double theb = the72/2.0;
    double the = 0.0;

    for(int i=0; i<5; i++)
    {
        vertices[i][0]=r*cos(the)*cos(phia);
        vertices[i][1]=r*sin(the)*cos(phia);
        vertices[i][2]=r*sin(phia);
        the = the+the72;
    }
    the=0.0;
    for(int i=5; i<10; i++)
    {
        vertices[i][0]=r*cos(the)*cos(phib);
        vertices[i][1]=r*sin(the)*cos(phib);
        vertices[i][2]=r*sin(phib);
        the = the+the72;
    }
    the = theb;
    for(int i=10; i<15; i++)
    {
        vertices[i][0]=r*cos(the)*cos(phic);
        vertices[i][1]=r*sin(the)*cos(phic);
        vertices[i][2]=r*sin(phic);
        the = the+the72;
    }
    the=theb;
    for(int i=15; i<20; i++)
    {
        vertices[i][0]=r*cos(the)*cos(phid);
        vertices[i][1]=r*sin(the)*cos(phid);
        vertices[i][2]=r*sin(phid);
        the = the+the72;
    }

    return vertices;
}


#endif
