
package network_pkg;
    parameter IP_SIZE = 32;
    parameter PORT_SIZE = 16;
    parameter PROTOCOL_SIZE = 8;

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
    } rule_s;
endpackage

import network_pkg::*;

module reinterpret_bits_as_socket(
    input logic[($bits(socket_s)-1):0] bits,
    output socket_s socket
);
    assign socket.port = bits[PORT_SIZE-1 + IP_SIZE:IP_SIZE];
    assign socket.ip = bits[IP_SIZE-1:0];
endmodule

module reinterpret_bits_as_packet(
    input logic[$bits(rule_s)-1:0] bits,
    output packet_s packet
);
    localparam DST_OFFSET = $bits(socket_s);
    localparam PORT_OFFSET = 2*$bits(socket_s);
    reinterpret_bits_as_socket my_src(.bits(bits[$bits(socket_s)-1 : 0]), .socket(packet.src));
    reinterpret_bits_as_socket my_dst(.bits(bits[$bits(socket_s)-1 + DST_OFFSET : DST_OFFSET]), .socket(packet.dst));
    always_comb begin
        packet.protocol = bits[PROTOCOL_SIZE-1 + PORT_OFFSET:PORT_OFFSET];
    end
endmodule

module reinterpret_bits_as_rule(
    input logic[$bits(rule_s)-1:0] bits,
    output rule_s rule_out
);
    localparam LAST_OFFSET = $bits(packet_s);
    reinterpret_bits_as_packet start(.bits(bits[$bits(packet_s)-1:0]), .packet(rule_out.start));
    reinterpret_bits_as_packet last(.bits(bits[$bits(packet_s)-1 + LAST_OFFSET:LAST_OFFSET]), .packet(rule_out.last));
endmodule