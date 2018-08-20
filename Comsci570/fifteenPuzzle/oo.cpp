#include <fstream> //ifstream
//#include <cmath> //std::abs
#include <cstdlib> //abs
#include <iostream> //cout


int main(int argc, char** argv) {
	int* input = new int[256];
	std::ifstream file(argv[1]);
	for(size_t i=0; i<16; i++) {
		for(size_t j=0; j<16; j++) {
			file >> input[i*16+j];
		}
	}
	file.close();
	for(size_t i=0; i<16; i++) {
		for(size_t j=0; j<16; j++) {
			std::cout<<input[i*16+j]<<", ";
		}
		std::cout<<"\n";
	}
}