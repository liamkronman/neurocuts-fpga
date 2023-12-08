#include <verilated.h>
#include <time.h>
#include <memory.h>
#include <random>
#include <iostream>
#include "Vclassifier.h"
#include "classbench_parser.h"

using ClassifierPtr = std::unique_ptr<Vclassifier>;

int randomInt(int min, int max) {
    random_device rd;
    mt19937 rng(rd());
    uniform_int_distribution<int> uni(min, max);
    return uni(rng);
}

VlWide<4> packPacket(packet p) {
    //src ip 33, src port 17, dst ip 33, dst port 17, protocol 9

    uint32_t elt1 = p.src_ip;
    uint32_t elt2 = (p.src_port << 1) + (p.dst_ip << 18);
    uint32_t elt3 = (p.dst_ip >> 14) + (p.dst_port << 19);
    uint32_t elt4 = (p.dst_port >> 13) + (p.protocol << 4);

    return {{elt1, elt2, elt3, elt4}};
}

Rule* unpackRule(VlWide<8> r) {
    // start src ip 33, start src port 17, start dst ip 33, start dst port 17, start protocol 9,
    // last src ip 33, last src port 17, last dst ip 33, last dst port 17, last protocol 9,
    // weight 32 (ignored)

    uint32_t src_ip_start = r.m_storage[0];
    uint16_t src_port_start = (r.m_storage[1] >> 1) & 0xFFFF;
    uint32_t dst_ip_start = ((r.m_storage[1] >> 18) & 0x3FFF) + ((r.m_storage[2] & 0x3FFFF) << 14);
    uint16_t dst_port_start = ((r.m_storage[2] >> 19) & 0x1FFF) + ((r.m_storage[3] & 0x7) << 13);
    uint16_t protocol_start = ((r.m_storage[3] >> 4) & 0xFF);

    uint32_t src_ip_last = ((r.m_storage[3] >> 13) & 0x7FFFF) + ((r.m_storage[4] & 0x1FFF) << 19);
    uint16_t src_port_last = (r.m_storage[4] >> 14) & 0xFFFF;
    uint32_t dst_ip_last = ((r.m_storage[4] >> 31) & 0x1) + ((r.m_storage[5] & 0x7FFFFFFF) << 1);
    uint16_t dst_port_last = r.m_storage[6] & 0xFFFF;
    uint16_t protocol_last = (r.m_storage[6] >> 17) & 0xFF;

    return new Rule({src_ip_start, src_ip_last}, {dst_ip_start, dst_ip_last},
                    {src_port_start, src_port_last}, {dst_port_start, dst_port_last},
                    {protocol_start, protocol_last});
}

void printRule(Rule* r) {
    if (r == nullptr) {
        std::cout << "{null}";
    } else {
        std::cout << "{" << r->src_ip.start << "-" << r->src_ip.end << ", " <<
                            r->dst_ip.start << "-" << r->dst_ip.end << ", " <<
                            r->src_port.start << "-" << r->src_port.end << ", " <<
                            r->dst_port.start << "-" << r->dst_port.end << ", " <<
                            r->protocol.start << "-" << r->protocol.end << "}";
    }
}

void Init(ClassifierPtr& classifier) {
    classifier->reset = 1;
    classifier->clk = 0;
    classifier->eval();
    classifier->clk = 1;
    classifier->eval();
    classifier->reset = 0;
}

bool test_classbench(ClassifierPtr& classifier, std::vector<Rule> const& rules)
{
    int failures = 0;
    int numRules = rules.size();

    int i = 0;
    bool new_input = true;
    packet p;

    while (i < 10000) {
        classifier->clk ^= 1;
        classifier->eval();

        if (new_input) {
            if (i % 100 == 0) {
                std::cout << "Testing randomly sampled packets " << i << std::endl;
            }

            p = (randomInt(0, 1) == 0) ? rules[randomInt(0, numRules-1)].sample() : generateRandomPacket();
            VlWide<4> test = packPacket(p);
            classifier->packet = test;
            classifier->input_is_valid = 1;

            new_input = false;
        } else {
            if (classifier->ready_to_process) {
                Rule* expectedMatch = nullptr;
                for (Rule rule : rules) {
                    if (rule.packetMatches(p)) {
                        expectedMatch = &rule;
                        break;
                    }
                }

                VlWide<8> mrs = classifier->matched_rule_storage;
                Rule* actual_match = unpackRule(mrs);

                if (expectedMatch != nullptr && !expectedMatch->equals(actual_match)) {
                    std::cout << "failure -- actual: ";
                    printRule(actual_match);
                    std::cout << ", expected: ";
                    printRule(expectedMatch);
                    std::cout << std::endl;
                    failures += 1;
                }

                i += 1;
                new_input = true;
            }
        }
    }

    return failures == 0;
}

int main(int argc, char ** argv)
{
    Verilated::commandArgs(argc, argv);
    auto rules = parse_classbench_to_rule(std::string{argv[1]});

    // auto p = rules[0].sample();
    // std::cout << p.src_ip << " " << p.dst_ip << " " << p.src_port << " " << p.dst_port << " " << p.protocol << std::endl;
    // std::cout << rules[0].packetMatches(p) << std::endl;

    auto classifier = std::make_unique<Vclassifier>();
    Init(classifier);
    if (!test_classbench(classifier, rules)) {
        return -1;
    }
}