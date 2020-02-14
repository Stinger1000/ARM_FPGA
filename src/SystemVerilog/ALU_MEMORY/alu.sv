module cpu (
    input logic clk,
    input logic dv,
    input logic[15:0] com,
    input logic[3:0] data,
    output logic give_com,
    output logic[3:0] adr,
    output logic give_data,
    output logic write
);

typedef enum {
    READ,
    DECODE,
    OPERATOR,
    EXEC,
    WRITE
} sm_t;

// reg[3:0] State = READ;

sm_t State = READ;

logic [15:0]command;

logic[3:0] operator1;
logic[3:0] operator2;

logic[3:0] adr1;
logic[3:0] adr2;
logic[3:0] adr_result;
logic operation;

int i = 0;

logic flag_operand1_get = 1'b0;
logic flag_pull_give_com = 1'b0;
logic flag_pull_give_data = 1'b0;
logic flag_pull_give_data2 = 1'b0;
logic flag_write_adr = 1'b0;


always_ff @(posedge clk)
begin
    case (State)
        READ:
        begin
            if (flag_pull_give_com==1'b0)
            begin
                give_com <= 1'b1;
                adr <= i;
            end
            if (give_com==1'b1)
            begin
                give_com<=1'b0;
                flag_pull_give_com <= 1'b1;
            end
            if (dv==1'b1)
            begin
                command <= com;
                State <= DECODE;
                flag_pull_give_com <= 1'b0;
            end
        end
        DECODE:
        begin
            adr1 <= command[4:1];
            adr2 <= command[8:5];
            adr_result <= command[12:9];
            operation <= command[1:0];
            State <= OPERATOR;
        end
        OPERATOR:
        begin
            if (flag_pull_give_data == 1'b0)
            begin
                give_data <= 1'b1;
                adr <= adr1;
                flag_pull_give_data <= 1'b1;
            end
            if (give_data==1'b1)
            begin
                give_data<=1'b0;
            end
            if ((dv==1'b1)&&(flag_operand1_get == 1'b0))
            begin
                operator1 <= data;
                flag_operand1_get <= 1'b1;
            end
            if (flag_operand1_get == 1'b1)
            begin
                if (flag_pull_give_data2 == 1'b0)
                begin
                    give_data <= 1'b1;
                    adr <= adr2;
                    flag_pull_give_data2 <= 1'b1;
                end
                if (give_data == 1'b1)
                begin
                    give_data <= 1'b0;
                end
                if ((dv == 1'b1)&&(flag_pull_give_data2==1'b1))
                begin
                    operator2 <= data;
                    State <= EXEC;
                    i<=i+1;
                    flag_operand1_get <= 1'b0;
                    flag_pull_give_data2 <= 1'b0;
                    flag_pull_give_data <= 1'b0;
                end
            end
        end
        EXEC:
        begin
            case (operation)
                1'b0:
                begin
                    operator1<=operator2+operator1;
                    State <= WRITE;
                end
                1'b1:
                begin
                    operator1<=operator2-operator1;
                    State <= WRITE;
                end
            default:
            begin
                State <= READ;
            end
            endcase
        end
        WRITE:
        begin
            if (flag_write_adr==1'b0)
            begin
                write <= 1'b1;
                adr <= adr_result;
                flag_write_adr <= 1'b1;
            end
            if (flag_write_adr == 1'b1)
            begin
                adr<=operator1;
                State <= READ;
                write<=1'b0;
                flag_write_adr<=1'b0;
            end
        end
        default:
        begin
            State <= READ;
        end
    endcase
end
endmodule