#include <iostream>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <random>
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

valueRange<uint32_t> maskToRange(uint32_t value, uint16_t mask_int) {
	uint32_t mask_bits = ((1 << mask_int) - 1) << (32 - mask_int);
	uint32_t start = value & mask_bits;
	uint32_t end = start + ~mask_bits + 1;
	return {start, end};
}

template<typename T>
T randomRange(valueRange<T> range) {
	random_device rd;
	mt19937 rng(rd());
	uniform_int_distribution<T> uni(range.start, range.end);
	return uni(rng);
}


ClassBenchLine::ClassBenchLine(ipPair ip1, ipPair ip2, intPair port1, intPair port2, intPair hex1, intPair hex2) {
	this->src_ip = ip1.ip;
	this->src_ip_mask = ip1.ip_width;
	this->dst_ip = ip2.ip;
	this->dst_ip_mask = ip2.ip_width;

	this->src_port_begin = port1.v1;
	this->src_port_end = port1.v2;
	this->dst_port_begin = port2.v1;
	this->dst_port_end = port2.v2;

	this->protocol = hex1.v1;
	this->protocol_mask = hex1.v2;
	// hex2 ignored
}

Rule ClassBenchLine::asRule() {
	valueRange<uint32_t> src_ip_range = maskToRange(this->src_ip, this->src_ip_mask);
	valueRange<uint32_t> dst_ip_range = maskToRange(this->dst_ip, this->dst_ip_mask);
	valueRange<uint16_t> protocol_range = maskToRange(this->protocol, this->protocol_mask);

	return Rule(src_ip_range, dst_ip_range, {this->src_port_begin, this->src_port_end},
		{this->dst_port_begin, this->dst_port_end}, protocol_range);

}

Rule::Rule(valueRange<uint32_t> src_ip, valueRange<uint32_t> dst_ip, valueRange<uint16_t> src_port, valueRange<uint16_t> dst_port, valueRange<uint16_t> protocol) {
	this->src_ip = src_ip;
	this->dst_ip = dst_ip;
	this->src_port = src_port;
	this->dst_port = dst_port;
	this->protocol = protocol;
}

packet Rule::sample() {
	uint32_t src_ip = randomRange(this->src_ip);
	uint32_t dst_ip = randomRange(this->dst_ip);
	uint16_t src_port = randomRange(this->src_port);
	uint16_t dst_port = randomRange(this->dst_port);
	uint16_t protocol = randomRange(this->protocol);

	return {src_ip, dst_ip, src_port, dst_port, protocol};
}

vector<ClassBenchLine> parse_classbench(string filename) {
	ifstream cbfile (filename);
	string cb_string;
	vector<ClassBenchLine> results;

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
			break;
		}
	}
	return results;
}

int main(int argc, char** argv) {
	parse_classbench("classbench/acl2_1k");
}