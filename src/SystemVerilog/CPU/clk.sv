module extern_clk(
);
logic clk = 1'b0;
logic in_alu;
logic operation;
logic [15:0] operand1;
logic[15:0] operand2;
logic in_data_mem;
logic[15:0] result;
logic out_alu;
logic [3:0]adr;
logic [15:0]data;
logic out_data_mem;
logic write_data;
logic[15:0] data_write;
logic in_cmd_mem;
logic [15:0]cmd;
logic out_cmd_mem;

cpu cpu(
    .clk(clk)
);

    initial begin
        forever begin
            #5 clk =!clk;
        end
    end
endmodule