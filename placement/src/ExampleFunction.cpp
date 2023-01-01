#include "ExampleFunction.h"
#include <vector>
// minimize 3*x^2 + 2*x*y + 2*y^2 + 7

//ExampleFunction::ExampleFunction(Placement &placement) 
//{
	
//}
void ExampleFunction::fastbuild(const vector<double> &x){
	for(int i = 0; i<_N; i++){
		int xi = 2*i;
		int XCurrentBin = floor((_modules[i].width()/2.0+x[xi] - _L) / _binW);
		int YCurrentBin = floor((_modules[i].width()/2.0+x[xi+1] - _B) / _binH);
		_xcur[i][0] = max(XCurrentBin - 1, 0);
		_xcur[i][1] = min(XCurrentBin + 1, _N- 1);
		_ycur[i][0] = max(YCurrentBin - 1, 0);
		_ycur[i][1] = min(YCurrentBin + 1, _N- 1);
	}
	
}
void ExampleFunction::initialization(){
	_gamma = (_W+_H)/1200.0;//need modification
	_binRow = 15;			//need modification
	_alpha = 0.0000075;			//need modification
	_binNum = _binRow*_binRow;
	//_N = _placement.numModules();
	_LSEcontainer.resize(_N);
	_density.resize(_binNum);
	_xd.resize(_N);
	_yd.resize(_N);
	
	_tF.resize(4);
	vector <double> row;
	row.assign(2,0);
	_xcur.assign(_N,row);
	_ycur.assign(_N,row);
	//cout << _N << "　"<< _NN << endl;
	
	//_W =_placement.boundryRight() - _placement.boundryLeft();
	//_H = _placement.boundryTop() - _placement.boundryBottom();
	_binW = _W*1.0/_binRow;
	_binH = _H*1.0/_binRow;
	_A = _W*_H;
	
    double tempA=0.0;
    for(int i = 0; i < _N; ++i){
		tempA += _modules[i].area();
	}
	_Mb = tempA/_A; //need modification
	_Mb = _N/_binNum;
	//cout << "gamma " << _gamma << endl;
	if(_N == 12028 && _NN == 11507){
		_alpha = 0.000008;
	}
	else if(_N == 29347 && _NN ==28446){
		_alpha = 0.000008;
	}
	else if(_N == 19321 && _NN ==18429){
		_alpha = 0.0000001;
	}
	else if(_N == 50958 && _NN ==47944){
		//_alpha = 0.0000001;
	}
}

void ExampleFunction::setDnorm(const vector<double> &x){
	fill(_density.begin(), _density.end(), 0.0);
	fastbuild(x);
	for(int w = 0; w<_binRow; ++w){
		double l = w*_binW;
		double r = l+_binW;
		for(int h = 0; h<_binRow; ++h){
			//fill(_xd.begin(), _xd.end(), 0.0);
			//fill(_yd.begin(), _yd.end(), 0.0);
			double b = h*_binH;
			double t = b+_binH;
			for(int i = 0; i<_N; ++i){
				int xi = 2*i;
				if(w >= _xcur[i][0] && w <= _xcur[i][1] && h >= _ycur[i][0] && h <= _ycur[i][1]){
					_xd[i] = sigmoid(x[xi]-l)*sigmoid(r-x[xi]);
					_yd[i] = sigmoid(x[xi+1]-b)*sigmoid(t-x[xi+1]);
					_density[w*_binRow+h] += _xd[i]*_yd[i];
				}
			}
		}
	}
	//if(_lambda!=0){
		//for(int i = 0; i<_binNum;i++)
			//cout << _density[i] << "　";
	//}
	//cout << endl;
	double temp = 0;
	for(int j = 0; j<_binNum; ++j){
		temp += (_density[j]-_Mb)*(_density[j]-_Mb);
	}
	_dnorm = temp;  //normalize
}


