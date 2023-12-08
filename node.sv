
import network_pkg::*;
import tree_constants::*;

package node_pkg;
    parameter PARTITION = 0;
    parameter CUT = 1;
    parameter LEAF = 2;

    typedef struct packed {
        logic[1:0] node_type;
        rule_s range;
        logic[31:0] rule_count;
        rule_s[MAX_RULES_PER_NODE-1:0] rules;
        logic[31:0] child_count;
        logic[31:0][MAX_CHILDREN_PER_NODE-1:0] children;
    } node_s;

    parameter NODE_TYPE_OFFSET = 0;
    parameter RANGE_OFFSET = $bits(logic[1:0]);
    parameter RULE_COUNT_OFFSET = RANGE_OFFSET + $bits(rule_s);
    parameter RULES_OFFSET = RULE_COUNT_OFFSET + $bits(logic[31:0]);
    parameter CHILD_COUNT_OFFSET = RULES_OFFSET + $bits(rule_s) * MAX_RULES_PER_NODE;
    parameter CHILDREN_OFFSET = CHILD_COUNT_OFFSET + $bits(logic[31:0]);
endpackage

import node_pkg::*;

module reinterpret_bits_as_node(
    input logic[$bits(node_s)-1:0] node_data,
    output node_s node
);
    assign node.node_type = node_data[$bits(logic[1:0])-1+NODE_TYPE_OFFSET:NODE_TYPE_OFFSET];
    reinterpret_bits_as_rule my_interpret(
        .bits(node_data[$bits(rule_s)-1 + RANGE_OFFSET:RANGE_OFFSET]),
        .rule_out(node.range)
    );
    assign node.rule_count = node_data[$bits(logic[31:0])-1+RULE_COUNT_OFFSET:RULE_COUNT_OFFSET];
    // add rules
    genvar j;
    generate
        for (j = 0; j < MAX_RULES_PER_NODE; j = j +1) begin
            reinterpret_bits_as_rule my_interpret(
                .bits(node_data[$bits(rule_s)-1 + j*$bits(rule_s)+RULES_OFFSET: j*($bits(rule_s))+RULES_OFFSET]),
                .rule_out(node.rules[j])
            );
        end
    endgenerate
    assign node.child_count = node_data[$bits(logic[31:0])-1+CHILD_COUNT_OFFSET:CHILD_COUNT_OFFSET];
    generate 
        //add children
        for (j = 0; j < MAX_CHILDREN_PER_NODE; j = j+1) begin
            assign node.children[j] = node_data[31+j*32 + CHILDREN_OFFSET : j*32 + CHILDREN_OFFSET];
        end 
    endgenerate
endmodule
