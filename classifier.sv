
import network_pkg::*;
import tree_constants::*;
import node_pkg::*;

module classifier(
    input logic clk,
    input logic reset,
    output logic finish
);
    // 32 bits large enough to hold index of any node
    logic [$bits(node_s)-1:0] node_data [0:TOTAL_NODES-1];
    logic[31:0] current_node;
    node_s node;

    initial begin
        $display("Size of node_s in rule: %d", $bits(rule_s));
        $display("Size of node_s in bits: %d", $bits(node_s));
        $readmemb("./nodes.txt", node_data);
        $display("initialized node_data");
    end
    
    // 370 iterations cause we need to see that were invalid
    always_ff @(posedge clk) begin
        if (reset) begin
            current_node <= 0;
            finish <= 0;
        end
        else begin
            if (current_node == TOTAL_NODES - 1) begin
                finish <= 1;
                current_node <= 0;
            end
            else begin
                current_node <= current_node + 1;
            end
            $display("node:%d, rule_count:%d", current_node, node.rule_count);
            node <= node_data[current_node];
        end
    end

    always_comb begin
        if (finish) begin
            $finish();
        end
    end
endmodule