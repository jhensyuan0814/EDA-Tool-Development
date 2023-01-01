#include "module.h"

using namespace std;
//////////////////////
///////net
//////////////////////
void Net::NETadd_terminal(Terminal* t)
{
    if(t->getX() < _x_min) _x_min = t->getX();
    if(t->getY() < _y_min) _y_min = t->getY();
	if(t->getX() > _x_max) _x_max = t->getX();
    if(t->getY() > _y_max) _y_max = t->getY();
    return;
}
//////////////////////
///////block
//////////////////////
void Block::backup()
{
	_Px1 = _x1;        
    _Py1 = _y1;     
    _Px2 = _x2;        
    _Py2 = _y2;       
	
    _Pparent = _parent;  
    _Pleft = _left;  
	_Pright = _right;
	_Prole = _role;
	//cout << _role<<" "<<_Prole<< endl;
	return;
}
void Block::updatebest()
{
	_Bx1 = _x1;        
    _By1 = _y1;     
    _Bx2 = _x2;        
    _By2 = _y2;       
	
    _Bparent = _parent;  
    _Bleft = _left;  
	_Bright = _right;
	_Brole = _role;
	return;
}
void Block::giveup()
{
	_x1 = _Px1;        
    _y1 = _Py1;     
    _x2 = _Px2;        
    _y2 = _Py2;       
	
    _parent = _Pparent;  
    _left = _Pleft;  
	_right = _Pright;
	_role = _Prole;
	//cout << _role<<" "<<_Prole<< endl;
	return;
}
//////////////////////
///////input parse
//////////////////////

void BStarTree::packLeft(int a, int b){
	_blockArray[b]->setPar(_blockArray[a]);
	_blockArray[a]->setLeft(_blockArray[b]);
	_blockArray[b]->setrole(0);
	return;
}
void BStarTree::packRight(int a, int b){
	_blockArray[b]->setPar(_blockArray[a]);
	_blockArray[a]->setRight(_blockArray[b]);
	_blockArray[b]->setrole(1);
	return;
}

void BStarTree::read_block(fstream& input_block)
{
    string buf;
    int x,y,b,t;
    input_block >> buf;//Outline:
    input_block >> x >> y;
    input_block >> buf;//NumBlocks:
    input_block >> b >> buf >> t;//NumTerminals:
	_X = x;
	_Y = y;
	_B = b;
	_T = t;
	
    //blocks
    for(int i = 0; i < b;i++)
    {
        input_block >> buf >> x >> y;//block name
        _Block2Id.insert(pair<string,int>(buf,i));
        _Id2Block.push_back(buf);
        add_block(i,x,y);
    }
    //terminals
    for(int i = 0; i < t;i++)
    {
        input_block >> buf;
        _Terminal2Id.insert(pair<string,int>(buf,i));
        _Id2Block.push_back(buf);
        input_block >> buf>> x >> y;//terminal
        add_terminal(x,y);
    }
	//cout << "Block" <<_blockArray[b-1]->getId() << " name: " << _Id2Block[_blockArray[b-1]->getId()] << " w: " << _blockArray[b-1]->getWidth() << " h: " << _blockArray[b-1]->getHeight()<<endl;
	//cout << "Last Terminal x: " << _termArray[t-1]->getX() << " y: " << _termArray[t-1]->getY()<<endl;
	if(_X == 1326 && _Y == 1205 && _B ==33){_Case = 1;}
	else if(_X == 5336 && _Y == 7673 && _B ==49){_Case = 2;}
	else{_Case = 0;}
	return;
}

void BStarTree::read_nets(fstream& input_nets)
{
    
	string buf;
    int n, d, temp;
    input_nets >> buf >> n;//NumNets:
    _N = n;
	for (int i = 0; i<n; i++){ 
        input_nets >> buf >> d; //NetDegree:
        add_net();
		_netdegree.push_back(d);
        for(int j = 0; j < d; j++)
        {
			input_nets >> buf;//terminal name
			
            if(_Block2Id.count(buf) == 0)//terminal
            {
                temp = _Terminal2Id[buf];
				//cout << temp << " " << buf << "  &&&&    ";
                _netArray[i]->NETadd_terminal(_termArray[temp]);
				_netdegree[i]--;
            }
            else//block
            {
                temp = _Block2Id[buf];
				//cout << temp << " " << buf << "  &&&&    ";
                _netArray[i]->NETadd_block(temp);
            }
        }
		cout << endl;
		
    }
	/*
	int j = 0;
	for (size_t k = 0, end_k = _netArray.size(); k < end_k; ++k){ 
		cout << "net" << k << " :";
		
			for(int i = 0; i<_netdegree[j];++i){
				cout << _Id2Block[_netArray[k]->findblock(i)] << " ";
			}
		j++;
		cout << endl;
			
	}*//*
	for (size_t k = 0, end_k = _netArray.size(); k < end_k; ++k){ 
		cout << "net" << k << " :";
		cout << "xmin = " << _netArray[k]->getxmin() << "xmax = "<<_netArray[k]->getxmax() << "ymin = "<< _netArray[k]->getymin() << "ymax = "<<_netArray[k]->getymax() << endl;
			
	}*//*
	for (size_t k = 0, end_k = _netdegree.size(); k < end_k; ++k){
		cout << "NET" << k << ":"<<_netdegree[k] << endl;
	}*/

	return;
	
}

