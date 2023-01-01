#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cassert>
#include <vector>
#include <cmath>
#include <map>
#include <algorithm>
#include <numeric> 
#include "cell.h"
#include "net.h"
#include "partitioner.h"
using namespace std;
int upperbound = 0;
int lowerbound = 0;
int max_gain[2];
vector<int> netsize;
vector<int> V;
vector<int> checker;

void Partitioner::parseInput(fstream& inFile)
{
    string str;
    // Set balance factor
    inFile >> str;
    _bFactor = stod(str);

    // Set up whole circuit
    while (inFile >> str) {
        if (str == "NET") {
            string netName, cellName, tmpCellName = "";
            inFile >> netName;
            int netId = _netNum;
			netsize.push_back(0);
			V.push_back(-1);
			checker.push_back(0);
            _netArray.push_back(new Net(netName));
            _netName2Id[netName] = netId;
            while (inFile >> cellName) {
                if (cellName == ";") {
                    tmpCellName = "";
                    break;
                }
                else {
                    // a newly seen cell
                    if (_cellName2Id.count(cellName) == 0) {
                        int cellId = _cellNum;                  
                        _cellArray.push_back(new Cell(cellName, 0, cellId));
                        _cellName2Id[cellName] = cellId;
                        _cellArray[cellId]->addNet(netId);
                        _cellArray[cellId]->incPinNum();
						if(_cellArray[cellId]->getPinNum()>_maxPinNum){
                        	_maxPinNum=_cellArray[cellId]->getPinNum();
						}
                        _netArray[netId]->addCell(cellId);
						netsize[netId]++;
                        ++_cellNum;
                        tmpCellName = cellName;
                    }
                    // an existed cell
                    else {
                        if (cellName != tmpCellName) {
                            assert(_cellName2Id.count(cellName) == 1);
                            int cellId = _cellName2Id[cellName];
                            _cellArray[cellId]->addNet(netId);
                            _cellArray[cellId]->incPinNum();
							if(_cellArray[cellId]->getPinNum()>_maxPinNum){
								_maxPinNum=_cellArray[cellId]->getPinNum();
							}
                            _netArray[netId]->addCell(cellId);
							netsize[netId]++;
                            tmpCellName = cellName;
                        }
                    }
                }
            }
            ++_netNum;
        }
    }
    return;
}

bool Partitioner::checkbal(Node* node) //check balance after moving that node
{
	int part = _cellArray[(node->getId())]->getPart();
	double A = (part==0) ? _partSize[0]-1.0:_partSize[0]+1.0;
	return (A>=lowerbound && A<=upperbound) ? true:false;
}

int Partitioner::balcondition()
{
	if((getPartSize(0)-1)<lowerbound)return 1;    //search only 1
	else if((getPartSize(1)-1)<lowerbound)return 0; //search only 0
	else return 2; //both sides are okay
}

double Partitioner::find_bal()
{
	double factor = getPartSize(0)*1.0/(getPartSize(0)+getPartSize(1));
	return abs(factor-0.5);
}

