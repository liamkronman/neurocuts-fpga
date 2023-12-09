
import network_pkg::*;
import tree_constants::*;
import node_pkg::*;

module leaf_match(
    input node_s node,
    input packet_s packet,
    output logic[31:0] rule_index,
    output logic found_rule
);
    logic matched[MAX_RULES_PER_NODE-1:0];
    genvar i;
    generate
        for (i = 0; i < MAX_RULES_PER_NODE; i = i + 1) begin
            logic idx_matches;
            rule_match my_match(
                .rule(node.rules[i]), .packet(packet),
                .matched(idx_matches)
            );
            assign matched[i] = idx_matches && i < node.rule_count;
        end
    endgenerate
    always_comb begin
        found_rule = 0;
        rule_index = 0;
        for (int j = 0; j < MAX_RULES_PER_NODE; j++) begin
            if (matched[j]) begin
                rule_index = j;
                found_rule = 1;
                break;  // Exit the loop once the first match is found
            end
        end
    end
endmodule


module cut_match(
    input logic[$bits(node_s)-1:0] node_data [0:TOTAL_NODES-1],
    input node_s node,
    input packet_s packet,

    output logic matched[MAX_CHILDREN_PER_NODE-1:0],
    output logic[31:0] child_index,
    output logic found_child
);
    genvar i;
    generate
        for (i = 0; i < MAX_CHILDREN_PER_NODE; i = i + 1) begin
            node_s child_node;
            logic idx_matched;
            assign child_node = node_data[node.children[i]];
            rule_match my_match(
                .rule(child_node.range), .packet(packet),
                .matched(idx_matched)
            );
            assign matched[i] = idx_matched; //&& i < children_nodes[i].child_count;
        end
    endgenerate
    always_comb begin
        found_child = 0;
        child_index = 0;
        for (int j = 0; j < MAX_CHILDREN_PER_NODE; j++) begin
            if (matched[j]) begin
                child_index = j;
                found_child = 1;
                break;  // Exit the loop once the first match is found
            end
        end
    end
endmodule

// if ready is true, pass in a packet and a single cycle of valid.
// ready will turn off, and when it's matched, ready_to_process will be true again
module classifier(
    input logic clk,
    input logic reset,
    input logic[$bits(packet_s)-1:0] packet,
    input logic input_is_valid,

    output logic ready_to_process,
    output logic[$bits(rule_s)-1:0] matched_rule_storage
);
    // 32 bits large enough to hold index of any node
    logic[$bits(node_s)-1:0] node_data [0:TOTAL_NODES-1];
    logic[$bits(packet_s)-1:0] internal_packet;
    logic[31:0] current_node_idx;
    logic processing;
    node_s current_node;
    rule_s matched_rule;
    assign matched_rule_storage = matched_rule;

    node_s root;
    assign root = node_data[0];
    initial begin
        $display("Size of rule_s in bits: %d", $bits(rule_s));
        $display("Size of node_s in bits: %d", $bits(node_s));
        $readmemb("./nodes.txt", node_data);
        $display("initialized node_data");
    end
    
    logic[31:0] rule_index;
    logic found_rule;
    leaf_match my_leaf_match(
        .node(current_node), .packet(internal_packet),
        .rule_index(rule_index), .found_rule(found_rule)
    );
    
    logic[31:0] child_index;
    logic found_child;
    logic cut_matches[MAX_CHILDREN_PER_NODE-1:0];
    cut_match my_cut_match(
        .node_data(node_data),
        .node(current_node), .packet(internal_packet),
        .child_index(child_index), .found_child(found_child),
        .matched(cut_matches)
    );

    // 370 iterations cause we need to see that were invalid
    always_ff @(posedge clk) begin
        if (reset) begin
            ready_to_process <= 1;
            processing <= 0;
            matched_rule <= 0;
        end
        else begin
            if (input_is_valid && ready_to_process) begin
                internal_packet <= packet;
                ready_to_process <= 0;
                current_node_idx <= 0;
                current_node <= node_data[0];
                processing <= 1;
                matched_rule.weight <= {31{1'b1}};
            end
            // will start cycle after requested to process
            if (processing) begin
                $display("node:%d, rule_count:%d, child_count:%d", current_node_idx, current_node.rule_count, current_node.child_count);
                print_node(current_node);
                print_packet(internal_packet);
                $display("\nchildren start:");
                for (int i = 0; i < current_node.child_count; i++) begin
                    print_node(node_data[current_node.children[MAX_CHILDREN_PER_NODE-1-i]]);
                end
                $display("child end^\n");
                if (current_node.node_type == PARTITION) begin
                    $display("found partition uhoh");
                end
                else if (current_node.node_type == CUT) begin
                    $display("At a cut node. Found Child: %d", found_child);
                    $write("matched: {");
                    for (int i = 0; i < MAX_CHILDREN_PER_NODE; i++) begin
                        $write("%0b", cut_matches[i]);
                    end
                    $display("}");
                    if (found_child) begin
                        current_node_idx <= child_index;
                        current_node <= node_data[child_index];
                    end
                end
                else if (current_node.node_type == LEAF) begin
                    if (found_rule && current_node.rules[rule_index].weight < matched_rule.weight) begin
                        matched_rule <= current_node.rules[rule_index];
                    end
                end
                else begin
                    $display("Error: This should be unreachable");
                end
            end
        end
    end
endmodule