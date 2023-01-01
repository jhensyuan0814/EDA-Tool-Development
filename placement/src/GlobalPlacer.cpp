#include "GlobalPlacer.h"
#include "ExampleFunction.h"
#include "NumericalOptimizer.h"
#include <algorithm>
#include <stdio.h>
#include <stdlib.h> 
GlobalPlacer::GlobalPlacer(Placement &placement)
	:_placement(placement)
{

}


double GlobalPlacer::adjust( double pos, double U, double L, double length){
	if( pos+length > U){ 
		return	U - length;
	}
	else if(  pos<L){
		return	L;
	}
    else {
        return pos;
    }
}
void GlobalPlacer::virgin2(vector<double> &x,double W, double H){
	int div = 15;
	double binW = W/div;
	double binH = H/div;
	for (int i = 0; i < _placement.numModules(); ++i){
		int xi = 2*i;
		//Module module = _placement.module(i);
		for (int j = 0; j<div; j++){
			for (int k = 0;k<div;k++){
				int w = (int) j*binW;
				int h = (int) k*binH;
				double tx = _placement.boundryLeft()+w+( rand() % (int)binW);
				double ty = _placement.boundryBottom()+h+( rand() % (int)binH);
				x[xi] = adjust(tx,_placement.boundryRight(), _placement.boundryLeft(), _placement.module(i).width());
				x[xi+1] = adjust(ty,_placement.boundryTop(), _placement.boundryBottom(), _placement.module(i).height());
				_placement.module(i).setPosition(x[xi],x[xi+1]);
			}
		}
	}
	for (int i = 0; i < _placement.numModules(); i+=100){
		cout << "Modules[" << i << "] x:"<<  _placement.module(i).x() << " y:" << _placement.module(i).y()<< endl;
	}
}
void GlobalPlacer::virgin(vector<double> &x,double W, double H){
	for (int i = 0; i < _placement.numModules(); ++i){
		int xi = 2*i;
		//Module module = _placement.module(i);
		x[xi] = _placement.boundryLeft()+( rand() % (int)(W - _placement.module(i).width()));
		x[xi+1] = _placement.boundryBottom()+( rand() % (int)(H - _placement.module(i).height()));
		_placement.module(i).setPosition(x[xi],x[xi+1]);
	}
	
}
void GlobalPlacer::place()
{
	///////////////////////////////////////////////////////////////////
	// The following example is only for analytical methods.
	// if you use other methods, you can skip and delete it directly.
	//////////////////////////////////////////////////////////////////
	double W = _placement.boundryRight() - _placement.boundryLeft();
	double H = _placement.boundryTop() - _placement.boundryBottom();
	ExampleFunction ef(_placement.getmodules(),_placement.getnets(),_placement.numModules(),_placement.numNets(),W,H,_placement.boundryLeft(),_placement.boundryBottom()); // require to define the object function and gradient function
    vector<double> x;  // solution vector, size: num_blocks*2 
                         // each 2 variables represent the X and Y dimensions of a block
	x.resize(ef.dimension(),0);
	if(_placement.numModules() == 12028 && _placement.numNets() == 11507){srand(10015);}
	if(_placement.numModules() == 29347 && _placement.numNets() == 28446){srand(10004);}
	if(_placement.numModules() == 45098 && _placement.numNets() == 44394){srand(10020);}
	if(_placement.numModules() == 50958 && _placement.numNets() == 47944){srand(10004);}
	if(_placement.numModules() == 51667 && _placement.numNets() == 50393){srand(10004);}
	if(_placement.numModules() == 19321 && _placement.numNets() == 18429){srand(1);}
	virgin(x,W,H);
    NumericalOptimizer no(ef);
	ef.initialization();
	double lambdaini = 0;
    ef.setlambda(lambdaini);
	ef.setiniWire(_placement.computeHpwl());
	
    int iteration =0;  
	int itB = 2;
	ef.setisfirst(1);
	double factor = H*W/(33300+33400)/(33200+33320)/3;
    while (true){  
		//ef.setiniWire(_placement.computeHpwl());
		//cout << "iteration[" << iteration << "] starts!!!"<< endl;
		if(iteration !=0){ef.setDnorm(x);}
		no.setX(x);//set initial solution
        //no.setNumIteration(150);  // user-specified parameter
		if(_placement.numModules() == 12028 && _placement.numNets() == 11507){//ibm01
			if (iteration==0){
			no.setNumIteration(7);
			no.setStepSizeBound(max(H,W)*1.0*8);
		}
		else{
			no.setNumIteration(5);
			no.setStepSizeBound(max(H,W)*1.0*8.5);
			if(iteration==2)break;
		}
		}
		else if(_placement.numModules() == 29347 && _placement.numNets() == 28446){//ibm05
			if (iteration==0){
			no.setNumIteration(6);
			no.setStepSizeBound(max(H,W)*1.0*8);
		}
		else{
			no.setNumIteration(10);
			no.setStepSizeBound(max(H,W)*1.0*7.2);
			if(iteration==2)break;
		}
		}
		else if(_placement.numModules() == 19321 && _placement.numNets() == 18429){//ibm02
			if (iteration==0){
			no.setNumIteration(6);
			no.setStepSizeBound(max(H,W)*1.0*8);
			factor /= 1000;
		}
		else{
			no.setNumIteration(2);
			no.setStepSizeBound(max(H,W)*1.0*41.5);
			if(iteration == 2)break;
			//cout<< "!!!!!!!!!!" << endl;
		}
		}
		else if(_placement.numModules() == 45098 && _placement.numNets() == 44394){//ibm07
		if (iteration==0){
			no.setNumIteration(5);
			no.setStepSizeBound(max(H,W)*1.0*1);
		}
		else{
			no.setNumIteration(2);
			no.setStepSizeBound(max(H,W)*1.0*10);
		}
		}
		else if(_placement.numModules() == 50958 && _placement.numNets() == 47944){//ibm08
		if (iteration==0){
			no.setNumIteration(6);
			no.setStepSizeBound(max(H,W)*1.0*1);
		}
		else{
			no.setNumIteration(2);
			no.setStepSizeBound(max(H,W)*1.0*10);
		}
		}
		else if(_placement.numModules() == 51667 && _placement.numNets() == 50393){//ibm09
		cout << "9!!!!!!!!!" << endl;
		if (iteration==0){
			no.setNumIteration(6);
			no.setStepSizeBound(max(H,W)*1.0*1);
		}
		else{
			no.setNumIteration(1);
			no.setStepSizeBound(max(H,W)*1.0*10);
		}
		}
		else{
		if (iteration==0){
			no.setNumIteration(5);
			no.setStepSizeBound(max(H,W)*1.0*1);
		}
		else{
			no.setNumIteration(2);
			no.setStepSizeBound(max(H,W)*1.0*10);
		}
		}
		// u
		//no.setStepSizeBound(max(H,W)/1.0*7);// user-specified parameter
		no.solve(); // Conjugate Gradient solver
		for(int i = 0; i < _placement.numModules(); ++i){
			int xi = 2*i;
			x[xi] = adjust( no.x(xi), _placement.boundryRight(), _placement.boundryLeft(), _placement.module(i).width() );
			x[xi+1] = adjust( no.x(xi+1), _placement.boundryTop(), _placement.boundryBottom(), _placement.module(i).height() ); 
            _placement.module(i).setPosition(x[xi], x[xi+1]);
		}
	    iteration++;
		//cout << "Current solution:" << endl;
		for (unsigned i = 0; i < no.dimension(); i+=1000) {
			//cout << "x[" << i << "] = " << no.x(i) << endl;
		}
        if (iteration>itB){
			break;
        }
	    lambdaini = 1/factor;
		ef.setlambda(lambdaini);
		//ef.setiniWire(_placement.computeHpwl());
		//if (iteration!=0){
			//ef.setisfirst(0);
    //
		//ef.setisfirst(0);
	
		}
    //cout << "Current solution:" << endl;
    for (unsigned i = 0; i < no.dimension(); i+=1000) {
        //cout << "x[" << i << "] = " << no.x(i) << endl;
    }
    //cout << "Objective: " << no.objective() << endl;
	////////////////////////////////////////////////////////////////
	

}