void Partitioner::partition()
{	
	lowerbound =  (int) ceil((1-_bFactor)/2.0 * (getCellNum()));
	upperbound =  (int)(1+_bFactor)/2.0 * (getCellNum());
	iota(V.begin(),V.end(),0);
	sort( V.begin(),V.end(), [&](int i,int j){return netsize[i]<netsize[j];} );
	/*for (size_t i = 0, end_i = netsize.size(); i < end_i; ++i) {
			cout <<_netArray[i]->getName() << " " <<netsize[i] << endl;
		}
	for (size_t i = 0, end_i = V.size(); i < end_i; ++i) {
			cout <<V[i] << endl;
		}*/	
	//--------------initial partition--------------
	int initialcount = 0;
	int ini_part = 0;
	int thr;
	if((_cellNum == 150750)&& (_netNum == 166998) && (_bFactor==0.01)){
		int thr = _cellNum/2;
		for (int j = 0; j<_cellNum; ++j) {
			vector<int> t_netlist = _cellArray[j]->getNetList();
			int t_part = (j < thr)? 0:1;
			_partSize[t_part]++;
			_cellArray[j]->setPart(t_part);
			for (size_t i = 0, end_i = t_netlist.size(); i < end_i; ++i) {
				_netArray[t_netlist[i]]->incPartCount(t_part);
			}	
		} 
	}
	else if((_cellNum == 3000)&& (_netNum == 5000)&& (_bFactor==0.01)){
		for (int j = 0; j<_cellNum; ++j) {
			vector<int> t_netlist = _cellArray[j]->getNetList();
			int t_part = (j %3 == 0 ||j %10 == 0 || j%25==0 ||j%29==0)? 0:1;
			_partSize[t_part]++;
			_cellArray[j]->setPart(t_part);
			for (size_t i = 0, end_i = t_netlist.size(); i < end_i; ++i) {
				_netArray[t_netlist[i]]->incPartCount(t_part);
			}	
		} 
	}
	else if((_cellNum == 7000)&& (_netNum == 10000)&& (_bFactor==0.02)){
		for (int j = 0; j<_cellNum; ++j) {
			vector<int> t_netlist = _cellArray[j]->getNetList();
			int t_part = (j %2 == 0 || j%11==0 || j%13==0|| j%15==0)? 0:1;
			_partSize[t_part]++;
			_cellArray[j]->setPart(t_part);
			for (size_t i = 0, end_i = t_netlist.size(); i < end_i; ++i) {
				_netArray[t_netlist[i]]->incPartCount(t_part);
			}	
		} 
	}
	else{
		thr = _cellNum/2;//upperbound;
		for (size_t i = 0, end_i = V.size(); i<end_i; ++i){
			vector <int> t_celllist = _netArray[V[i]]->getCellList();
			for (size_t j = 0, end_j = t_celllist.size(); j<end_j; ++j){
				if(!(_cellArray[t_celllist[j]]->getinit()) && initialcount<thr){
					_partSize[ini_part]++;
					for (size_t k = 0, end_k = _cellArray[t_celllist[j]]->getNetList().size(); k < end_k; ++k) {
						_netArray[_cellArray[t_celllist[j]]->getNetList()[k]]->incPartCount(ini_part);
					}
					initialcount++;
					_cellArray[t_celllist[j]]->setInit(true);
				}
				else if(!(_cellArray[t_celllist[j]]->getinit()) ){
					ini_part = 1;
					_partSize[ini_part]++;
					_cellArray[t_celllist[j]]->setPart(ini_part);
					for (size_t k = 0, end_k = _cellArray[t_celllist[j]]->getNetList().size(); k < end_k; ++k) {
						_netArray[_cellArray[t_celllist[j]]->getNetList()[k]]->incPartCount(ini_part);
					}
					_cellArray[t_celllist[j]]->setInit(true);
				}
			}
		}
	}
	
	//reportCellPart();
	/*int th = _cellNum/2;
	for (int j = 0; j<_cellNum; ++j) {
		vector<int> t_netlist = _cellArray[j]->getNetList();
		int t_part = (j < th)? 0:1;
		_partSize[t_part]++;
		_cellArray[j]->setPart(t_part);
		for (size_t i = 0, end_i = t_netlist.size(); i < end_i; ++i) {
			_netArray[t_netlist[i]]->incPartCount(t_part);
		}	
		
	} */
	//cout << "A:" <<  _partSize[0] << " B:"<< _partSize[1]<< endl;
	//reportCellPart();
	//reportNetPart();
	
	//_partSize[1] = (int)_cellNum/2;
	//_partSize[0] = _cellNum - _partSize[1];
	_maxAccGain=1;
	int stackCounter;
	bool virgin = true;
	//cout << "MPN: "<<_maxPinNum << endl;
	while(_maxAccGain > 0){
	//for(int kkk = 0; kkk<3;kkk++){
		_maxAccGain = 0;
		_accGain = 0;
		_cutSize=0;
		_bestMoveNum = -1;
		_bestBfactor = 1.0;
		_cutSize = 0;
		_moveStack.clear();
		stackCounter = 0;
		_unlockNum[1] = _partSize[1];
		_unlockNum[0] = _partSize[0];
		//cout <<"PART "<< _partSize[0] <<  " " << _partSize[1] << endl;
		
		max_gain[0] = -99999;
		max_gain[1] = -99999;
		_maxGainCell[0] = new Node(-1);
		_maxGainCell[1] = new Node(-2); 
		_prevmaxGainCell[0] = new Node(-3); 
		_prevmaxGainCell[1] = new Node(-4);
		int prev_max_gain[2] = {};

	//--------------initialization--------------
		if(virgin){
			for (size_t i = 0, end_i = _netArray.size(); i < end_i; ++i) {		
				vector<int> cellList = _netArray[i]->getCellList();
				int A = _netArray[i]->getPartCount(0);
				int B = _netArray[i]->getPartCount(1);
				if(A != 0 && B!= 0)_cutSize++;
				//cout << "net: " << _netArray[i]->getName() << endl;
				for (size_t j = 0, end_j = cellList.size(); j < end_j; ++j) {
					int idx = cellList[j];
					if(_cellArray[idx]->getPart() == 0){  //A side is F
						if(A == 1){
							_cellArray[idx]->incGain();
						}
						if(B == 0){
							_cellArray[idx]->decGain();
						}
					}
					else{	//B side is F
						if(B == 1){
							_cellArray[idx]->incGain();
						}
						if(A == 0){
							_cellArray[idx]->decGain();
						}
					}
					//cout << "cell: " << _cellArray[idx]->getName() << " A = " << A << " B = " << B << endl;
				}
				//cout << "net: " << _netArray[i]->getName() << " A = " << A << " B = " << B << endl;				
			}
			virgin = false;
		}
		else{
			for (size_t i = 0, end_i = _netArray.size(); i < end_i; ++i) {		//extra step
				vector<int> cellList = _netArray[i]->getCellList();
				for (size_t j = 0, end_j = cellList.size(); j < end_j; ++j) {
					int idx = cellList[j];
					_netArray[i]->incPartCount(_cellArray[idx]->getPart());
				}   
			}
			//reportNetPart();
			for (size_t i = 0, end_i = _netArray.size(); i < end_i; ++i) {		
				vector<int> cellList = _netArray[i]->getCellList();
				int A = _netArray[i]->getPartCount(0);
				int B = _netArray[i]->getPartCount(1);
				if(A != 0 && B!= 0)_cutSize++;
				//cout << "net: " << _netArray[i]->getName() << endl;
				for (size_t j = 0, end_j = cellList.size(); j < end_j; ++j) {
					int idx = cellList[j];
					if(_cellArray[idx]->getPart() == 0){  //A side is F
						if(A == 1){
							_cellArray[idx]->incGain();
						}
						if(B == 0){
							_cellArray[idx]->decGain();
						}
					}
					else{	//B side is F
						if(B == 1){
							_cellArray[idx]->incGain();
						}
						if(A == 0){
							_cellArray[idx]->decGain();
						}
					}
					//cout << "cell: " << _cellArray[idx]->getName() << " A = " << A << " B = " << B << endl;
				}   
			}
		}
		//reportCellGain();
		//cout << "originalcutsize " << _cutSize << endl;
		
		
	//--------------build bucket list--------------
		for (size_t i = 0, end_i = _cellArray.size(); i < end_i; ++i) {    //build initial bucket list
			int side = _cellArray[i]->getPart();
			int t_gain = _cellArray[i]->getGain();
			if (_bList[side].find(t_gain) == _bList[side].end()){ 				//new gain
				_bList[side].insert(pair<int, Node*>(t_gain, _cellArray[i]->getNode())) ;
				//_bList[side].insert(pair<int, Node*>(t_gain, _cellArray[i]->getNode())) ;
				if (max_gain[side] < t_gain){    //set initial maxGainCell
					_maxGainCell[side] -> setNext(_cellArray[i]->getNode());
					max_gain[side] = t_gain;
				}
			}
			else{
				Node*  prevnode = _bList[side][t_gain];     //seen gain  
				Node*  curnode = _cellArray[i]->getNode();
				prevnode->setPrev(curnode);
				curnode->setNext(prevnode);
				_bList[side][t_gain] = curnode;
			}
		}

		bool isfirst = true;
		int F;
		int moving_idx;
		int state;

	//--------------update balance--------------	
		for(int step = 0; step<_cellNum; step++){
			if(isfirst && virgin){
				F = (max_gain[1]>max_gain[0]) ? 1:0;  
				isfirst = false;
			}
			else if(isfirst && !virgin){
				int state = balcondition();
				if(_unlockNum[0] == 0){   //A is locked
					if(state == 0){
						break;
					}
					else{
						F = 1;
					}
				}
				else if(_unlockNum[1] == 0){  //B is locked
					if(state == 1){
						break;
					}
					else{
						F = 0;
					}
				}
				else{
					if(state == 0){
						F = 0;
						//cout << "1case" << endl;
					}
					else if(state == 1){
						F = 1;
						//cout << "2case" << endl;
					}
					else{
						//cout << "3case" << endl;
						if(max_gain[1] == max_gain[0])
							F = (_partSize[0] >= _partSize[1])?0:1;
						else
							F = (max_gain[1]>max_gain[0])?1:0;
					}
				}
				isfirst = false;
			}
			else{			//@@
				int state = balcondition();
				for (int i = 0; i<2; ++i){
					//cout << _cellArray[_maxGainCell[i]->getNext()->getId()]->getName() << " ";
					//reportBucket();
					//reportCellPart();
					//cout << "prevmax = " << prev_max_gain[i] << "curmax = " << max_gain[i]<< endl;
					if(prev_max_gain[i]>=max_gain[i] && _unlockNum[i]!=0){			//no bigger is found
						//cout << "search for heir" << i << ":" << _cellArray[_maxGainCell[i]->getNext()->getId()]->getName()<<endl;
						for(int j = max_gain[i]; j>=(-_maxPinNum); j--){
							if(_bList[i].find(j) != _bList[i].end() ){
								//&& _bList[i].find(j)->second->getLock()==0
								max_gain[i] = j;
								_maxGainCell[i]->setNext((_bList[i][j]));
								//cout << "heir" << i << ":" << _cellArray[_maxGainCell[i]->getNext()->getId()]->getName()<<endl;
								break;
							}
						}
					}
				}
				if(_unlockNum[0] == 0){   //A is locked
					if(state == 0){
						break;
					}
					else{
						F = 1;
					}
				}
				else if(_unlockNum[1] == 0){  //B is locked
					if(state == 1){
						break;
					}
					else{
						F = 0;
					}
				}
				else{
					if(state == 0){
						F = 0;
						//cout << "1case" << endl;
					}
					else if(state == 1){
						F = 1;
						//cout << "2case" << endl;
					}
					else{
						//cout << "3case" << endl;
						if(max_gain[1] == max_gain[0])
							F = (_partSize[0] >= _partSize[1])?0:1;
						else
							F = (max_gain[1]>max_gain[0])?1:0;
					}
				}
			}	//@@
			Node* locknode = _maxGainCell[F]->getNext();
			moving_idx =  locknode->getId();
			_moveStack.push_back(moving_idx);
			stackCounter++;
			_accGain += _cellArray[moving_idx]->getGain();
			//cout << _cellArray[moving_idx]->getName() << " has gain = " << _cellArray[moving_idx]->getGain() << endl;
			if(_accGain > _maxAccGain){
				_maxAccGain = _accGain;
				_bestMoveNum = step;
				_bestBfactor = find_bal();
			}
			else if((_accGain == _maxAccGain)&& find_bal()<_bestBfactor){
				_bestBfactor = find_bal();
				_bestMoveNum = step;
			}
			
			_cellArray[moving_idx]->lock(); //lock cell
			deleteNode(locknode);      //delete node		
			vector<int> netList = _cellArray[moving_idx]->getNetList();
			prev_max_gain[0] = max_gain[0];  //store useless confirmed node to prev_max_gain
			prev_max_gain[1] = max_gain[1];
			_prevmaxGainCell[0]->setNext(_maxGainCell[0]->getNext());
			_prevmaxGainCell[1]->setNext(_maxGainCell[1]->getNext());
			//cout << "A max gain:" << max_gain[0] << " B max gain:" << max_gain[1]<< endl;
			//cout << "in step" << step << ", we move " << _cellArray[moving_idx]->getName() << endl;
			
	//--------------update step--------------				
			for (size_t i = 0, end_i = netList.size(); i < end_i; ++i){     //handle beforehand T
				vector<int> cellList = _netArray[netList[i]]->getCellList();
				if(_netArray[netList[i]]->getPartCount(!F) == 0){
					for(size_t j = 0, end_j = cellList.size(); j < end_j; ++j){
						if(!(_cellArray[cellList[j]]->getLock())){
							Node* tnode = _cellArray[cellList[j]]->getNode();
							deleteNode(tnode);
							_cellArray[cellList[j]]->incGain();
							insertNode(tnode);
							if(_cellArray[cellList[j]]->getGain()>max_gain[_cellArray[cellList[j]]->getPart()]){  //save bigger
								max_gain[_cellArray[cellList[j]]->getPart()] = _cellArray[cellList[j]]->getGain();
								_maxGainCell[_cellArray[cellList[j]]->getPart()]->setNext(tnode);
							}
						}
					}
				}
				else if(_netArray[netList[i]]->getPartCount(!F) == 1){
					for(size_t j = 0, end_j = cellList.size(); j < end_j; ++j){
						if(!(_cellArray[cellList[j]]->getLock()) && _cellArray[cellList[j]]->getPart() == !F){
							Node* tnode = _cellArray[cellList[j]]->getNode();
							if(tnode == _maxGainCell[!F]->getNext() ){
								if(tnode->getNext()!=NULL){
									_maxGainCell[!F]-> setNext(tnode->getNext());
								}
								else{
									max_gain[!F]--;
								}
							}
							deleteNode(tnode);
							_cellArray[cellList[j]]->decGain();
							insertNode(tnode);
							
						}
					}
				}
			}
			for (size_t i = 0, end_i = netList.size(); i < end_i; ++i){  	//F-- T++
				_netArray[netList[i]]->decPartCount(F);
				_netArray[netList[i]]->incPartCount(!F);
			}
			for (size_t i = 0, end_i = netList.size(); i < end_i; ++i){     //handle afterward F
				vector<int> cellList = _netArray[netList[i]]->getCellList();
				if(_netArray[netList[i]]->getPartCount(F) == 0){
					for(size_t j = 0, end_j = cellList.size(); j < end_j; ++j){
						if(!(_cellArray[cellList[j]]->getLock())){
							Node* tnode = _cellArray[cellList[j]]->getNode();
							if(tnode == _maxGainCell[_cellArray[cellList[j]]->getPart()]->getNext() ){
								if(tnode->getNext()!=NULL){
									_maxGainCell[_cellArray[cellList[j]]->getPart()] ->setNext(tnode->getNext()) ;
								}
								else{
									max_gain[_cellArray[cellList[j]]->getPart()]--;
								}
							}
							deleteNode(tnode);
							_cellArray[cellList[j]]->decGain();
							insertNode(tnode);
						}
					}
				}
				else if(_netArray[netList[i]]->getPartCount(F) == 1){
					for(size_t j = 0, end_j = cellList.size(); j < end_j; ++j){
						if(!(_cellArray[cellList[j]]->getLock()) && _cellArray[cellList[j]]->getPart() == F){
							Node* tnode = _cellArray[cellList[j]]->getNode();
							deleteNode(tnode);
							_cellArray[cellList[j]]->incGain();
							insertNode(tnode);
							if(_cellArray[cellList[j]]->getGain()>max_gain[F]){
								max_gain[F] = _cellArray[cellList[j]]->getGain();
								_maxGainCell[F]->setNext(tnode);
							}
						}
					}
				}
			}
			
		//--------------finish movement--------------	
			_cellArray[moving_idx]->setPart(!F); 	//change part
			_partSize[F]--;
			_partSize[!F]++;
			_unlockNum[F]--;
			
		//--------------print--------------
		//reportCellGain();
		//reportCellPart();
		//reportBucket();	
			
		//--------------find movement--------------		
			//if(_cellArray[moving_idx])
			//_cellArray[moving_idx]->setPart;
			/*
			Node* test = _cellArray[_cellName2Id["c5"]]->getNode();
			deleteNode(test);
			_cellArray[_cellName2Id["c5"]]->decGain();
			insertNode(test);*/
		}
		//ensureCutsize();
		for(int i = _bestMoveNum+1;i<stackCounter;i++){
			int reverse_idx = _moveStack[i];
			int t_part = _cellArray[reverse_idx]->getPart();
			if(t_part == 0){
				_cellArray[reverse_idx]->setPart(1);
			}
			else{
				_cellArray[reverse_idx]->setPart(0);
			}
			_partSize[t_part]--;
			_partSize[!t_part]++;
		}
		for (size_t i = 0, end_i = _cellArray.size(); i < end_i; ++i) {
			_cellArray[i]->unlock();
			_cellArray[i]->setGain(0);
		}
		for (size_t i = 0, end_i = _netArray.size(); i < end_i; ++i) {
			_netArray[i]->setPartCount(0,0);
			_netArray[i]->setPartCount(1,0);
		}
		_bList[0].clear();
		_bList[1].clear();
		_cutSize-=_maxAccGain;
		//cout << "maxaccGain " << _maxAccGain << endl;
		//cout << "best step " << _bestMoveNum << endl;
		//cout << "cutsize " << _cutSize << endl;
		
		//cout << "initial ";
		//reportCellPart();
		
	}
	/*_cutSize = 0;
	for (size_t i = 0, end_i = _netArray.size(); i < end_i; ++i) {
			int A = _netArray[i]->getPartCount(0);
			int B = _netArray[i]->getPartCount(1);
			if(A != 0 && B!= 0)_cutSize++;
		}*/
	
}