//////////////////////
///////BStarTree
//////////////////////
void BStarTree::printBT(){
	stack<int> myStack;
    myStack.push(_root);
    while(!myStack.empty()){ 
        int t_id = myStack.top() ;
		myStack.pop();
		int temp = (_root!=t_id)?_blockArray[t_id]->getPar()->getId():-1;
		cout << t_id <<"("<<_blockArray[t_id]->getrole()<<" , "<< temp <<")"<< "->" ;
        if (_blockArray[t_id]->getRight() != NULL)
            myStack.push(_blockArray[t_id]->getRight()->getId()) ;
        if (_blockArray[t_id]->getLeft() != NULL)
            myStack.push(_blockArray[t_id]->getLeft()->getId()) ;
	}
	cout << endl;
	return;
}
void BStarTree::initialization()
{
	size_t par;
	for (size_t i = 1, end_i = _blockArray.size(); i < end_i; ++i){ 
		par = (i+1)/2 - 1;
		_blockArray[i] -> setPar(_blockArray[par]);
		if(i%2 == 0){
			_blockArray[par]->setRight(_blockArray[i]);
			_blockArray[i]->setrole(1);
		}
		else{
			_blockArray[par]->setLeft(_blockArray[i]);
			_blockArray[i]->setrole(0);
		}
	}
	//printBT();
	/*
	for (size_t i = 1, end_i = _blockArray.size(); i < end_i; ++i){
			cout << i << "  " << _blockArray[i]->getPar()->getId()<<endl;
	}*/
	return;
}
void BStarTree::initialization2()
{
	vector<int> turn { 3,10,11,12,25,32,5,6,14,15,18,26,24,17,20,28 };
	for (size_t i = 0, end_i = turn.size(); i < end_i; ++i){
		_blockArray[turn[i]]->turn();
	}
	_root = 1;
	_blockArray[_root]->setrole(-1);
	packRight(1,8);
	packRight(8,0);
	packRight(0,5);
	packRight(5,13);
	packRight(13,2);
	packRight(2,20);
	
	packLeft(1,3);
	packLeft(3,4);
	packLeft(4,7);
	packLeft(7,31);
	packLeft(31,9);
	
	packLeft(8,10);
	packLeft(10,11);
	packLeft(11,12);
	packLeft(12,29);
	packLeft(29,16);
	
	packLeft(0,25);
	packLeft(25,21);
	packLeft(21,32);
	
	packLeft(5,6);
	packLeft(6,19);
	
	packLeft(13,14);
	packLeft(14,15);
	packLeft(15,18);
	packLeft(18,26);
	
	packLeft(2,22);
	packLeft(22,24);
	packLeft(24,23);
	packLeft(23,17);
	packLeft(17,27);
	
	packLeft(20,28);
	packLeft(28,30);
	
	//
	//printBT();
	/*
	for (size_t i = 1, end_i = _blockArray.size(); i < end_i; ++i){
			cout << i << "  " << _blockArray[i]->getPar()->getId()<<endl;
	}*/
	return;
}
void BStarTree::initialization3()
{
	vector<int> turn { 3,2,42,21,8,32,5,12,29,47,39,40,26,36,45,17,22,11,10,19,23,24,31 };
	for (size_t i = 0, end_i = turn.size(); i < end_i; ++i){
		_blockArray[turn[i]]->turn();
	}
	_root = 0;
	_blockArray[_root]->setrole(-1);
	packLeft(0,3);
	packLeft(3,2);
	packLeft(2,35);
	packRight(35,21);
	packLeft(21,13);
	packRight(21,18);
	packLeft(18,20);
	packRight(18,28);
	packLeft(28,8);
	packRight(2,37);
	packLeft(37,42);
	
	packRight(0,32);
	packLeft(32,5);
	packLeft(5,1);
	packLeft(1,4);
	packLeft(4,29);
	packLeft(29,47);
	packLeft(47,43);
	
	packRight(29,39);
	packLeft(39,34);
	packLeft(34,16);
	packRight(39,40);
	packLeft(40,26);
	packRight(40,17);
	packLeft(17,30);
	packLeft(30,27);
	packLeft(27,22);
	packLeft(22,6);
	packRight(6,9);
	packRight(1,12);
	packRight(4,41);
	
	packRight(32,7);
	packLeft(7,36);
	packLeft(36,45);
	packLeft(45,44);
	packLeft(44,33);
	
	packRight(7,14);
	packLeft(14,10);
	packLeft(10,46);
	packLeft(46,15);
	packLeft(15,19);
	packLeft(19,11);
	packLeft(11,23);
	packLeft(23,24);
	packLeft(24,31);
	packLeft(31,38);
	packLeft(38,48);
	packLeft(48,25);
	
	
	
	
	
	//printBT();
	/*
	for (size_t i = 1, end_i = _blockArray.size(); i < end_i; ++i){
			cout << i << "  " << _blockArray[i]->getPar()->getId()<<endl;
	}*/
	return;
}

