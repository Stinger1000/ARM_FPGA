`timescale 1ns / 1ps

module top_tb(

    );
    logic RD_EN;
    logic clk = 1'b0;
    logic [3:0]ADR; 
    Mem_Data dut (
        .clk(clk) 
        );
        initial begin
        forever begin
        #5 clk = !clk; 
        end
        end
        initial begin
        end
endmodule
