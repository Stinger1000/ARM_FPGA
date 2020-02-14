module cpu(
    input logic clk
);
logic operation;
logic in_alu;
logic [15:0] operand1;
logic[15:0] operand2;
logic in_data_mem;
logic[15:0] result;
logic out_alu;
logic [3:0]adr_cmd;
logic [3:0]adr_data;
logic [3:0]adr_data_write;
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
    .adr_data_write(adr_data_write),
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
logic[3:0] adr_result[0:4];
logic operation2[0:4];

logic [15:0] operand1_last;
logic[15:0] operand2_last;

logic[3:0] adr1_last;
logic[3:0] adr2_last;


int i = 0;
int k1 = 0;
int k2 = 0;
int k3 = 0;
int g = 0;

logic flag_alu = 1'b1;
logic flag_readcom = 1'b0;
logic flag_read_data1 = 1'b0;

logic flag_read_launch = 1'b0;
logic flag_decode_launch = 1'b0;
logic flag_operator_launch = 1'b0;
logic flag_exec_launch = 1'b0;
logic flag_write_launch = 1'b0;

logic send_data = 1'b0;
logic send_data2 =1'b0;
logic flag_start =1'b0;


always_ff @(posedge clk)
begin

    if(g==0)
    begin
        flag_read_launch <= 1'b1;
    end

    if(g==1)
    begin
        flag_decode_launch <= 1'b1;
        flag_read_launch <= 1'b1;
    end

    if(g==3)
    begin
        flag_decode_launch <= 1'b1;
        flag_read_launch <= 1'b1;
        flag_operator_launch <=1'b1;
    end

    if(g==6)
    begin
        flag_decode_launch <= 1'b1;
        flag_read_launch <= 1'b1;
        flag_operator_launch <=1'b1;
        flag_exec_launch <= 1'b1;
    end

    if(g==9)
    begin
        flag_decode_launch <= 1'b1;
        flag_read_launch <= 1'b1;
        flag_operator_launch <=1'b1;
        flag_exec_launch <= 1'b1;
        flag_write_launch <= 1'b1;
    end

    if((flag_operator_launch==1'b0)&&(flag_start==1'b1))
    begin
        flag_decode_launch <= 1'b1;
        flag_read_launch <= 1'b1;
        flag_operator_launch <=1'b1;
        flag_exec_launch <= 1'b1;
        flag_write_launch <= 1'b1;
    end

////////////////////////////////////////////////////////
    if(flag_read_launch==1'b1)
    begin
        if(flag_readcom==1'b0)
        begin
            in_cmd_mem<=1'b1;
            adr_cmd<= i;
            if (out_cmd_mem == 1'b1)
            begin
                in_cmd_mem<=1'b0;
                flag_readcom <= 1'b0;
                flag_read_launch<=1'b0;
                i <= i+1;
                g <= g+1;
            end
        end
    end
////
    if(flag_decode_launch==1'b1)
    begin
        adr2_last <= cmd[8:5];
        adr1_last <= cmd[4:1];
        adr1 <= adr1_last;
        adr2 <= adr2_last;

        adr_result[k1] <= cmd[12:9];
        operation2[k1] <= cmd[1:0];

        flag_decode_launch<=1'b0;
        g <= g+1;
        k1<=k1+1;
        if(k1==4)
        begin
            k1<=0;
        end
    end
///
    if(flag_operator_launch==1'b1)
    begin
        if(flag_read_data1==1'b0)
        begin  
            send_data<=1'b1;
            if(send_data==1'b1)
            begin
                in_data_mem <= 1'b1;
                adr_data<=adr1;
                send_data <= 1'b0;
            end
            if(out_data_mem==1'b1)
            begin
                operand1<=data;
                in_data_mem<=1'b0;
                flag_read_data1 <=1'b1;
            end
        end
            if(flag_read_data1==1'b1)
            begin
                in_data_mem <= 1'b1;
                adr_data<=adr1;
                if(out_data_mem == 1'b1)
                begin
                    operand2 <= data;
                    flag_read_data1<=1'b0;
                    flag_operator_launch <= 1'b0;
                    g<=g+1;
                end
            end
            //send_data<=1'b1;
            // if (send_data==1'b1)
            // begin
            //     adr_data<=adr1;
            //     in_data_mem<=1'b1;
            //     if(out_data_mem==1'b1)
            //     begin
            //         operand1<=data;
            //         adr_data<=adr2;
            //         send_data<=1'b0;
            //         flag_read_data1<=1'b1;
            //     end
            // end
    
        // if ((flag_read_data1==1'b1)&&(out_data_mem==1'b1))
        // begin
        //     send_data2 <=1'b1;
        //     flag_read_data1<=1'b0;
        //     in_data_mem<=1'b0;
        // end
        // if(send_data2==1'b1)
        // begin
        //     operand2<=data;
        //     send_data2 <= 1'b0;
        //     flag_operator_launch<=1'b0;
        //     g<=g+1;
        //     flag_start <= 1'b1;
        // end
    end
////
    if(flag_exec_launch==1'b1)
    begin
        operation <= operation2[k2];
        in_alu<=1'b1;
        if(out_alu==1'b1)
        begin
            in_alu <= 1'b0;
            flag_exec_launch<=1'b0;
            g <= g+1;
            k2<=k2+1;
            if(k2==4)
            begin
                k2<=0;
            end
        end
    end
////
    if(flag_write_launch==1'b1)
    begin
        adr_data_write<=adr_result[k3];
        write_data<=1'b1;
        data_write<=result;
        if(out_data_mem==1'b1)
        begin
            flag_write_launch<=1'b0;
            write_data<=1'b0;
            k3<=k3+1;
            g<=g+1;
            if(k3==4)
            begin
                k3<=0;
            end
        end
    end
end

endmodule
