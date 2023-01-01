#define _GLIBCXX_USE_CXX11_ABI 0
#ifndef GLOBALPLACER_H
#define GLOBALPLACER_H

#include "Placement.h"
#include <cstdlib>
#include <cstdio>
#include <fstream>

class GlobalPlacer 
{
public:
    GlobalPlacer(Placement &placement);
	void place();
    void plotPlacementResult( const string outfilename, bool isPrompt = false );
	void virgin(vector<double> &x,double W, double H);
	double adjust( double pos, double U, double L, double length);
	void virgin2(vector<double> &x,double W, double H);
	

private:
    Placement& _placement;
    void plotBoxPLT( ofstream& stream, double x1, double y1, double x2, double y2 );
};

#endif // GLOBALPLACER_H
