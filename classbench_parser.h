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

template<typename T>
struct valueRange{
	T start;
	T end;
};

struct packet{
	uint32_t src_ip;
	uint32_t dst_ip;
	uint16_t src_port;
	uint16_t dst_port;
	uint16_t protocol;
}


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

		ClassBenchLine(ipPair ip1, ipPair ip2, intPair port1, intPair port2, intPair hex1, intPair hex2);
};

class Rule {
	public:
		valueRange<uint32_t> src_ip;
		valueRange<uint32_t> dst_ip;
		valueRange<uint16_t> src_port;
		valueRange<uint16_t> dst_port;
		valueRange<uint16_t> protocol;

		Rule(valueRange<uint32_t> src_ip, valueRange<uint32_t> dst_ip, valueRange<uint16_t> src_port, valueRange<uint16_t> dst_port, valueRange<uint16_t> protocol);
		packet sample();
}

std::vector<ClassBenchLine> parse_classbench(std::string filename);

#endif