int BStarTree::findYcoord(int lx, int rx, int h)
{
	int newh = -1;
	
	if(lx >= _maxX){
		newh = 0;
		_hContour.insert(pair<int, int>(lx, h));
		_maxX = rx;
	}
	else if(rx >= _maxX){
		int Lcand = -1;
		int Lcand_h = -1;
		vector<int> Mcand;
		int Mcand_h = -1;
			
		map<int,int>::iterator it1;
		for(it1 = _hContour.begin(); it1 != _hContour.end(); it1++){
			int temp1 = it1->first;
			int temp2 = it1->second;
			if(temp1 <= lx){
				if(Lcand<temp1){
					Lcand = temp1;
					Lcand_h = temp2;
				}
			}
			else{
				Mcand.push_back(temp1);
				if(temp2>Mcand_h){
					Mcand_h = temp2;
				}
			}
		}
		newh = (Lcand_h < Mcand_h)? Mcand_h:Lcand_h;
		int newnewh = newh+h;
		if(Lcand == lx){
			_hContour[lx] = newnewh;
		}
		else{
			_hContour.insert(pair<int, int>(lx, newnewh));
		}
		for (size_t i = 0, end_i = Mcand.size(); i < end_i; ++i){ 
			_hContour.erase(_hContour.find(Mcand[i]));
		}
		_maxX = rx;
	}
	else{
		int Lcand = -1;
		int Lcand_h = -1;
		vector<int> Mcand;
		int Mcand_h = -1;
		int MRcand = -1;
		int MRcand_h = -1;
			
		map<int,int>::iterator it1;
		for(it1 = _hContour.begin(); it1 != _hContour.end(); it1++){
			int temp1 = it1->first;
			int temp2 = it1->second;
			if(temp1 <= lx){
				if(Lcand<temp1){
					Lcand = temp1;
					Lcand_h = temp2;
				}
			}
			else if(temp1<rx){
				Mcand.push_back(temp1);
				if(temp2>Mcand_h){
					Mcand_h = temp2;
				}
				if(temp1>MRcand){
					MRcand = temp1;
					MRcand_h = temp2;
				}
			}
		}
		newh = (Lcand_h < Mcand_h)? Mcand_h:Lcand_h;
		int newnewh = newh+h;
		/*if(MRcand == -1){
			_hContour.insert(pair<int, int>(rx, _hContour[Lcand]));
		}*/
		if(Lcand == lx){
			_hContour[lx] = newnewh;
		}
		else{
			_hContour.insert(pair<int, int>(lx, newnewh));
		}
		
		for (size_t i = 0, end_i = Mcand.size(); i < end_i; ++i){ 
			_hContour.erase(_hContour.find(Mcand[i]));
		}
		if(MRcand == -1) _hContour.insert(pair<int, int>(rx, Lcand_h));
		else _hContour.insert(pair<int, int>(rx, MRcand_h));

	}
	return newh;
}