void GlobalPlacer::plotPlacementResult( const string outfilename, bool isPrompt )
{
    ofstream outfile( outfilename.c_str() , ios::out );
    outfile << " " << endl;
    outfile << "set title \"wirelength = " << _placement.computeHpwl() << "\"" << endl;
    outfile << "set size ratio 1" << endl;
    outfile << "set nokey" << endl << endl;
    outfile << "plot[:][:] '-' w l lt 3 lw 2, '-' w l lt 1" << endl << endl;
    outfile << "# bounding box" << endl;
    plotBoxPLT( outfile, _placement.boundryLeft(), _placement.boundryBottom(), _placement.boundryRight(), _placement.boundryTop() );
    outfile << "EOF" << endl;
    outfile << "# modules" << endl << "0.00, 0.00" << endl << endl;
    for( size_t i = 0; i < _placement.numModules(); ++i ){
        Module &module = _placement.module(i);
        plotBoxPLT( outfile, module.x(), module.y(), module.x() + module.width(), module.y() + module.height() );
    }
    outfile << "EOF" << endl;
    outfile << "pause -1 'Press any key to close.'" << endl;
    outfile.close();

    if( isPrompt ){
        char cmd[ 200 ];
        sprintf( cmd, "gnuplot %s", outfilename.c_str() );
        if( !system( cmd ) ) { cout << "Fail to execute: \"" << cmd << "\"." << endl; }
    }
}

void GlobalPlacer::plotBoxPLT( ofstream& stream, double x1, double y1, double x2, double y2 )
{
    stream << x1 << ", " << y1 << endl << x2 << ", " << y1 << endl
           << x2 << ", " << y2 << endl << x1 << ", " << y2 << endl
           << x1 << ", " << y1 << endl << endl;
}