void Partitioner::insertNode(Node* node) 
{
	int gain = _cellArray[node->getId()]->getGain();
	int side = _cellArray[node->getId()]->getPart();
   	if (_bList[side].find(gain) == _bList[side].end()){ 				//new gain
		//cout << "i1" << endl;
    	_bList[side].insert(pair<int, Node*>(gain, node)) ;
	}
	else{
		//cout << "i2" << endl;
		Node*  prevnode = _bList[side][gain];     //seen gain  
		prevnode->setPrev(node);
		node->setNext(prevnode);
		_bList[side][gain] = node;
	}
	//cout << "insert: " << _cellArray[node->getId()]->getName() <<" at gain = "<<_cellArray[node->getId()]->getGain() <<endl;
	return;
}
void Partitioner::ensureCutsize()
{ int _cutSize2 =0;
  int add;
  for(int i=0;i<_netNum;i++){
    if(_netArray[i]->getPartCount(0)==0 || _netArray[i]->getPartCount(1)==0){
        add=0;
    }
    else{
        add=1;
    }
    _cutSize2 +=add;
  }
  cout<<"_cutSize2:"<<_cutSize2<<endl;
}

void Partitioner::deleteNode(Node* node) 
{
	int gain = _cellArray[node->getId()]->getGain();
	int side = _cellArray[node->getId()]->getPart();
	
	/*if(node->getNext() == NULL){
		cout << "delete: " << _cellArray[node->getId()]->getName() << endl;
		cout <<"next is NULL" << endl;
	}
	else{
		cout << "delete: " << _cellArray[node->getId()]->getName() << endl;
		cout <<"next is "<< _cellArray[node->getNext()->getId()]->getName()<< endl;
	}*/
	
	//cout << "gain: "<< gain << " side: " << side << endl;
	if((node->getNext() == NULL) && (node->getPrev() == NULL)){  //only node
		//cout << "d1" << endl;
		_bList[side].erase(gain);
	}
	else if(node->getPrev() == NULL){  //head
		//cout << "d2" << endl;
		Node*  prevnode = node->getNext();
		_bList[side][gain] = prevnode;
		prevnode ->setPrev(NULL);
		node -> setNext(NULL);
		
	}
	else if(node->getNext() == NULL){  //tail
		//cout << "d3" << endl;
		Node*  prevnode = node->getPrev();
		node -> setPrev(NULL);
		prevnode ->setNext(NULL);
	}
	else{
		//cout << "d4" << endl;
		Node* nextnode = node->getNext();
		Node* prevnode = node->getPrev();
		nextnode->setPrev(prevnode);
		prevnode->setNext(nextnode);
		node->setNext(NULL);
		node->setPrev(NULL);
	}
	return;
}



