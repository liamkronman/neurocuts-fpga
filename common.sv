
package network_pkg;
    parameter IP_SIZE = 33;
    parameter PORT_SIZE = 17;
    parameter PROTOCOL_SIZE = 9;

    typedef struct packed {
        logic[IP_SIZE-1:0] ip;
        logic[PORT_SIZE-1:0] port;
    } socket_s;

    typedef struct packed {
        socket_s src;
        socket_s dst;
        logic[PROTOCOL_SIZE-1:0] protocol;
    } packet_s;

    typedef struct packed {
        packet_s start;
        packet_s last;
        logic[31:0] weight;
    } rule_s;

    task print_packet(input packet_s pkt);
        $display("Packet:");
        $display("  Src IP: %d, Src Port: %d", pkt.src.ip, pkt.src.port);
        $display("  Dst IP: %d, Dst Port: %d", pkt.dst.ip, pkt.dst.port);
        $display("  Protocol: %d", pkt.protocol);
    endtask
    
    task print_rule(input rule_s rule);
        // Print start packet
        $display("Start Packet - src IP: %0d, src Port: %0d, dst IP: %0d, dst Port: %0d, Protocol: %0d",
                 rule.start.src.ip, rule.start.src.port, rule.start.dst.ip, rule.start.dst.port, rule.start.protocol);
        
        // Print last packet
        $display("Last Packet - src IP: %0d, src Port: %0d, dst IP: %0d, dst Port: %0d, Protocol: %0d",
                 rule.last.src.ip, rule.last.src.port, rule.last.dst.ip, rule.last.dst.port, rule.last.protocol);

        // Print weight
        $display("Weight: %0d", rule.weight);
    endtask
endpackage

package node_pkg;
    import network_pkg::*;
    parameter PARTITION = 0;
    parameter CUT = 1;
    parameter LEAF = 2;

    typedef struct packed {
        logic[1:0] node_type;
        rule_s range;
        logic[31:0] rule_count;
        rule_s[MAX_RULES_PER_NODE-1:0] rules;
        logic[31:0] child_count;
        logic[MAX_CHILDREN_PER_NODE-1:0][31:0] children;
    } node_s;

    task print_node(node_s node);
        $display("Node Type: %b", node.node_type);

        // Print range
        // Assuming you have a method to print rule_s type

        print_rule(node.range);
        // Print rule_count
        $display("Rule Count: %0d", node.rule_count);

        // Print rules
        $display("Rules:");
        for (int i = MAX_RULES_PER_NODE-1; i >= MAX_RULES_PER_NODE-node.rule_count; i--) begin
            // Assuming you have a method to print rule_s type
            print_rule(node.rules[i]);
        end

        // Print child_count
        $display("Child Count: %0d", node.child_count);

        // Print children
        $display("Children:");
        //for (int i = MAX_CHILDREN_PER_NODE-1; i >= MAX_CHILDREN_PER_NODE-node.child_count; i--) begin
        for (int i = 0; i < MAX_CHILDREN_PER_NODE; i++) begin
            $display("Child[%0d]: %0d", i, node.children[i]);
        end
    endtask

    task display_node_bits(logic[$bits(node_s)-1:0] arr);
        $write("node_bits={");
        for (int i = 0; i < $bits(node_s); i++) begin
            $write("%b", arr[$bits(node_s) - 1 - i]);
        end
        $display("}");
    endtask

endpackage