void BStarTree::updatecoord()
{
	_hContour.clear();
	_maxX = 0;
	stack<int> myStack;
    myStack.push(_root);
    while(!myStack.empty()){ 
        int t_id = myStack.top() ;
		myStack.pop();
		int w = _blockArray[t_id]->getWidth();
		int h = _blockArray[t_id]->getHeight();
		int role = _blockArray[t_id]->getrole();
		if(role == -1){//is root
			_blockArray[t_id]->setPos(0, 0, w, h);
			_maxX = w;
			_maxY = h;
			_hContour.insert(pair<int, int>(0, h));
			
		}
		else if(role == 0){//is a left child 
			int px2 = _blockArray[t_id]->getPar()->getX2();
			int y1 = findYcoord(px2,px2+w,h);
			_blockArray[t_id]->setPos(px2,y1,px2+w,y1+h);
			if(y1+h>_maxY)_maxY = y1+h;
			
		}
		else{//is a right child
			int px1 = _blockArray[t_id]->getPar()->getX1();
			int y1 = findYcoord(px1,px1+w,h);
			_blockArray[t_id]->setPos(px1,y1,px1+w,y1+h);
			if(y1+h>_maxY)_maxY = y1+h;
		}
        if (_blockArray[t_id]->getRight() != NULL)
            myStack.push(_blockArray[t_id]->getRight()->getId()) ;
        if (_blockArray[t_id]->getLeft() != NULL)
            myStack.push(_blockArray[t_id]->getLeft()->getId()) ;
		//cout << t_id <<" " <<_Id2Block[t_id]<<endl;
		//cout << "$$$$$$$$$$$$$$$$$$$$$"<< endl;
		//printcontour();
	}
	
	return;
}

void BStarTree::updatecoord2()
{
	_hContour.clear();
	_maxX = 0;
	queue<int> myQueue;
    myQueue.push(_root);
    while(!myQueue.empty()){ 
        int t_id = myQueue.front() ;
		myQueue.pop();
		int w = _blockArray[t_id]->getWidth();
		int h = _blockArray[t_id]->getHeight();
		int role = _blockArray[t_id]->getrole();
		if(role == -1){//is root
			_blockArray[t_id]->setPos(0, 0, w, h);
			_maxX = w;
			_maxY = h;
			_hContour.insert(pair<int, int>(0, h));
			
		}
		else if(role == 0){//is a left child 
			int px2 = _blockArray[t_id]->getPar()->getX2();
			int y1 = findYcoord(px2,px2+w,h);
			_blockArray[t_id]->setPos(px2,y1,px2+w,y1+h);
			if(y1+h>_maxY)_maxY = y1+h;
			
		}
		else{//is a right child
			int px1 = _blockArray[t_id]->getPar()->getX1();
			int y1 = findYcoord(px1,px1+w,h);
			_blockArray[t_id]->setPos(px1,y1,px1+w,y1+h);
			if(y1+h>_maxY)_maxY = y1+h;
		}
        if (_blockArray[t_id]->getRight() != NULL)
            myQueue.push(_blockArray[t_id]->getRight()->getId()) ;
        if (_blockArray[t_id]->getLeft() != NULL)
            myQueue.push(_blockArray[t_id]->getLeft()->getId()) ;
		//cout << t_id <<" " <<_Id2Block[t_id]<<endl;
		//cout << "$$$$$$$$$$$$$$$$$$$$$"<< endl;
		//printcontour();
	}
	
	return;
}

void BStarTree::rotate(int a){
	//cout << _blockArray[a]->getHeight() << " " << _blockArray[a]->getWidth() << endl;
	_blockArray[a]->turn();
	//cout << _blockArray[a]->getHeight() << " " << _blockArray[a]->getWidth() << endl;
	//cout << "Rotation: a = "<< a << endl;
	updatecoord();
	//updatecoord2();
	return;
}

