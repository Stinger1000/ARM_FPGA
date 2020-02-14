module data_mem(
    input logic clk,
    input logic in_data_mem,
    input logic write_data,
    input logic [3:0]adr_data,
    input logic[3:0]adr_data_write,
    input logic [15:0]data_write,
    output logic [15:0]data,
    output logic out_data_mem
);

logic [15:0]DATA_MEM[0:15] = '{
    16'd1,
    16'd2,
    16'd3,
    16'd4,
    16'd0,
    16'd0,
    16'd0,
    16'd0,
    16'd0,
    16'd0,
    16'd0,
    16'd0,
    16'd0,
    16'd0,
    16'd0,
    16'd0
};

always_ff @(posedge clk)
begin
    if (in_data_mem==1'b1)
    begin
        data<=DATA_MEM[adr_data];
        out_data_mem <= 1'b1;
    end
    if (out_data_mem == 1'b1)
    begin
        out_data_mem <= 1'b0;
    end
    if(write_data==1'b1)
    begin
        DATA_MEM[adr_data_write]<=data_write;
        out_data_mem <= 1'b1;
    end
end

endmodule