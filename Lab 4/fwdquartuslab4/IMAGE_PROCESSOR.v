`define SCREEN_WIDTH 176
`define SCREEN_HEIGHT 144
`define NUM_BARS 3
`define BAR_HEIGHT 48

module IMAGE_PROCESSOR (
	PIXEL_IN,
	CLK,
	VGA_PIXEL_X,
	VGA_PIXEL_Y,
	VGA_VSYNC_NEG,
	RESULT
);


//=======================================================
//  PORT declarations
//=======================================================
input	[7:0]	PIXEL_IN;
input 		CLK;

input [9:0] VGA_PIXEL_X;
input [9:0] VGA_PIXEL_Y;
input			VGA_VSYNC_NEG;

output [2:0] RESULT;
reg [2:0] reg_result;
assign RESULT = reg_result;

reg [15:0] countBLUE; 
reg [15:0] countRED; 
reg [15:0] countNULL; 
reg [2:0] R_THRESHOLD = 3'b100;
reg [2:0] B_THRESHOLD = 3'b100;
reg [15:0] R_CNT_THRESHOLD = 16'd30000; 
reg [15:0] B_CNT_THRESHOLD = 16'd30000; 
reg lastsync = 1'b0; 

always @(posedge CLK) begin 
	if(VGA_VSYNC_NEG) begin 
	
		if(PIXEL_IN[1:0] > 2'd0) begin 
			countBLUE = countBLUE + 16'd1; // should it be 16'b1
			//reg_result[0] = 1'b1; 
			end 
		else begin 
			countNULL = countNULL + 16'd1; // should it be 16'b1
			end
		
		if(PIXEL_IN[7:5] > 3'd0) begin 
			countRED = countRED + 16'd1; 
			//reg_result[1] = 1'b1; 
			end 
		else begin 
			countNULL = countNULL + 16'd1; // should it be 16'b1
			//reg_result[2] = 1'b1; 
			end 
	//end 
	
	//if(VGA_VSYNC_NEG == 1'b1 && lastsync == 1'b0) begin //posedge VSYNC 
		$display("countBLUE = %d\n",countBLUE);
		$display("countRED = %d\n",countRED);
		$display("countNULL = %d\n",countNULL);
		if(countBLUE > B_CNT_THRESHOLD) begin 
			reg_result[0] = 1'b1; 
			end 
		if(countRED > R_CNT_THRESHOLD) begin 
			reg_result[1] = 1'b1; 
			end 
		else begin 
			reg_result[2] = 1'b1;
			end 
	end 
	
	if(VGA_VSYNC_NEG == 1'b0 && lastsync == 1'b1) begin //negedge VSYNC 
		countBLUE = 16'b0; 
		countRED = 16'b0; 
		countNULL = 16'b0; 
		reg_result = 3'b0;
		end 
	lastsync = VGA_VSYNC_NEG; 
end 


endmodule