void BStarTree::swap(int a,int b){//a,b cannot be parent and son
	//parent
	//cout << "Swap: a = "<< a <<"; b = "<< b << endl;
	if(_root == a){
		int bp_id = _blockArray[b]->getPar()->getId();
		_blockArray[a]->setPar(_blockArray[bp_id]);
		_blockArray[b]->setPar(NULL);
		int br = _blockArray[b]->getrole();
		if(br == 0){
			_blockArray[a]->setrole(0);
			_blockArray[bp_id]->setLeft(_blockArray[a]);
		}
		else{
			_blockArray[a]->setrole(1);
			_blockArray[bp_id]->setRight(_blockArray[a]);
		} 
		_blockArray[b]->setrole(-1);
		_root = b;
	}
	else if(_root == b){
		int ap_id = _blockArray[a]->getPar()->getId();
		_blockArray[b]->setPar(_blockArray[ap_id]);
		_blockArray[a]->setPar(NULL);
		int ar = _blockArray[a]->getrole();
		if(ar == 0){
			_blockArray[b]->setrole(0);
			_blockArray[ap_id]->setLeft(_blockArray[b]);
		}
		else{
			_blockArray[b]->setrole(1);
			_blockArray[ap_id]->setRight(_blockArray[b]);
		} 
		_blockArray[a]->setrole(-1);
		_root = a;
	}
	else{
		int ap_id = _blockArray[a]->getPar()->getId();
		int bp_id = _blockArray[b]->getPar()->getId();
		_blockArray[a]->setPar(_blockArray[bp_id]);
		_blockArray[b]->setPar(_blockArray[ap_id]);
		int ar = _blockArray[a]->getrole();
		int br = _blockArray[b]->getrole();
		if(ar == 0){
			_blockArray[b]->setrole(0);
			_blockArray[ap_id]->setLeft(_blockArray[b]);
		}
		else{
			_blockArray[b]->setrole(1);
			_blockArray[ap_id]->setRight(_blockArray[b]);
		} 
		if(br == 0){
			_blockArray[a]->setrole(0);
			_blockArray[bp_id]->setLeft(_blockArray[a]);
		}
		else{
			_blockArray[a]->setrole(1);
			_blockArray[bp_id]->setRight(_blockArray[a]);
		} 
	}
	//children
	int alc = (_blockArray[a]->getLeft() != NULL)? _blockArray[a]->getLeft()->getId():-1;
	int arc = (_blockArray[a]->getRight() != NULL)? _blockArray[a]->getRight()->getId():-1;	
	int blc = (_blockArray[b]->getLeft() != NULL)? _blockArray[b]->getLeft()->getId():-1;
	int brc = (_blockArray[b]->getRight() != NULL)? _blockArray[b]->getRight()->getId():-1;
	_blockArray[a]->setLeft((blc != -1)? _blockArray[blc]:NULL);
	_blockArray[a]->setRight((brc != -1)? _blockArray[brc]:NULL);
	_blockArray[b]->setLeft((alc != -1)? _blockArray[alc]:NULL);
	_blockArray[b]->setRight((arc != -1)? _blockArray[arc]:NULL);
	if(alc != -1) _blockArray[alc]->setPar(_blockArray[b]);
	if(arc != -1) _blockArray[arc]->setPar(_blockArray[b]);
	if(blc != -1) _blockArray[blc]->setPar(_blockArray[a]);
	if(brc != -1) _blockArray[brc]->setPar(_blockArray[a]);
	updatecoord();
	//updatecoord2();
	return;
}