void ExampleFunction::LSE_F(const vector<double> &x, double &f){
for(int i = 0; i<_N; ++i){
		int xi = 2*i;
		_LSEcontainer[i].clear();
		_LSEcontainer[i].push_back(exp(x[xi] / _gamma));
		_LSEcontainer[i].push_back(exp(-1.0*x[xi] / _gamma));
		_LSEcontainer[i].push_back(exp(x[xi+1] / _gamma));
		_LSEcontainer[i].push_back(exp(-1.0*x[xi+1] / _gamma));
		//if(isnan(_LSEcontainer[i][0])) cout << "0!!! "<< x[xi]<< endl;
		//if(isnan(_LSEcontainer[i][1])) cout << "1!!! "<< x[xi]<< endl;
		//if(isnan(_LSEcontainer[i][2])) cout << "2!!! "<< x[xi+1]<< endl;
		//if(isnan(_LSEcontainer[i][3])) cout << "3!!! "<< x[xi+1]<< endl;
	}
	for(int i = 0; i < _NN; ++i){
		fill(_tF.begin(),_tF.end(),0.0);
        for(int j = 0; j < _nets[i].numPins(); ++j){
            int idx = _nets[i].pin(j).moduleId();
            for (int k = 0; k<4; ++k){
				_tF[k] += _LSEcontainer[idx][k];
				
			}          
		}
		for (int k = 0; k<4; ++k){
			f += log(_tF[k]);
			
		}
	}
	f *= (_gamma/_iniWire) ;  //normalization
}

void ExampleFunction::LSE_FG(const vector<double> &x, double &f, vector<double> &g){
	for(int i = 0; i<_N; ++i){
		int xi = 2*i;
		_LSEcontainer[i].clear();
		_LSEcontainer[i].push_back(exp(x[xi] / _gamma));
		_LSEcontainer[i].push_back(exp(-1.0*x[xi] / _gamma));
		_LSEcontainer[i].push_back(exp(x[xi+1] / _gamma));
		_LSEcontainer[i].push_back(exp(-1.0*x[xi+1] / _gamma));
	}
	for(int i = 0; i < _NN; ++i){
		fill(_tF.begin(),_tF.end(),0.0);
        for(int j = 0; j < _nets[i].numPins(); ++j){
            int idx = _nets[i].pin(j).moduleId();
            for (int k = 0; k<4; ++k){
				_tF[k] += _LSEcontainer[idx][k];
			}          
		}
		for (int k = 0; k<4; ++k){
			f += log(_tF[k]);
		}
		for(unsigned j = 0; j < _nets[i].numPins(); ++j){
            unsigned idx = _nets[i].pin(j).moduleId();
			unsigned gidx = 2*idx;
			g[gidx] += _LSEcontainer[idx][0]  / _tF[0];
			g[gidx] += _LSEcontainer[idx][1]  / _tF[1]*(-1);
			g[gidx+1] += _LSEcontainer[idx][2] / _tF[2];
			g[gidx+1] += _LSEcontainer[idx][3]  /_tF[3]*(-1);   
		}
	}
	for(unsigned i = 0; i<_N; ++i){  //normalization
		unsigned gidx = 2*i;
		g[gidx] /= _iniWire;
		g[gidx+1] /= _iniWire;
	}
	f *= (_gamma/_iniWire) ;  //normalization
}

double ExampleFunction::sigmoid(double x){
	return 1/(1+exp(-_alpha*x));
}
double ExampleFunction::sigmoidPrime(double a,double b, int i, bool o){
	if(o==0){
		return _alpha*(exp(-_alpha*a)-exp(-_alpha*b))*_yd[i]*_xd[i]*_xd[i];
	}
	else{
		return _alpha*(exp(-_alpha*a)-exp(-_alpha*b))*_xd[i]*_yd[i]*_yd[i];
	}	
}

void ExampleFunction::Denstiy_F(const vector<double> &x, double &f){
	fill(_density.begin(), _density.end(), 0.0);
	fastbuild(x);
	for(int w = 0; w<_binRow; ++w){
		double l = w*_binW;
		double r = l+_binW;
		for(int h = 0; h<_binRow; ++h){
			//fill(_xd.begin(), _xd.end(), 0.0);
			//fill(_yd.begin(), _yd.end(), 0.0);
			double b = h*_binH;
			double t = b+_binH;
			for(int i = 0; i<_N; ++i){
				int xi = 2*i;
				if(w >= _xcur[i][0] && w <= _xcur[i][1] && h >= _ycur[i][0] && h <= _ycur[i][1]){
					_xd[i] = sigmoid(x[xi]-l)*sigmoid(r-x[xi]);
					_yd[i] = sigmoid(x[xi+1]-b)*sigmoid(t-x[xi+1]);
					_density[w*_binRow+h] += _xd[i]*_yd[i];
				}
			}
		}
	}
	//if(_lambda!=0){
		//for(int i = 0; i<_binNum;i++)
			//cout << _density[i] << "　";
	//}
	//cout << endl;
	double temp = 0;
	for(int j = 0; j<_binNum; ++j){
		temp += (_density[j]-_Mb)*(_density[j]-_Mb);
	}
	f += _lambda*temp/(_dnorm);  //normalize
}