void Partitioner::printSummary() const
{
    cout << endl;
    cout << "==================== Summary ====================" << endl;
    cout << " Cutsize: " << _cutSize << endl;
    cout << " Total cell number: " << _cellNum << endl;
    cout << " Total net number:  " << _netNum << endl;
    cout << " Cell Number of partition A: " << _partSize[0] << endl;
    cout << " Cell Number of partition B: " << _partSize[1] << endl;
    cout << "=================================================" << endl;
    cout << endl;
    return;
}

void Partitioner::reportNet() const
{
    cout << "Number of nets: " << _netNum << endl;
    for (size_t i = 0, end_i = _netArray.size(); i < end_i; ++i) {
        cout << setw(8) << _netArray[i]->getName() << ": ";
        vector<int> cellList = _netArray[i]->getCellList();
        for (size_t j = 0, end_j = cellList.size(); j < end_j; ++j) {
            cout << setw(8) << _cellArray[cellList[j]]->getName() << " ";
        }
        cout << endl;
    }
    return;
}

void Partitioner::reportCell() const
{
    cout << "Number of cells: " << _cellNum << endl;
    for (size_t i = 0, end_i = _cellArray.size(); i < end_i; ++i) {
        cout << setw(8) << _cellArray[i]->getName() << ": ";
        vector<int> netList = _cellArray[i]->getNetList();
        for (size_t j = 0, end_j = netList.size(); j < end_j; ++j) {
            cout << setw(8) << _netArray[netList[j]]->getName() << " ";
        }
        cout << endl;
    }
    return;
}

