module cpu(
    input logic clk
);

logic in_alu;
logic [15:0] operand1;
logic[15:0] operand2;
logic in_data_mem;
logic[15:0] result;
logic out_alu;
logic [3:0]adr_data;
logic [3:0]adr_cmd;
logic [15:0]data;
logic out_data_mem;
logic in_cmd_mem;
logic [15:0]cmd;
logic out_cmd_mem;
logic [15:0]data_write;
logic write_data;


alu alucpu(
    .clk(clk),
    .operation(operation),
    .in_alu(in_alu),
    .operand1(operand1),
    .operand2(operand2),
    .result(result),
    .out_alu(out_alu)
);

data_mem data_mem_cpu(
    .clk(clk),
    .in_data_mem(in_data_mem),
    .adr_data(adr_data),
    .data(data),
    .data_write(data_write),
    .write_data(write_data),
    .out_data_mem(out_data_mem)
);

cmd_mem cmd_mem(
    .clk(clk),
    .in_cmd_mem(in_cmd_mem),
    .adr_cmd(adr_cmd),
    .cmd(cmd),
    .out_cmd_mem(out_cmd_mem)
);

typedef enum {
    READ,
    DECODE,
    OPERATOR,
    EXEC,
    WRITE
} sm_t;
sm_t State = READ;

logic[3:0] adr1;
logic[3:0] adr2;
logic operation;
logic[3:0] adr_result[0:4];

int i = 0;//schetch

logic flag_alu = 1'b1;
logic flag_readcom = 1'b0;
logic flag_readdata1 = 1'b0;
logic flag_readdata2 = 1'b0;

always_ff @(posedge clk)
begin
    case (State)
    READ:
    begin
        if(flag_readcom==1'b0)
        begin
            in_cmd_mem<=1'b1;
            adr_cmd<= i;
            if (out_cmd_mem == 1'b1)
            begin
                in_cmd_mem<=1'b0;
                flag_readcom <= 1'b0;
                State <= DECODE;
                i <= i+1;
            end
        end
    end
    DECODE:
    begin
        adr1 <= cmd[4:1];
        adr2 <= cmd[8:5];
        adr_result <= cmd[12:9];
        operation <= cmd[1:0];
        State<=OPERATOR;
    end
    OPERATOR:
    begin
        if(flag_readdata1==1'b0)
        begin
            adr<=adr1;
            in_data_mem<=1'b1;
            if(out_data_mem==1'b1)
            begin
                operand1<=data;
                adr_data<=adr2;
                flag_readdata1<=1'b1;
            end
        end
        if ((flag_readdata1==1'b1)&&(out_data_mem==1'b1))
        begin
            operand2<=data;
            flag_readdata1<=1'b0;
            in_data_mem<=1'b0;
            State<=EXEC;
        end
    end
    EXEC:
    begin
        in_alu<=1'b1;
        if(out_alu==1'b1)
        begin
            in_alu <= 1'b0;
            State<=WRITE;
        end
    end
    WRITE:
    begin
        adr_data<=adr_result;
        write_data<=1'b1;
        data_write<=result;
        if(out_data_mem==1'b1)
        begin
            State<=READ;
            write_data<=1'b0;
        end
    end
    endcase
end

endmodule
