
import network_pkg::*;
import tree_constants::*;
import node_pkg::*;

module leaf_match(
    input node_s node,
    input packet_s packet,
    output logic[31:0] rule_index,
    output logic found_rule,
    output logic matched[MAX_RULES_PER_NODE-1:0]
);
    logic actual_matched[MAX_RULES_PER_NODE-1:0];
    logic [31:0] min;
    assign min = (MAX_RULES_PER_NODE-1) - node.rule_count;
    genvar i;
    generate
        for (i = 0; i < MAX_RULES_PER_NODE; i = i + 1) begin
            logic idx_matches;
            rule_match my_match(
                .rule(node.rules[i]), .packet(packet),
                .matched(idx_matches)
            );
            assign actual_matched[i] = idx_matches && ((i+1) > (min+1));
            assign matched[i] = actual_matched[i];
        end
    endgenerate
    always_comb begin
        found_rule = 0;
        rule_index = 0;
        for (int j = 0; j < MAX_RULES_PER_NODE; j++) begin
            if (actual_matched[MAX_RULES_PER_NODE-1-j]) begin
                rule_index = MAX_RULES_PER_NODE-1-j;
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

    output logic[31:0] min,
    output logic matched[MAX_CHILDREN_PER_NODE-1:0],
    output logic[31:0] child_index,
    output logic found_child
);
    genvar i;
    generate
        for (i = 0; i < MAX_CHILDREN_PER_NODE; i = i + 1) begin
            node_s child_node;
            logic idx_matched;
            assign min = (MAX_CHILDREN_PER_NODE-1) - node.child_count;
            assign child_node = node_data[node.children[i]];
            rule_match my_match(
                .rule(child_node.range), .packet(packet),
                .matched(idx_matched)
            );
            assign matched[i] = idx_matched && ((i+1) > (min+1));
        end
    endgenerate
    always_comb begin
        found_child = 0;
        child_index = 0;
        for (int j = 0; j < MAX_CHILDREN_PER_NODE; j++) begin
            if (matched[MAX_CHILDREN_PER_NODE-1-j]) begin
                child_index = MAX_CHILDREN_PER_NODE-1-j;
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
    //incoming packet
    input logic input_is_valid,
    input logic[31:0] src_ip,
    input logic[31:0] dst_ip,
    input logic[15:0] src_port,
    input logic[15:0] dst_port,
    input logic[7:0] protocol,
    //out
    output logic ready_to_process,
    // output rule
    output logic[31:0] first_src_ip,
    output logic[31:0] first_dst_ip,
    output logic[15:0] first_src_port,
    output logic[15:0] first_dst_port,
    output logic[7:0] first_protocol,
    
    output logic[31:0] last_src_ip,
    output logic[31:0] last_dst_ip,
    output logic[15:0] last_src_port,
    output logic[15:0] last_dst_port,
    output logic[7:0] last_protocol
);
    // 32 bits large enough to hold index of any node
    logic[$bits(node_s)-1:0] node_data [0:TOTAL_NODES-1];
    logic[$bits(packet_s)-1:0] internal_packet;

    // current state
    logic node_push, node_pop;
    logic[31:0] node_stack_out;
    logic[31:0] node_stack_in;
    logic node_just_popped;
    logic processed_all_nodes;
    stack#(TOTAL_NODES, $bits(logic[31:0])) node_stack(
        .clk(clk), .reset(reset), .just_popped(node_just_popped),
        .data_in(node_stack_in), .data_out(node_stack_out),
        .push(node_push), .pop(node_pop),
        .empty(processed_all_nodes), .full()
    );
    logic[31:0] current_node_idx;
    node_s current_node;
    
    // input/output aliases
    rule_s matched_rule;
    packet_s packet;
    // flags
    logic processing;
    logic wants_new_node;
    // pushing state
    logic is_pushing;
    logic[31:0] pushing_index;

    always_comb begin
        first_src_ip = matched_rule.start.src.ip;
        first_dst_ip = matched_rule.start.dst.ip;
        first_src_port = matched_rule.start.src.port;
        first_dst_port = matched_rule.start.dst.port;
        first_protocol = matched_rule.start.protocol;

        last_src_ip = matched_rule.last.src.ip - 1;
        last_dst_ip = matched_rule.last.dst.ip - 1;
        last_src_port = matched_rule.last.src.port - 1;
        last_dst_port = matched_rule.last.dst.port - 1;
        last_protocol = matched_rule.last.protocol - 1;

        packet.src.ip = src_ip;
        packet.src.port = src_port;
        packet.dst.ip = dst_ip;
        packet.dst.port = dst_port;
        packet.protocol = protocol;
    end

    node_s root;
    assign root = node_data[0];
    initial begin
        //$display("Size of packet_s in bits: %d", $bits(packet_s));
        //$display("Size of rule_s in bits: %d", $bits(rule_s));
        //$display("Size of node_s in bits: %d", $bits(node_s));
        $readmemb("./nodes.txt", node_data);
        $display("initialized node_data");
    end
    
    logic[31:0] rule_index;
    logic found_rule;
    logic rule_matches[MAX_RULES_PER_NODE-1:0];
    leaf_match my_leaf_match(
        .node(current_node), .packet(internal_packet),
        .rule_index(rule_index), .found_rule(found_rule),
        .matched(rule_matches)
    );
    
    logic[31:0] child_index;
    logic found_child;
    logic cut_matches[MAX_CHILDREN_PER_NODE-1:0];
    logic[31:0] child_min;
    cut_match my_cut_match(
        .node_data(node_data),
        .node(current_node), .packet(internal_packet),
        .child_index(child_index), .found_child(found_child),
        .matched(cut_matches), .min(child_min)
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
                is_pushing <= 0;
                node_pop <= 0;
                node_push <= 0;
                wants_new_node <= 0;
                matched_rule <= 0;
                matched_rule.weight <= {31{1'b1}};
                print_packet(packet);
            end
            // will start cycle after requested to process
            if (processing) begin
                if (is_pushing) begin
                    if (1+pushing_index <= MAX_CHILDREN_PER_NODE-current_node.child_count) begin
                        //$display("didnt push at node: %d", current_node.children[pushing_index]);
                        node_push <= 0;
                        is_pushing <= 0;
                    end
                    else begin
                        //$display("Pushing node: %d", current_node.children[pushing_index]);
                        node_stack_in <= current_node.children[pushing_index];
                        pushing_index <= pushing_index - 1;
                        if (1+pushing_index <= MAX_CHILDREN_PER_NODE-current_node.child_count) begin
                            node_push <= 0;
                        end
                    end
                end
                else begin
                    if (wants_new_node) begin
                        if (node_just_popped) begin
                            //$display("Going to process: %d", node_stack_out);
                            node_pop <= 0;
                            current_node_idx <= node_stack_out;
                            current_node <= node_data[node_stack_out];
                            wants_new_node <= 0;
                        end
                        else if (processed_all_nodes) begin
                            ready_to_process <= 1;
                            wants_new_node <= 0;
                            processing <= 0;
                        end
                        // we already popped. only do it once
                        else if (node_pop) begin 
                            node_pop <= 0;
                        end
                        else begin
                            node_pop <= 1;
                        end
                    end
                    else begin
                        //$display("node:%d, rule_count:%d, child_count:%d", current_node_idx, current_node.rule_count, current_node.child_count);
                        //print_node(current_node);
                        //print_packet(internal_packet);
                        //$display("\nchildren start:");
                        for (int i = 0; i < current_node.child_count; i++) begin
                            //node_s child = node_data[current_node.children[MAX_CHILDREN_PER_NODE-1-i]];
                            //print_rule(child.range);
                            //print_node(node_data[current_node.children[]]);
                        end
                        //$display("child end^\n");
                        if (current_node.node_type == PARTITION) begin
                            // we can assume it has at least 1 child
                            //$display("At partition:");
                            is_pushing <= 1;
                            //$display("Pushing node: %d", current_node.children[MAX_CHILDREN_PER_NODE - 1]);
                            pushing_index <= MAX_CHILDREN_PER_NODE - 2;
                            node_stack_in <= current_node.children[MAX_CHILDREN_PER_NODE - 1];
                            node_push <= 1;
                            wants_new_node <= 1;
                        end
                        else if (current_node.node_type == CUT) begin
                            //$display("At cut node:");
                            //$display("At a cut node. Found Child: %d", found_child);
                            //$display("Child min: %d", child_min);
                            //$display("Child min+1: %d", child_min+1);
                            $write("matched: {");
                            for (int i = 0; i < MAX_CHILDREN_PER_NODE; i++) begin
                                $write("%0b", cut_matches[i]);
                            end
                            //$display("}");
                            if (found_child) begin
                                //$display("Child index: %d", child_index);
                                current_node_idx <= current_node.children[child_index];
                                current_node <= node_data[current_node.children[child_index]];
                                //$display("Next Node Index: %d", current_node_idx);
                            end
                            else begin
                                wants_new_node <= 1;
                            end
                        end
                        else if (current_node.node_type == LEAF) begin
                            $write("matched: {");
                            for (int i = 0; i < MAX_RULES_PER_NODE; i++) begin
                                $write("%0b", rule_matches[i]);
                            end
                            //$display("}");
                            //$display("At leaf node:");
                            for (int i = 0; i < current_node.rule_count; i++) begin
                                rule_s rule = current_node.rules[MAX_RULES_PER_NODE-1-i];
                                print_rule(rule);
                            end
                            if (found_rule && current_node.rules[rule_index].weight < matched_rule.weight) begin
                                //$display("found new rule:");
                                print_rule(current_node.rules[rule_index]);
                                matched_rule <= current_node.rules[rule_index];
                            end
                            wants_new_node <= 1;
                        end
                        else begin
                            //$display("Error: This should be unreachable");
                        end
                    end
                end
            end
        end
    end
endmodule