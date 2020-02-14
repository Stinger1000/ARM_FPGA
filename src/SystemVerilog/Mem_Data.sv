module Mem_Data(
    input logic clk,
    input logic dv,
    input logic[3:0] data_t,
    input logic[15:0] com,
    output logic giveD,
    output logic giveC,
    output logic write_data,
    output logic [3:0]ADR_1
    );


    int i = 0;
    logic reset = 1'b1;
    logic [15:0]CMD; 

    parameter READ = 1;
    parameter DECODE = 2;
    parameter OPERATOR = 3;
    parameter EXEC = 4;
    parameter WRITE = 5;

    reg [3:0]State = READ;

    logic[3:0] adr1;
    logic[3:0] adr2;
    logic[3:0] result;
    logic oper; 
    logic[3:0] operand1;
    logic[3:0] operand2;
    logic flag;


    always_ff @(posedge clk) 
    begin
        if (reset == 1'b1)
        begin
            case (State)
            READ:
            begin
                giveC <= 1'b1;
                ADR_1 <= i;
                if (dv==1'b1)
                begin
                    CMD <= com;
                    giveC <=1'b0;
                    State <= DECODE;
                end
            end
            DECODE:
            begin
                adr1 <= CMD[4:1];
                adr2 <= CMD[8:5];
                result <= CMD[12:9];
                oper <= CMD[1:0];
                State <= OPERATOR;
            end
            OPERATOR:
            begin
                giveD <= 1'b1;
                ADR_1 <= adr1;
                if (giveD == 1'b1) giveD <= 1'b0;
                if (dv == 1'b1)
                begin
                giveD <= 1'b0;
                operand1 <= data_t;
                flag <= 1'b1;
                end
                if((flag == 1'b1)&&(dv==1'b0))
                begin
                    giveD <= 1'b1;
                    ADR_1 <= adr2;
                end
                if(giveD==1'b1) giveD <= 1'b0;
                if (dv == 1'b1)
                begin
                operand2 <= data_t;
                if (operand2 == data_t) State <= EXEC;
                end
            end
            EXEC:
            begin
                State <= WRITE;
                i <= i + 1;
                case(oper)
                1'b0:
                begin
                    operand1 <= operand1 + operand2;
                end
                1'b1:
                begin
                    operand1 <= operand1 - operand2;
                end
                default:
                begin
                    State<=READ;
                end
                endcase
            end
            WRITE:
            begin
                write_data <= 1'b1;
                ADR_1 <= operand1;
                if (write_data == 1'b1)
                begin
                write_data <= 1'b0;
                State <= READ;
                end
            end
            default:
            begin
                State <= READ;
            end
            endcase
        end
        if (i == 2)
        begin 
            reset <= 1'b0;
        end
    end
endmodule
