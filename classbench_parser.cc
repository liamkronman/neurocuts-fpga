#include <time.h>
#include <memory.h>
#include <iostream>
#include <cstdint>
#include <fstream>
#include <string>

class ClassBenchLine {
	public:
		uint32_t ip_1;
		uint8_t ip_width_1;
		uint32_t ip_2;
		uint8_t ip_width_2;

		uint16_t min_port_1;
		uint16_t max_port_1;
		uint16_t min_port_2;
		uint16_t max_port_2;

		uint8_t first_hex_1;
		uint8_t second_hex_1;
		uint16_t first_hex_2;
		uint16_t second_hex_2;

		ClassBenchLine(std::string line) {
			std::cout << line;
		}
};

ClassBenchLine parseClassBenchFile(std::string filename) {
	std::ifstream cbfile (filename);
	std::string cb_string;

	if (cbfile.isopen()) {
		while (cbfile) {
			std::string ip1, ip2, p1a, p1b, p2a, p2b, hx1, hx2, _; 
			cbfile >> ip1;
			cbfile >> ip2;
			cbfile >> p1a;
			cbfile >> _;
			cbfile >> p1b;
			cbfile >> p2a;
			cbfile >> _;
			cbfile >> p2b;
			cbfile >> hx1;
			cbfile >> hx2;
			cb_string = ip1 + " " + ip2 + " " + p1a + " " + p1b + " " + p2a + " " + p2b + " " + hx1 + " " + hx2;
			return ClassBenchLine(cb_string);
		}
	}

	return ClassBenchLine(cb_string);
}

int main(int argc, char ** argv)
{
    parseClassBenchFile("classbench/acl2_1k");
}