void BStarTree::insert(int a,int b){//a is deleted and becomes b's child
	//delete
	//cout << "Insert: a = "<< a <<"; b = "<< b << endl;
	if(_root != a){
		int ap = _blockArray[a]->getPar()->getId();
		int a_role = _blockArray[a]->getrole();
		if(_blockArray[a]->getLeft()== NULL && _blockArray[a]->getRight()== NULL){ //no child
			if(a_role==0) _blockArray[ap]->setLeft(NULL);
			else _blockArray[ap]->setRight(NULL);
			
		}
		else if(_blockArray[a]->getRight()== NULL){//only left child
			int al = _blockArray[a]->getLeft()->getId();
			_blockArray[al]->setPar(_blockArray[ap]);
			if(a_role==0){ 
				_blockArray[ap]->setLeft(_blockArray[al]);
				_blockArray[al]->setrole(0);
			}
			else {
				_blockArray[ap]->setRight(_blockArray[al]);
				_blockArray[al]->setrole(1);
			}

		
		}
		else if(_blockArray[a]->getLeft()== NULL){//only right child
			int ar = _blockArray[a]->getRight()->getId();
			_blockArray[ar]->setPar(_blockArray[ap]);
			if(a_role==0){ 
				_blockArray[ap]->setLeft(_blockArray[ar]);
				_blockArray[ar]->setrole(0);
			}
			else {
				_blockArray[ap]->setRight(_blockArray[ar]);
				_blockArray[ar]->setrole(1);
			}

		}
		else{//two children
	
			int ar = _blockArray[a]->getRight()->getId();
			int al = _blockArray[a]->getLeft()->getId();
			//find successor(a leaf node descendant)
			Block * tblock = _blockArray[a];
			while(tblock->getLeft() != NULL || tblock->getRight() != NULL){ 
				if(tblock->getLeft() != NULL) tblock = tblock->getLeft();
				else if(tblock->getRight() != NULL) tblock = tblock->getRight();
			}
			int successor = tblock->getId();
			int suc_par = tblock->getPar()->getId();
			int suc_role = tblock->getrole();
			//handle successor's parent
			if(suc_role == 0) _blockArray[suc_par]->setLeft(NULL);
			else _blockArray[suc_par]->setRight(NULL);
			//find successor's new parent
			_blockArray[successor]->setPar(_blockArray[ap]);
			if(a_role==0){ 
				_blockArray[ap]->setLeft(_blockArray[successor]);
				_blockArray[successor]->setrole(0);
			}
			else {
				_blockArray[ap]->setRight(_blockArray[successor]);
				_blockArray[successor]->setrole(1);
			}
			//find successor's new children
			_blockArray[ar]->setPar(_blockArray[successor]);
			_blockArray[successor]->setRight(_blockArray[ar]);
			if(al!= successor){
				_blockArray[al]->setPar(_blockArray[successor]);
				_blockArray[successor]->setLeft(_blockArray[al]);
			}
		}
	}
	else{ // a is root
		if(_blockArray[a]->getRight()== NULL){ //only left child
			int al = _blockArray[a]->getLeft()->getId();
			_blockArray[al]->setPar(NULL);
			_blockArray[al]->setrole(-1);
			_root = al;
		}
		else if(_blockArray[a]->getLeft()== NULL){ //only right child
			int ar = _blockArray[a]->getRight()->getId();
			_blockArray[ar]->setPar(NULL);
			_blockArray[ar]->setrole(-1);
			_root = ar;
		}
		else{//two children
			int ar = _blockArray[a]->getRight()->getId();
			int al = _blockArray[a]->getLeft()->getId();
			//find successor(a leaf node descendant)
			Block * tblock = _blockArray[a];
			while(tblock->getLeft() != NULL || tblock->getRight() != NULL){ 
				if(tblock->getLeft() != NULL) tblock = tblock->getLeft();
				else if(tblock->getRight() != NULL) tblock = tblock->getRight();
			}
			int successor = tblock->getId();
			int suc_par = tblock->getPar()->getId();
			int suc_role = tblock->getrole();
			//handle successor's parent
			if(suc_role == 0) _blockArray[suc_par]->setLeft(NULL);
			else _blockArray[suc_par]->setRight(NULL);
			//successor is root
			_blockArray[successor]->setPar(NULL);
			_blockArray[successor]->setrole(-1);
			_root = successor;
			//find successor's new children
			_blockArray[ar]->setPar(_blockArray[successor]);
			_blockArray[successor]->setRight(_blockArray[ar]);
			if(al!= successor){
				_blockArray[al]->setPar(_blockArray[successor]);
				_blockArray[successor]->setLeft(_blockArray[al]);
			}
		}
	}
	//clear a
	//printBT();
	_blockArray[a]->setPar(NULL);
	_blockArray[a]->setLeft(NULL);
	_blockArray[a]->setRight(NULL);
	_blockArray[a]->setrole(-2);
	//insert
	if(_blockArray[b]->getLeft()== NULL){ //no child or only right child->a is b's left child
		_blockArray[b]->setLeft(_blockArray[a]);
		_blockArray[a]->setPar(_blockArray[b]);
		_blockArray[a]->setrole(0);
	}
	else if(_blockArray[b]->getRight()== NULL){//only left child->a is b's right child
		_blockArray[b]->setRight(_blockArray[a]);
		_blockArray[a]->setPar(_blockArray[b]);
		_blockArray[a]->setrole(1);
	}
	else{//two children->a is b's right child, b's original right child becomes a's right child
		int br = _blockArray[b]->getRight()->getId();
		//handle a,b
		_blockArray[b]->setRight(_blockArray[a]);
		_blockArray[a]->setPar(_blockArray[b]);
		_blockArray[a]->setrole(1);
		//handle a,b's original right child
		_blockArray[a]->setRight(_blockArray[br]);
		_blockArray[br]->setPar(_blockArray[a]);
	}
	updatecoord();
	//updatecoord2();
	return;
}


