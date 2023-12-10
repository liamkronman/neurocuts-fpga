#include <verilated.h>
#include <time.h>
#include <memory.h>
#include <random>
#include <iostream>
#include "Vclassifier.h"
#include "classbench_parser.h"
#include <immintrin.h>

using ClassifierPtr = std::unique_ptr<Vclassifier>;

int randomInt(int min, int max) {
    random_device rd;
    mt19937 rng(rd());
    uniform_int_distribution<int> uni(min, max);
    return uni(rng);
}

void sendInputPacket(ClassifierPtr& classifier, packet p) {
    classifier->src_ip = p.src_ip;
    classifier->dst_ip = p.dst_ip;
    classifier->src_port = p.src_port;
    classifier->dst_port = p.dst_port;
    classifier->protocol = p.protocol;
}

Rule getOutputRule(ClassifierPtr& classifier) {

    uint32_t src_ip_start = classifier->first_src_ip;
    uint16_t src_port_start = classifier->first_src_port;
    uint32_t dst_ip_start = classifier->first_dst_ip;
    uint16_t dst_port_start = classifier->first_dst_port;
    uint16_t protocol_start = classifier->first_protocol;

    uint32_t src_ip_last = classifier->last_src_ip;
    uint16_t src_port_last = classifier->last_src_port;
    uint32_t dst_ip_last = classifier->last_dst_ip;
    uint16_t dst_port_last = classifier->last_dst_port;
    uint16_t protocol_last = classifier->last_protocol;

    return Rule({src_ip_start, src_ip_last}, {dst_ip_start, dst_ip_last},
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
    Rule* expectedMatch;

    while (i < 10000) {
        classifier->clk ^= 1;
        classifier->eval();

        if (new_input) {
            if (i % 100 == 0) {
                std::cout << "Testing randomly sampled packets " << i << std::endl;
            }

            expectedMatch = nullptr;
            while (!expectedMatch) {
                p = (randomInt(0, 1) == 0) ? rules[randomInt(0, numRules-1)].sample() : generateRandomPacket();
                for (Rule rule : rules) {
                    if (rule.packetMatches(p)) {
                        expectedMatch = &rule;
                        break;
                    }
                }
            }
            sendInputPacket(classifier, p);
            classifier->input_is_valid = 1;

            new_input = false;
        } else {
            if (classifier->ready_to_process) {
                Rule actual_match = getOutputRule(classifier);

                if (expectedMatch != nullptr && !expectedMatch->equals(&actual_match)) {
                    std::cout << "failure -- actual: ";
                    printRule(&actual_match);
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