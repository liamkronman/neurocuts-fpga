
import network_pkg::*;
import tree_constants::*;
import node_pkg::*;

module classifier(
    input logic clk,
    input logic reset
);
    // 32 bits large enough to hold index of any node
    logic [$bits(node_s)-1:0] node_data [0:TOTAL_NODES-1];
    logic[31:0] current_node;
    initial begin
        $readmemb("./nodes.txt", node_data);
        $display("initialized node_data");
    end

    node_s node;
    reinterpret_bits_as_node my_interpret(
        .node_data(node_data[current_node]),
        .node(node)
    );
    always_ff @(posedge clk) begin
        if (reset) begin
            current_node <= 0;
        end
        else begin
            if (current_node == TOTAL_NODES - 1) begin
                current_node <= 0;
            end
            else begin
                current_node <= current_node + 1;
            end
            $display(node);
        end
    end
endmodule