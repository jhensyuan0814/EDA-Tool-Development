#ifndef PARTITIONER_H
#define PARTITIONER_H

#include <fstream>
#include <vector>
#include <map>
#include <numeric>
#include "cell.h"
#include "net.h"
using namespace std;

class Partitioner
{
public:
    // constructor and destructor
    Partitioner(fstream& inFile) :
        _cutSize(0), _netNum(0), _cellNum(0), _maxPinNum(0), _bFactor(0),
        _accGain(0), _maxAccGain(0), _iterNum(0) {
        parseInput(inFile);
        _partSize[0] = 0;
        _partSize[1] = 0;
    }
    ~Partitioner() {
        clear();
    }

    // basic access methods
    int getCutSize() const          { return _cutSize; }
    int getNetNum() const           { return _netNum; }
    int getCellNum() const          { return _cellNum; }
    double getBFactor() const       { return _bFactor; }
    int getPartSize(int part) const { return _partSize[part]; }

    // modify method
    void parseInput(fstream& inFile);
    void partition();
    void insertNode(Node* node) ;
    void deleteNode(Node* node) ;
    
    // member functions about reporting
    void printSummary() const;
    void reportNet() const;
    void reportCell() const;
    void writeResult(fstream& outFile);
	bool checkbal(Node* node);
	int balcondition();
	double find_bal();
	void reportCellGain() const;
	void reportCellPart() const;
	void reportBucket() const;
	void reportNetPart() const;
	void ensureCutsize();

	//check balance after moving cell

private:
    int                 _cutSize;       // cut size$$$ 
    int                 _partSize[2];   // size (cell number) of partition A(0) and B(1)$$$
    int                 _netNum;        // number of nets
    int                 _cellNum;       // number of cells
    int                 _maxPinNum;     // Pmax for building bucket list
    double              _bFactor;       // the balance factor to be met
	double				_bestBfactor;
    Node*               _maxGainCell[2];   // pointer to max gain cell$$$
	Node*               _prevmaxGainCell[2];   // pointer to previous max gain cell$$$
    vector<Net*>        _netArray;      // net array of the circuit
    vector<Cell*>       _cellArray;     // cell array of the circuit
    map<int, Node*>     _bList[2];      // bucket list of partition A(0) and B(1)
    map<string, int>    _netName2Id;    // mapping from net name to id
    map<string, int>    _cellName2Id;   // mapping from cell name to id

    int                 _accGain;       // accumulative gain$$$
    int                 _maxAccGain;    // maximum accumulative gain$$$
    int                 _moveNum;       // number of cell movements$$$
    int                 _iterNum;       // number of iterations$$$
    int                 _bestMoveNum;   // store best number of movements$$$
    int                 _unlockNum[2];  // number of unlocked cells$$$
    vector<int>         _moveStack;     // history of cell movement$$$

    // Clean up partitioner
    void clear();
};

#endif  // PARTITIONER_H
