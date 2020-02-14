module top(
    input logic clk,
    input logic mosi,
    input logic ss,
    output logic dv0=1'b0,
    output logic [7:0]d0 = 8'b00000000 
    );
    int i = 0;
    always_ff @(posedge clk) 
    begin
        if (ss==1'b0) 
        begin
            i<=i+1;
            d0 <= {d0[6:0],mosi};
            if (i == 7)
            begin
                dv0 <= 1'b1;
            end
            else dv0 <= 1'b0;
            if (i==7)
            i<=0;
        end
    end
endmodule