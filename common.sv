
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
endpackage

package node_pkg;
    import network_pkg::rule_s;
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

endpackage