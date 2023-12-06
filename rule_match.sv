
import network_pkg::*
import utils_pkg::*

module rule_match(
    input rule_s rule,
    input packet_s packet,
    output logic matched
);
    always_comb begin
        packet_s start = rule.start, last = rule.last
        logic match_src, match_dst, match_protocol

        match_protocol = compare_bits#(PROTOCOL_SIZE)(
            packet.protocol, start.protocol, last.protocol);
        match_src = compare_bits#(IP_SIZE)(packet.src.ip, start.src.ip, last.src.ip)
                    && compare_bits#(PORT_SIZE)(packet.src.port, first.src.port, last.src.port);
        match_dst = compare_bits#(IP_SIZE)(packet.dst.ip, start.dst.ip, last.dst.ip)
                    && compare_bits#(PORT_SIZE)(packet.dst.port, first.dst.port, last.dst.port);

        matched = match_protocol && match_src && match_dst
    end
endmodule : rule