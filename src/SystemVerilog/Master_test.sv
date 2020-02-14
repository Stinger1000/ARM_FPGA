`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 06.02.2020 14:58:10
// Design Name: 
// Module Name: top_tb
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module top_tb(

    );
    logic clk = 1'b0;
    logic mosi; 
    logic ss;
    logic dv0;
    logic [7:0]d0;
    logic k = 1'b1; 
    top dut (
        .clk (clk), 
        .mosi(mosi), 
        .ss(ss),
        .dv0(dv0),
        .d0(d0)
        );
        initial begin
        forever begin
        #5 clk = !clk; 
        end
        end
        initial begin
            @(posedge clk);
            dv0 = 1'b1;
            if (dv0 == 1'b1) d0 = 7'b00_11_10_11;
            @(posedge clk);
            dv0 = 1'b0;
            for (int i = 0;i<15;i++)
            begin
                @(posedge clk);
            end
            dv0 = 1'b1;
            if (dv0 == 1'b1) d0 = 7'b01_01_01_01;
            @(posedge clk);
            dv0 = 1'b0;
        end
endmodule
