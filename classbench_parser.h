#ifndef CLASS_BENCH_PARSER
#define CLASS_BENCH_PARSER
#include <stdint.h>
#include <string>
#include <vector>

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
		uint32_t src_ip;
		uint8_t src_ip_mask;
		uint32_t dst_ip;
		uint8_t dst_ip_mask;

		uint16_t src_port_begin;
		uint16_t src_port_end;
		uint16_t dst_port_start;
		uint16_t dst_port_end;

		uint16_t protocol;
		uint16_t protocol_mask;

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
		}
};

std::vector<ClassBenchLine> parse_classbench(std::string filename);

#endif