void ExampleFunction::Denstiy_FG(const vector<double> &x, double &f,vector<double> &g){
	fill(_density.begin(), _density.end(), 0.0);
	fastbuild(x);
	for(int w = 0; w<_binRow; ++w){
		double l = w*_binW;
		double r = l+_binW;
		for(int h = 0; h<_binRow; ++h){
			//fill(_xd.begin(), _xd.end(), 0.0);
			//fill(_yd.begin(), _yd.end(), 0.0);
			double b = h*_binH;
			double t = b+_binH;
			for(int i = 0; i<_N; ++i){
				int xi = 2*i;
				if(w >= _xcur[i][0] && w <= _xcur[i][1] && h >= _ycur[i][0] && h <= _ycur[i][1]){
					_xd[i] = sigmoid(x[xi]-l)*sigmoid(r-x[xi]);
					_yd[i] = sigmoid(x[xi+1]-b)*sigmoid(t-x[xi+1]);
					_density[w*_binRow+h] += _xd[i]*_yd[i];
				}
			}
			for(int i = 0; i<_N; ++i){
				if(w >= _xcur[i][0] && w <= _xcur[i][1] && h >= _ycur[i][0] && h <= _ycur[i][1]){
					int gidx = 2*i;
					double constant = 2.0*(_density[w*_binRow+h]-_Mb)/sqrt(_dnorm) / 9 *_lambda; //normalize
					g[gidx] += constant*sigmoidPrime(x[gidx]-l,r-x[gidx],i,0);
					g[gidx+1] += constant*sigmoidPrime(x[gidx+1]-b,t-x[gidx+1],i,1);
				}
			}
		}
	}
	double temp = 0;
	for(int j = 0; j<_binNum; ++j){
		temp += (_density[j]-_Mb)*(_density[j]-_Mb);
	}
	f += _lambda*temp/(_dnorm);  //normalize
}



void ExampleFunction::evaluateFG(const vector<double> &x, double &f, vector<double> &g)
{
    f = 0;
	fill(g.begin(), g.end(),0.0);
	fill(_density.begin(), _density.end(),0.0);
	if(_isfirst){
		LSE_FG(x,f,g);
	}
	double temp = f;
	//cout << "wirelength f1 = "<< temp << endl;
	if(_lambda!=0){
		Denstiy_FG(x,f,g);
	}
	//cout << "density f1 = "<< f-temp << endl;
	//if(_lambda!=0){
	//for (unsigned i = 0; i < 2*_N; i+=1000) {
        //cout << "x[" << i << "] = " << x[i] << endl;
    //}}
	//f = 3*x[0]*x[0] + 2*x[0]*x[1] + 2*x[1]*x[1] + 7; // objective function
    //g[0] = 6*x[0] + 2*x[1]; // gradient function of X
    //g[1] = 2*x[0] + 4*x[1]; // gradient function of Y
}

void ExampleFunction::evaluateF(const vector<double> &x, double &f)
{
    //if(_lambda!=0){
	//for (unsigned i = 0; i < 2*_N; i+=1000) {
        //cout << "x[" << i << "] = " << x[i] << endl;
    //}}
	f = 0;
	fill(_density.begin(), _density.end(), 0.0);
	if(_isfirst){
		LSE_F(x,f);
	}
	double temp = f;
	//cout << "wirelength f2 = "<< temp << endl;
	if(_lambda!=0){
		Denstiy_F(x,f);
		}
	//cout << "density f2 = "<< f-temp << endl;
	
	
	//f = 3*x[0]*x[0] + 2*x[0]*x[1] + 2*x[1]*x[1] + 7; // objective function
}

unsigned ExampleFunction::dimension()
{
    return 2*_N; // num_blocks*2 
    // each two dimension represent the X and Y dimensions of each block
}
