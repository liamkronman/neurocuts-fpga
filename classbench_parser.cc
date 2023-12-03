#include <iostream>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include "classbench_parser.h"

using namespace std;

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

	ipPair result = {ip_int, stoi(ipw)};
	return result;
}

intPair parseHexPair(string hexPairString) {
	int slash = hexPairString.find("/");
	string firstHex = hexPairString.substr(0, slash);
	string secondHex = hexPairString.substr(slash+1, hexPairString.length());

	intPair result = {stoul(firstHex, nullptr, 16), stoul(secondHex, nullptr, 16)};
	return result;
}

std::vector<ClassBenchLine> parse_classbench(std::string filename) {
	ifstream cbfile (filename);
	string cb_string;
	std::vector<ClassBenchLine> results;

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

			if (!cbfile) {break;}

			ipPair ip1_pair = parseIpString(ip1.substr(1, ip1.length()));
			ipPair ip2_pair = parseIpString(ip2);

			intPair port1_pair = {stoi(p1a), stoi(p1b)};
			intPair port2_pair = {stoi(p2a), stoi(p2b)};

			intPair hex1_pair = parseHexPair(hx1);
			intPair hex2_pair = parseHexPair(hx2);

			results.push_back(ClassBenchLine(ip1_pair, ip2_pair, port1_pair, port2_pair, hex1_pair, hex2_pair));
		}
	}
	return results;
}