#ifndef MODULE_H
#define MODULE_H

#include <stack>
#include <vector>
#include <map>
#include <unordered_map>
#include <math.h>
#include <vector>
#include <string>
#include <map>
#include <stdio.h>      
#include <stdlib.h>     
#include <time.h>  
#include <fstream>     
#include <iostream>
#include <ctime>
#include <chrono>
#include <iomanip> 
#include <queue>

using namespace std;
using namespace std::chrono;


class Terminal
{
public:
    Terminal(int x, int y):_x(x),_y(y) {}
    ~Terminal() {}
    int getX()  { return _x; }
    int getY()  { return _y; }
private:
    int                    _x;
    int                    _y;
};

class Net
{
public:
    Net(int x, int y):_x_min(x), _y_min(y), _x_max(0), _y_max(0) {}
    ~Net() {}
    void NETadd_block(int b) { _blockList.push_back(b); }
    void NETadd_terminal(Terminal*  ) ;
	int getxmin() {return _x_min;}
	int getymin() {return _y_min;}
	int getxmax() {return _x_max;}
	int getymax() {return _y_max;}
	int findblock(int i) {return _blockList[i];}
	
private:
    int                    _x_min;
    int                    _x_max;
    int                    _y_min;
    int                    _y_max;
    vector<int>         _blockList;

};

class Block 
{
public:
    // constructor and destructor
    Block(int id, int w, int h) :
         _id(id),_w(w), _h(h), _x1(0), _y1(0), _x2(0), _y2(0), _parent(NULL), _left(NULL) ,_right(NULL),_role(-1) { 
		}
    ~Block() { }

    // basic access methods
	const int getId() { return _id; }
    const int getWidth(bool rotate = false)  { return rotate? _h: _w; }
    const int getHeight(bool rotate = false) { return rotate? _w: _h; }
    const int getArea()  { return _h * _w; }
	
    const int getX1()    { return _x1; }
    const int getX2()    { return _x2; }
    const int getY1()    { return _y1; }
    const int getY2()    { return _y2; }
	
	int getrole() const       { return _role; }
    Block* getPar() const   { return _parent; }
    Block* getLeft() const   { return _left; }
	Block* getRight() const   { return _right; }

    // set functions
    //void setWidth(int w)         { _w = w; }
    //void setHeight(int h)        { _h = h; }
    //static void setMaxX(int x)   { _maxX = x; }
    //static void setMaxY(int y)   { _maxY = y; }
	
    void setPos(int x1, int y1, int x2, int y2) {
        _x1 = x1;   _y1 = y1;
        _x2 = x2;   _y2 = y2;
    }
	
	void setPar(Block* parent)  { _parent = parent; }
    void setLeft(Block* left)  { _left = left; }
	void setRight(Block* right)  { _right = right; }
	void setrole(const int role) {_role = role;}
	void turn()	{int temp = _w; _w = _h; _h = temp;}
	void backup();
	void updatebest();
	void giveup();


private:
    int			 _id;
	int          _w;         // width of the block
    int          _h;         // height of the block

	
    int      _x1;        // min x coordinate of the terminal
    int      _y1;        // min y coordinate of the terminal
    int      _x2;        // max x coordinate of the terminal
    int      _y2;        // max y coordinate of the terminal
	
    Block *		_parent;  
    Block * 	_left;  
	Block *		_right;
	int			_role;
	
	int      _Px1;        // min x coordinate of the terminal
    int      _Py1;        // min y coordinate of the terminal
    int      _Px2;        // max x coordinate of the terminal
    int      _Py2;        // max y coordinate of the terminal
	
    Block *		_Pparent;  
    Block * 	_Pleft;  
	Block *		_Pright;
	int			_Prole;
	
	int      _Bx1;        // min x coordinate of the terminal
    int      _By1;        // min y coordinate of the terminal
    int      _Bx2;        // max x coordinate of the terminal
    int      _By2;        // max y coordinate of the terminal
	
    Block *		_Bparent;  
    Block * 	_Bleft;  
	Block *		_Bright;
	int			_Brole;
};

class BStarTree
{
public:
    // constructor and destructor
    BStarTree(fstream& input_block,fstream& input_nets,double alpha):
        _X(0), _Y(0), _B(0), _T(0),_N(0),_root(0), _maxX(0),_maxY(0),_alpha(alpha) {
			vector<Block*>      _blockArray;
			vector<Terminal*>	_termArray;
			vector<Net*>		_netArray;
    }
    ~BStarTree() {
        clear();
    }
	void WriteResult(fstream&,duration<double>);
	int getCase()			{return _Case;}
    // basic access methods
    int getXBound()         { return _X; }
	int getYBound()         { return _Y; }
    int getBlockNum()       { return _B; }
	int get_root()  		{ return _root;}
    

    // modify method
	void packLeft(int a, int b);
	void packRight(int a, int b);
	
	void read_block(fstream& );
	void read_nets(fstream& );
    void add_block(int id, int w, int h){ Block* b = new Block(id, w, h); _blockArray.push_back(b);}
	void add_terminal(int x, int y){ Terminal* t = new Terminal(x, y); _termArray.push_back(t);}
	void add_net() { Net* n = new Net(_X, _Y); _netArray.push_back(n); }
	
	void setroot(int root) {_root = root; }

	
	void initialization();
	void initialization2();
	void initialization3();
	void updatecoord();
	void updatecoord2();
	int findYcoord(int lx, int rx, int h);
	
	void rotate(int a);
	void swap(int a, int b);
	void insert(int a, int b);
    
	// SA
	double HPWL();
	void SA();
	void selectMove(int choice);
	void calcost();
	void record();
	void recordbest();
	void undo();
	
	
    // member functions about reporting
	void printBT();
	void printcontour();
	void WriteResult2(fstream& output, duration<double> diff);

private:
	int					_X;
	int					_Y;
	int					_B;
	int					_T;
	int					_N;
	int					_Case;

	vector<Block*>      _blockArray;
	vector<Terminal*>	_termArray;
	vector<Net*>		_netArray;
	vector<int>			_netdegree;
    vector<string>                          _Id2Block;
    vector<string>                          _Id2Terminal;
    unordered_map<string, int>         _Block2Id;
    unordered_map<string, int>         _Terminal2Id;
	map<int,int>     	_hContour;
	int 				_maxX; 
	int					_maxY;
	int					_root;
	
	map<int,int>     	_PhContour;
	int 				_PmaxX; 
	int					_PmaxY;
	int					_Proot;
	
	map<int,int>     	_BhContour;
	int 				_BmaxX; 
	int					_BmaxY;
	int					_Broot;
	double					_BW;
	
	double					_alpha;
	double					_cost;
	double					_pcost;
	double					_bcost;
	double					_A;
	double 					_W;
	double					_avgA;
	double					_avgW;
	double					_deltacost;
	int 					_totcount;
	
    void clear();
};



#endif  // MODULE_H
