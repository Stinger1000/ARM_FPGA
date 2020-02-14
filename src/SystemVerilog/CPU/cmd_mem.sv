module cmd_mem(
    input logic clk,
    input logic in_cmd_mem,
    input logic[3:0] adr_cmd,
    output logic [15:0]cmd,
    output logic out_cmd_mem
);

logic [15:0]CMD_MEM[0:3] = '{16'b000_0110_0000_0001_1, 16'b000_0111_0010_0011_0,16'b000_1000_0000_0001_1,16'b000_1001_0000_0001_1};//adress

always_ff @(posedge clk)
begin
    if(in_cmd_mem==1'b1)
    begin
        out_cmd_mem <= 1'b1;
        cmd <= CMD_MEM[adr_cmd];
    end
    if (in_cmd_mem == 1'b0)
    begin
        out_cmd_mem <= 1'b0;
    end
end

endmodule