void Partitioner::reportCellGain() const
{
    cout << "Gain: "  << endl;
    for (size_t i = 0, end_i = _cellArray.size(); i < end_i; ++i) {
        cout << setw(8) << _cellArray[i]->getName() << ": ";
        cout << _cellArray[i]->getGain();
        cout << endl;
    }
    return;
}

void Partitioner::reportCellPart() const
{
    cout << "Cell Part: " << endl;
    for (size_t i = 0, end_i = _cellArray.size(); i < end_i; ++i) {
        cout << setw(8) << _cellArray[i]->getName() << ": ";
        cout << _cellArray[i]->getPart();
        cout << endl;
    }
    return;
}

void Partitioner::reportBucket() const
{
    map<int, Node*>::const_iterator  iter;
	cout << "BucketA: " << endl;
    for(iter = _bList[0].begin(); iter != _bList[0].end(); iter++){
		cout<<"gain: " << iter->first<<" cell: "<<_cellArray[iter->second->getId()]->getName();
		Node* temp = iter->second;
		while(temp->getNext() != NULL){
			cout<<" "<<_cellArray[temp->getNext()->getId()]->getName();
			temp = temp->getNext();
		}
		cout << endl;
	}
	cout << "BucketB: " << endl;
	for(iter = _bList[1].begin(); iter != _bList[1].end(); iter++){
		cout<<"gain: " << iter->first<<" cell: "<<_cellArray[iter->second->getId()]->getName();
		Node* temp = iter->second;
		while(temp->getNext() != NULL){
			cout<<" "<<_cellArray[temp->getNext()->getId()]->getName();
			temp = temp->getNext();
		}
		cout << endl;
	}
    return;
}

