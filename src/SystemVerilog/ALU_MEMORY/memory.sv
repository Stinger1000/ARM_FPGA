module memory(

);

logic clk = 1'b0;;
logic give_com;
logic [3:0]adr;
logic give_data;
logic write;
logic dv;
logic [15:0]com;
logic [3:0]data;

logic [15:0]CMD_MEM[0:1] = '{16'b000_0100_0010_0011_1, 16'b000_0110_0010_0011_0};//adress
logic [3:0]DATA_MEM[0:15] = '{
    16'd0,
    16'd0,
    16'd4,
    -16'd2,
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

logic[3:0]adr_result;

logic flag_write_adr = 1'b0;

alu mem(
    .clk(clk),
    .give_com(give_com),
    .adr(adr),
    .give_data(give_data),
    .write(write),
    .dv(dv),
    .com(com),
    .data(data)
);

initial begin
    forever begin
        #5 clk =!clk;
    end
end

always_ff @(posedge clk)
begin
    if(give_com==1'b1)
    begin
        dv<=1'b1;
        com<=CMD_MEM[adr];
    end
    if(dv==1'b1)
    begin
        dv<=1'b0;
    end

    if (give_data==1'b1)
    begin
        dv <= 1'b1;
        data <= DATA_MEM[adr];
    end

    if(write==1'b1)
    begin
        if(flag_write_adr==1'b0)
        begin
            flag_write_adr <= 1'b1;
            adr_result<=adr;
        end
    end
    if (flag_write_adr==1'b1)
    begin
            DATA_MEM[adr_result]<=adr;
            flag_write_adr <= 1'b0;
    end

end

endmodule



module alu(
    input clk,
    input dvi,
    input op_code,
    input [15:0] a,
    input [15:0] b,
    output dvo,
    input [15:0] p
);

endmodule

module cmd_mem (
    input clk,
);
endmodule

module data_mem(

);

endmodule

module cpu (
    input clk
);

    logic dv;

    alu arith (
        .clk (clk),
        .dvi (dv)
    );

endmodule