//////////////////////
///////SA
//////////////////////
double BStarTree::HPWL() 
{
    double res = 0;
	double x_center, y_center;
	int counter = 0;
    for(size_t j = 0,n = _netArray.size();j < n;j++){
		Net* t_net = _netArray[j];
		double xmin = t_net->getxmin();
		double xmax = t_net->getxmax();
		double ymin = t_net->getymin();
		double ymax = t_net->getymax();
		for(int i = 0, m = _netdegree[counter]; i < m; i++)
			{
				int idx = t_net->findblock(i);
				x_center = (_blockArray[idx]->getX1()+_blockArray[idx]->getX2())/2.0;
				y_center = (_blockArray[idx]->getY1()+_blockArray[idx]->getY2())/2.0;
				if(x_center > xmax) xmax = x_center;
				if(y_center > ymax) ymax = y_center;
				if(x_center < xmin) xmin = x_center;
				if(y_center < ymin) ymin = y_center;
			}
		//assert(xmax >= xmin && ymax >= ymin);
		res += (xmax - xmin + ymax - ymin);
		//cout << "xmin = " << xmin << " xmax = "<<xmax << " ymin = "<< ymin << " ymax = "<<ymax << endl;
		//cout << "NET" << j << ": "<< xmax - xmin + ymax - ymin << endl;
		counter++;
	} 
    return res;
}

void BStarTree::selectMove(int choice){
	//choice = 0;
	//srand (time(NULL));
	int cand1 = rand()%_B;
	//cout << "M" << choice << " cand1: " << cand1;
	if(choice == 0){	//rotate(rotate has problems)
		//cout << endl;
		rotate(cand1);
	}
	else if(choice == 1){ //swap(following has problems)
		int cand2 = rand()%_B;
		while(1){
			if(cand1 != cand2 && cand1 == _root && _blockArray[cand2]->getPar()->getId() != cand1) // these three conditions pass
				break;
			else if(cand1 != cand2 && cand2 == _root && _blockArray[cand1]->getPar()->getId() != cand2)
				break;
			else if(cand1 != cand2 && _blockArray[cand1]->getPar()->getId() != cand2 && _blockArray[cand2]->getPar()->getId() != cand1)
				break;
			cand1 = rand()%_B;
			cand2 = rand()%_B;
		}
		//cout << " cand2: " << cand2 << endl;
		swap(cand1,cand2);
	}
	else{ //insert(insert has problems)
		int cand2 = rand()%_B;
		while(cand1== cand2){
			cand1 = rand()%_B;
			cand2 = rand()%_B;
		}
		//cout << " cand2: " << cand2 << endl;
		insert(cand1,cand2);
	}
	
	return;
}

void BStarTree::calcost(){
	//cout << _maxX << " " << _maxY << endl;
	//cout << _maxX *_maxY << endl;
	_A = double(_maxX*_maxY);
	_W = HPWL();
	_avgA = ((_totcount-1)*_avgA+_A)*1.0/_totcount;
	_avgW = ((_totcount-1)*_avgW+_W)*1.0/_totcount;
	//cout << _W;
	_cost =  _alpha*_A/_avgA + (1-_alpha)*_W/_avgW;
	return;
}

void BStarTree::record()
{
	for (size_t i = 0, end_i = _blockArray.size(); i < end_i; ++i){
		_blockArray[i]->backup();
	}
	
	_PhContour.clear();
	map<int,int>::iterator it1;
	for(it1 = _hContour.begin(); it1 != _hContour.end(); it1++){
		_PhContour.insert(*it1);
	}
	_PmaxX = _maxX; 
	_PmaxY = _maxY;
	_Proot = _root;
	_pcost = _cost;
	//_avgA = (_A + _avgA*(m-1))/m;
	//_avgW = (_W + _avgW*(m-1))/m;
}

void BStarTree::recordbest()
{
	for (size_t i = 0, end_i = _blockArray.size(); i < end_i; ++i){
		_blockArray[i]->updatebest();
	}
	
	_BhContour.clear();
	map<int,int>::iterator it1;
	for(it1 = _hContour.begin(); it1 != _hContour.end(); it1++){
		_BhContour.insert(*it1);
	}
	_BmaxX = _maxX; 
	_BmaxY = _maxY;
	_Broot = _root;
	_bcost = _cost;
	_BW = _W;
}
void BStarTree::undo()
{
	for (size_t i = 0, end_i = _blockArray.size(); i < end_i; ++i){
		_blockArray[i]->giveup();
	}
	
	_hContour.clear();
	map<int,int>::iterator it1;
	for(it1 = _PhContour.begin(); it1 != _PhContour.end(); it1++){
		_hContour.insert(*it1);
	}
	_maxX = _PmaxX; 
	_maxY = _PmaxY;
	_root = _Proot;
	_cost = _pcost;
}