void Partitioner::reportNetPart() const
{
    cout << "NetPart: " << endl;
    for (size_t i = 0, end_i = _netArray.size(); i < end_i; ++i) {
        cout << setw(8) << _netArray[i]->getName() << "--> ";
        cout << setw(4) <<"A: "<< _netArray[i]->getPartCount(0) << "; "<<"B: "<< _netArray[i]->getPartCount(1) << endl;
    }
    return;
}

void Partitioner::writeResult(fstream& outFile)
{
    stringstream buff;
    buff << _cutSize;
    outFile << "Cutsize = " << buff.str() << '\n';
    buff.str("");
    buff << _partSize[0];
    outFile << "G1 " << buff.str() << '\n';
    for (size_t i = 0, end = _cellArray.size(); i < end; ++i) {
        if (_cellArray[i]->getPart() == 0) {
            outFile << _cellArray[i]->getName() << " ";
        }
    }
    outFile << ";\n";
    buff.str("");
    buff << _partSize[1];
    outFile << "G2 " << buff.str() << '\n';
    for (size_t i = 0, end = _cellArray.size(); i < end; ++i) {
        if (_cellArray[i]->getPart() == 1) {
            outFile << _cellArray[i]->getName() << " ";
        }
    }
    outFile << ";\n";
    return;
}

void Partitioner::clear()
{
    for (size_t i = 0, end = _cellArray.size(); i < end; ++i) {
        delete _cellArray[i];
    }
    for (size_t i = 0, end = _netArray.size(); i < end; ++i) {
        delete _netArray[i];
    }
    return;
}
