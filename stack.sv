
module stack#(
    parameter STACK_SIZE = 100,
    parameter DATA_WIDTH = 8
) (
    input logic clk,
    input logic reset,
    input logic push,
    input logic pop,
    input logic[DATA_WIDTH-1:0] data_in,
    output logic[DATA_WIDTH-1:0] data_out,
    output logic just_popped,
    output logic full,
    output logic empty
);
    logic [DATA_WIDTH-1:0] stack_memory[0:STACK_SIZE-1];
    logic [31:0] top = 0;

    always_comb begin
        full = top >= STACK_SIZE;
        empty = (top == 0);
    end

    always_ff@ (posedge clk) begin
        //$display("stack top: %d)", top);
        $write("stack: {");
        for (int i = 0; i < top; i++) begin
            $write("%d:%d,", i, stack_memory[i]);
        end
        $display("}");
        if (reset) begin
            top <= 0;
            just_popped <= 0;
        end
        else if (push && !full) begin
            top <= top + 1;
            stack_memory[top] <= data_in;
            just_popped <= 0;
        end
        else if (pop && !empty) begin
            just_popped <= 1;
            data_out <= stack_memory[top - 1];
            top <= top - 1;
        end
        else begin
            just_popped <= 0;
        end
    end

endmodule