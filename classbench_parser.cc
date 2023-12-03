#include <iostream>
#include <cstdint>
#include <fstream>
#include <string>

using namespace std;

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

		ClassBenchLine(string line) {
			cout << line << endl;
		}
};

struct ipPair{
	uint32_t ip;
	uint8_t ip_width;
};

struct int8Pair{
	uint8_t v1;
	uint8_t v2;
};

struct int16Pair{
	uint16_t v1;
	uint16_t v2;
};

ClassBenchLine parseClassBenchFile(string filename) {
	ifstream cbfile (filename);
	string cb_string;

	if (cbfile.is_open()) {
		while (cbfile) {
			string ip1, ip2, p1a, p1b, p2a, p2b, hx1, hx2, _; 
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

			cout << parseIpString(ip1.substr(1, ip1.length())) << endl;
			cout << parseIpString(ip2) << endl;

			return ClassBenchLine(cb_string);
		}
	}
	return ClassBenchLine(cb_string);
}

ipPair parseIpString(string ipString) {
	int slash = ipString.find("/");
	string ips = ipString.substr(0, slash) + ".";
	string ipw = ipString.substr(slash+1, ipString.length());

	uint32_t ip_int = 0;
	for (int dot = ips.find("."); dot != -1; dot = ips.find(".")) {
		string ippart = ips.substr(0, dot);
		ip_int = (ip_int << 8) + stoi(ippart);
		ips = ips.substr(dot+1, ips.length());
	}

	ipPair = {ip_int, stoi(ipw)};
	return ipPair;
}

int main(int argc, char ** argv)
{
    parseClassBenchFile("classbench/acl2_1k");
}