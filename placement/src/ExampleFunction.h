#define _GLIBCXX_USE_CXX11_ABI 0

#ifndef EXAMPLEFUNCTION_H
#define EXAMPLEFUNCTION_H
#include "Placement.h"
#include "NumericalOptimizerInterface.h"
#include <vector>
#include <math.h>
//#include "GlobalPlacer.h"

class ExampleFunction : public NumericalOptimizerInterface
{
public:
    ExampleFunction(const vector<Module> &modules,
    const vector<Net> &nets,
	int N,
	int NN,
	double W,
	double H,
	double L,
	double B
    ):
	_modules (modules),
	_nets (nets),
	_N(N),
	_NN(NN),
	_W(W),
	_H(H),
	_L(L),
	_B(B)
	
	{}

    void evaluateFG(const vector<double> &x, double &f, vector<double> &g);
    void evaluateF(const vector<double> &x, double &f);
    unsigned dimension();
	void initialization();
	void LSE_F(const vector<double> &x, double &f);
	void LSE_FG(const vector<double> &x, double &f, vector<double> &g);
	void Denstiy_F(const vector<double> &x, double &f);
	void Denstiy_FG(const vector<double> &x, double &f,vector<double> &g);
	double sigmoidPrime(double a,double b, int i, bool o);
	double sigmoid(double x);
	void setlambda(double l){_lambda = l;}
	void setiniWire(double l){_iniWire = l;}
	void fastbuild(const vector<double> &x);
	void setisfirst(bool a){_isfirst = a;}
	void setDnorm(const vector<double> &x);

private:
	double      _lambda;
	int			_N;
	int			_NN;
	int			_binRow;
	int			_binNum;
	double		_W;
	double		_H;
	double		_A;
	double		_binW;
	double 		_binH;
	double		_alpha;
	double		_iniWire;
	double		_gamma;
	double		_Mb;
	double 		_L;
	double		_B;
	bool		_isfirst;
	double		_dnorm;
	
	vector < vector<double> >		_LSEcontainer;
	vector <double>				_density;
	vector <double>				_xd;
	vector <double>				_yd;
	vector <double>				_tF;
	vector < vector<double> >	_xcur;
	vector < vector<double> >	_ycur;
	
	vector<Module> _modules;
    vector<Net> _nets;


};
#endif // EXAMPLEFUNCTION_H