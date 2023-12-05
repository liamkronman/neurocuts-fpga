
struct {
    logic[] ip;
    logic[] port;
} ip_s;

struct {
    ip_s src;
    ip_s dst;
    logic[] protocol;
} packet_s;

module rule_range(
    input ip_s start_src_ip,
    input ip_s end_src_ip,
    input ip_s start_src_ip,
    input ip_s end_src_ip,
    input logic[] start_proto,
    input logic[] end_proto
    output packet_s sample_packet
);

endmodule

function logic rule_ranges_intersect(input rule_range a, b)
    begin
        // TODO MAKE SURE ALL RANGE INTERSECTS
        rule_range_intersects = ;
    end
endfunction


function logic rule_intersects_range(input rule_range a, input packet_s packet)
    begin
        // TODO MAKE SURE ALL RANGE INTERSECTS
        rule_intersects_range = ;
    end
endfunction

function logic matches_range(input rule_range a, input packet_s packet)
    begin
        matches_range = rule_range_intersects(a, /*expand packet here*/)
    end
endfunction

module rule #(parameter PRIORITY; parameter RANGE_LENGTH)(

    input logic[4] ranges,
    output logic is_intersect,
    output logic sample_packet
);

endmodule : rule