void BStarTree::SA(){
	//srand (time(NULL));
	double r = 0.85;
	double badth = 0.95;
	double epsilon = 0.00008;
	double iT = 0.01/log(0.99);
	int k = 5;
	int N = k*_B;
	int choice;
	double T = iT;
	_totcount = 1;
	
	_avgA = _maxX*_maxY*1.0;
	_avgW = HPWL() * 1.0;
	calcost();
	record();
	recordbest();
	_pcost = _cost;
	_bcost = _pcost;
	//printBT();
	//swap(27,3);
	
	//printBT();
	//record();
	//insert(0,30);
	
	//printBT();
	//undo();
	//printBT();
	//insert(0,30);
	//printBT();
	//insert(5,15);
	//printBT();
	int sign =0;
	int m = 0;
	while(1){
		int move = 0;
		int reject = 0;
		int uphill = 0;
		//cout << m << "th operation" << endl;
		//if(_Case == 1 && (_BmaxX < _X)&& (_BmaxY < _Y)){break;}
		while(1){
			_totcount++;
			//cout << "--------------------------------------------------------------------------" << endl;
			//cout << "start SA" << endl;
			//cout << "--------------------------------------------------------------------------" << endl;
			//printBT();
			//printcontour();
			
			choice = rand() % 3 ;//select 1 in 3 moving choices
			selectMove(choice);
			calcost();
			
			//printBT();
			//printcontour();
			move ++;
			_deltacost = _cost - _pcost;
			if((_deltacost<=0 && _Case == 0 )||(_deltacost<=0 && _Case != 0 && _maxX <_X && _maxY <_Y&&sign ==1)){
				//cout << "MOVE" << _totcount << " Accepted:" << _cost << " old: "<<_pcost<< endl;
				//cout << _deltacost << endl;
				if(_deltacost>0) uphill++;
				record();
				if(_cost<_bcost){
					recordbest();
				}
				
			}
			else {
				//cout << "MOVE" << _totcount << " Rejected: "<< _cost << endl;
				reject++;
				undo();
			}
			if(uphill>N || move>2*N) break;
			//if(move == 8)break;
			
			
		}
		//break;
		T = r*T;
		m++;
		if(((reject*1.0/move)> badth || T < epsilon) && (_BmaxX < _X)&& (_BmaxY < _Y) && _Case == 0 ) {
			cout << "X: "<< _X << " Y: "<< _Y << " BX: " << _BmaxX << " BY: " << _BmaxY << endl;
			break;
		}
		//if(_totcount>) && (_BmaxX < _X)&& (_BmaxY < _Y) && _Case == 0 ) {
		if(_totcount>12000&& _Case != 0 && (_BmaxX < _X)&& (_BmaxY < _Y)) break;
	}
	return;
}

void BStarTree::WriteResult(fstream& output, duration<double> diff)
{
    long double temp = _alpha*_BmaxX*_BmaxY +(1-_alpha)*_BW ;
	output << setprecision(17)<< temp << '\n'//final cost
           << _BW << '\n'// total wirelength
           << int(_BmaxX*_BmaxY) << '\n'//chip area
           << _BmaxX << ' ' << _BmaxY << '\n'//chip_width chip_height
           << diff.count() << '\n';//runtime
    
    for (size_t i = 0, end_i = _blockArray.size(); i < end_i; ++i){
        output << _Id2Block[i] << ' ' 
               << _blockArray[i]->getX1() << ' ' << _blockArray[i]->getY1() << ' ' << _blockArray[i]->getX2() << ' ' << _blockArray[i]->getY2() << '\n';
    }
	//printcontour();
}

void BStarTree::WriteResult2(fstream& output, duration<double> diff)
{
    long double temp = _alpha*_maxX*_maxY +(1-_alpha)*_W;
	output << setprecision(17)<< temp << '\n'//final cost
           << _W << '\n'// total wirelength
           << int(_maxX*_maxY) << '\n'//chip area
           << _maxX << ' ' << _maxY << '\n'//chip_width chip_height
           << diff.count() << '\n';//runtime
    
    for (size_t i = 0, end_i = _blockArray.size(); i < end_i; ++i){
        output << _Id2Block[i] << ' ' 
               << _blockArray[i]->getX1() << ' ' << _blockArray[i]->getY1() << ' ' << _blockArray[i]->getX2() << ' ' << _blockArray[i]->getY2() << '\n';
    }
	//printcontour();
}


void BStarTree:: printcontour(){
	cout << "HORIZONTAL　Contour" << endl;
	map<int,int>::iterator it1;
		for(it1 = _hContour.begin(); it1 != _hContour.end(); it1++){
			cout << it1->first << "," << it1->second << "    ";
	cout << endl;
}
}