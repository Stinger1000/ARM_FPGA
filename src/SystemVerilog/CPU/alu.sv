module alu(
    input logic clk,
    input logic in_alu,
    input logic[15:0] operand1,
    input logic[15:0] operand2,
    input logic operation,
    output logic[15:0] result,
    output logic out_alu
);

always_ff @(posedge clk)
begin

if(in_alu==1'b1)
begin
    case(operation)
        1'b0:
        begin
            result <= operand2+operand1;
            out_alu <= 1'b1;
        end
        1'b1:
        begin
            result <= operand2-operand1;
            out_alu <= 1'b1;
        end
    endcase
    if (out_alu==1'b1)
    begin
        out_alu<=1'b0;
    end
end
end
endmodule