
import network_pkg::*;
import tree_constants::*;

module classifier();
    // 32 bits large enough to hold index of any node
    localparam NODE_SIZE = MAX_CHILDREN_PER_NODE * 32;
    localparam RULE_SIZE = $bits(rule_s);
    logic [NODE_SIZE-1:0] nodes [0:TOTAL_NODES-1];
    logic [RULE_SIZE-1:0] rules [0:TOTAL_RULES-1];

    initial begin
        $readmemb("./rules.txt", rules);
        $readmemb("./nodes.txt", nodes);
    end

endmodule