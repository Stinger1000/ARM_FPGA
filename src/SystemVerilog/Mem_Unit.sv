module Com(

);
    logic clk = 1'b0;
    logic [3:0]adr;
    logic dv;
    logic [15:0]com;
    logic write_data;
    logic giveD;
    int g = 4;
    logic [3:0]data_t;
    logic [15:0]CMD_MEM[0:1] = '{16'b000_0000_0010_0011_0,16'b000_0001_0010_0011_1};//адреса
    logic [3:0]DATA_MEM[0:6] = '{4'b00_00,4'b00_00,4'b00_11,4'b00_01,4'b00_00,4'b00_00,4'b00_00};
    Mem_Data good (
            .clk(clk),
            .dv(dv),
            .ADR_1(adr),
            .giveC(giveC),
            .giveD(giveD),
            .com(com),
            .data_t(data_t),
            .write_data(write_data)
    );
    initial begin
    forever begin
    #5 clk = !clk; 
    end
    end
    always_ff @(posedge clk)
    begin
        if (giveC==1'b1)
        begin
            com <= CMD_MEM[adr];
            dv <= 1'b1;
        end
        if(dv==1'b1) dv <= 1'b0;
        if (giveD==1'b1)
        begin
            data_t <= DATA_MEM[adr];
            dv <= 1'b1;
        end
        if (write_data==1'b1)
        begin
            DATA_MEM[g] <= adr;
            g<=g+1;
        end

    end

endmodule