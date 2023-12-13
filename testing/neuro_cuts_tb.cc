#include <verilated.h>
#include <time.h>
#include <memory.h>
#include <random>
#include <iostream>
#include "Vclassifier.h"
#include "classbench_parser.h"
#include <immintrin.h>

using ClassifierPtr = std::unique_ptr<Vclassifier>;

const int TESTING_ITERATIONS = 500;

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

    double current_cycles = 0;
    double average_cycles = 0;

    int i = 0;
    bool new_input = true;
    packet p;
    Rule* expectedMatch;
    std::vector<double> samples;

    while (i < TESTING_ITERATIONS) {
        classifier->clk ^= 1;
        classifier->eval();
        current_cycles += 0.5;

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

            current_cycles = 0;
            // get it into low state
            if (classifier->clk == 1) {
                classifier->clk = 0;
                classifier->eval();
            }
            sendInputPacket(classifier, p);
            classifier->input_is_valid = 1;
            classifier->clk ^= 1;
            classifier->eval();
            classifier->input_is_valid = 0;
            current_cycles += 1;

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
                else{
                    std::cout << "success -- actual: ";
                    printRule(&actual_match);
                    std::cout << ", expected: ";
                    printRule(expectedMatch);
                    std::cout << std::endl;
                }

                average_cycles += current_cycles / TESTING_ITERATIONS;
                samples.push_back(current_cycles);
                i += 1;
                new_input = true;
            }
        }
    }

    std::cout << "Average cycles per match: " << average_cycles << std::endl;
    double min_cycle = 1000000, max_cycle = 0;
    for (int i = 0; i < samples.size(); i++) {
        if (samples[i] < min_cycle) min_cycle = samples[i];
        if (samples[i] > max_cycle) max_cycle = samples[i];
    }
    std::cout << "min: " << min_cycle << std::endl;
    std::cout << "max: " << max_cycle << std::endl;
    return failures == 0;
}

int main(int argc, char ** argv)
{
    Verilated::commandArgs(argc, argv);
    std::vector<Rule> rules = std::vector<Rule>();
    for (int i = 1; i < argc; i++) {
        std::vector<Rule> new_rules = parse_classbench_to_rule(std::string{argv[i]});
        rules.insert(rules.begin(), new_rules.begin(), new_rules.end());
    }

    // auto p = rules[0].sample();
    // std::cout << p.src_ip << " " << p.dst_ip << " " << p.src_port << " " << p.dst_port << " " << p.protocol << std::endl;
    // std::cout << rules[0].packetMatches(p) << std::endl;

    auto classifier = std::make_unique<Vclassifier>();
    Init(classifier);
    if (!test_classbench(classifier, rules)) {
        return -1;
    }
}