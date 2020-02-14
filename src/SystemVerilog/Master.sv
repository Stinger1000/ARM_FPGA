module top(
    input logic clk,
    output logic mosi,
    output logic ss,
    input logic dv0,
    input logic [7:0]d0
    );
    logic [7:0]reg_1;
    int i;
    logic flag_1 = 1'b0;
    logic flag_2 = 1'b0;
    always_ff @(posedge clk) 
    begin
        if (dv0==1'b1) 
        begin
            reg_1 <= d0;
            i = 1'b0;
            flag_2 <= 1'b1;
        end
        if ((d0 == reg_1) && (flag_2==1'b1)) flag_1 <= 1'b1;
        if (flag_1 == 1'b1) 
        begin
            ss <= 1'b0;
            mosi <= reg_1[i];
            i <= i + 1;
        end;
        if ((i == 8) && (dv0!=1'b1))
        begin 
            i <= 0;
            flag_1 <= 1'b0;
            ss <= 1'b1;
            flag_2 <= 1'b0;
        end
    end
endmodule