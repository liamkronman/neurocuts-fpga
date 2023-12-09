
import network_pkg::*;

module compare_bit_range
#(parameter LENGTH)(
    input logic[LENGTH-1:0] value,
    input logic[LENGTH-1:0] first,
    input logic[LENGTH-1:0] last,
    output logic is_within
);
    assign is_within = (value >= first) && (value < last);
endmodule

module rule_match(
    input rule_s rule,
    input packet_s packet,
    output logic matched
);
    logic prot_match, ip_src_match, port_src_match, ip_dst_match, port_dst_match;
    packet_s start, last;

    always_comb begin
        start = rule.start;
        last = rule.last;
        assign matched = prot_match && ip_src_match && port_src_match && ip_dst_match && port_dst_match;
    end
    compare_bit_range#(PROTOCOL_SIZE) protocol(
        .value(packet.protocol), .first(start.protocol), .last(last.protocol),
        .is_within(prot_match)
    );
    compare_bit_range#(IP_SIZE) ip_src(
        .value(packet.src.ip), .first(start.src.ip), .last(last.src.ip),
        .is_within(ip_src_match)
    );
    compare_bit_range#(PORT_SIZE) port_src(
        .value(packet.src.port), .first(first.src.port), .last(last.src.port),
        .is_within(port_src_match)
    );
    compare_bit_range#(IP_SIZE) ip_dst(
        .value(packet.dst.ip), .first(start.dst.ip), .last(last.dst.ip),
        .is_within(ip_dst_match)
    );
    compare_bit_range#(PORT_SIZE) port_dst(
        .value(packet.dst.port), .first(first.dst.port), .last(last.dst.port),
        .is_within(port_dst_match)
    );
endmodule : rule_match