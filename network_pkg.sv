
package network_pkg;
    parameter IP_SIZE = 32;
    parameter PORT_SIZE = 16;
    parameter PROTOCOL_SIZE = 8;

    typedef struct {
        logic[IP_SIZE-1:0] ip;
        logic[PORT_SIZE-1:0] port;
    } socket_s;

    typedef struct {
        socket_s src;
        socket_s dst;
        logic[PROTOCOL_SIZE-1:0] protocol;
    } packet_s;

    typedef struct {
        packet_s start;
        packet_s last;
    } rule_s;
endpackage