#include <iostream>
#include <cstdint>
#include <fstream>
#include <string>

using namespace std;


struct ipPair{
	uint32_t ip;
	uint16_t ip_width;
};

struct intPair{
	uint16_t v1;
	uint16_t v2;
};

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

		uint16_t first_hex_1;
		uint16_t second_hex_1;
		uint16_t first_hex_2;
		uint16_t second_hex_2;

		ClassBenchLine(ipPair ip1, ipPair ip2, intPair port1, intPair port2, intPair hex1, intPair hex2) {
			this->ip_1 = ip1.ip;
			this->ip_width_1 = ip1.ip_width;
			this->ip_2 = ip2.ip;
			this->ip_width_2 = ip2.ip_width;

			this->min_port_1 = port1.v1;
			this->max_port_1 = port1.v2;
			this->min_port_2 = port2.v1;
			this->max_port_2 = port2.v2;

			this->first_hex_1 = hex1.v1;
			this->second_hex_1 = hex1.v2;
			this->first_hex_2 = hex2.v1;
			this->second_hex_2 = hex2.v2;

			cout << ip1.ip << " " << ip1.ip_width << " " << ip2.ip << " " << ip1.ip_width << " " << port1.v1 << " " << port1.v2 << " " << port2.v1 << " " << port2.v2 << " " << hex1.v1 << " " << hex1.v2 << " " << hex2.v1 << " " << hex2.v2 << " " << endl;
		}
};

// Linked List
class CBLNode {
	private:
		CBLNode* next;
		ClassBenchLine* value;

	public:
		CBLNode(ClassBenchLine* value) {
			this->next = nullptr;
			this->value = value;
		}

		ClassBenchLine* get(int idx) {
			if (idx == 0) {
				return this->value;
			} else {
				return this->next->get(idx-1);
			}
		}

		CBLNode* add(ClassBenchLine* value) {
			if (this->next == nullptr) {
				CBLNode* newNode = new CBLNode(value);
				this->next = newNode;
				return this->next;
			} else {
				return this->next->add(value);
			}
		}

		CBLNode* tail(int i) {
			if (i == 0) {
				return this;
			} else {
				return this->next->tail(i-1);
			}
		}
};

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

CBLNode* parseClassBenchFile(string filename) {
	ifstream cbfile (filename);
	string cb_string;
	CBLNode* resultHead = new CBLNode(nullptr);
	CBLNode* mrn = resultHead;

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

			ipPair ip1_pair = parseIpString(ip1.substr(1, ip1.length()));
			ipPair ip2_pair = parseIpString(ip2);

			intPair port1_pair = {stoi(p1a), stoi(p1b)};
			intPair port2_pair = {stoi(p2a), stoi(p2b)};

			intPair hex1_pair = parseHexPair(hx1);
			intPair hex2_pair = parseHexPair(hx2);

			ClassBenchLine* cbl = new ClassBenchLine(ip1_pair, ip2_pair, port1_pair, port2_pair, hex1_pair, hex2_pair);
			mrn = mrn->add(cbl);
		}
	}

	return resultHead->tail(1);
}

int main(int argc, char ** argv)
{
    parseClassBenchFile("classbench/acl2_1k");
}