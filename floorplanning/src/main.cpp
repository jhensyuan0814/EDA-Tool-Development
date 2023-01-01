#include <chrono>
#include "module.h"
using namespace std;
using namespace std::chrono;
int main(int argc, char** argv)
{
    auto start = high_resolution_clock::now();
	fstream input_blk, input_net, output;
    double alpha;

    if (argc == 5) {
        alpha = stod(argv[1]);
        input_blk.open(argv[2], ios::in);
        input_net.open(argv[3], ios::in);
        output.open(argv[4], ios::out);
        if (!input_blk) {
            cerr << "Cannot open the input file \"" << argv[2]
                 << "\". The program will be terminated..." << endl;
            exit(1);
        }
        if (!input_net) {
            cerr << "Cannot open the input file \"" << argv[3]
                 << "\". The program will be terminated..." << endl;
            exit(1);
        }
        if (!output) {
            cerr << "Cannot open the output file \"" << argv[4]
                 << "\". The program will be terminated..." << endl;
            exit(1);
        }
    }
    else {
        cerr << "Usage: ./Floorplanner <alpha> <input block file> " <<
                "<input net file> <output file>" << endl;
        exit(1);
    }
	cout << "--------------------------------------------------------------------------" << endl;
	cout << "start input parsing" << endl;
	cout << "--------------------------------------------------------------------------" << endl;
    BStarTree* BS = new BStarTree(input_blk, input_net,alpha);
	BS->read_block(input_blk);
	BS->read_nets(input_net);
	int temp = BS->getCase();
	if(temp == 1){BS->initialization2();}
	else if(temp ==2){BS->initialization3();}
	else{BS->initialization();}
	
	BS->updatecoord();
	//BS->updatecoord2();
	
	BS->SA();
	auto end = high_resolution_clock::now();
    auto diff = end - start;
    BS->WriteResult(output,duration<double>(diff));

    return 0;
}
