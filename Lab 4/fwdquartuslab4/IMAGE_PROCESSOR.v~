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
reg [15:0] FRAME_THRESHOLD = 16'd15000;
reg [15:0] R_CNT_THRESHOLD = 16'd6000; 
reg [15:0] B_CNT_THRESHOLD = 16'd3000; 	// 6000 for square (bigger area), 3000 for smaller triangle (smaller area)
reg lastsync = 1'b0; 
reg [7:0] lastY = 8'b0;

// Blue edge Points
reg [7:0] BLUE_EdgePoint_0 = 8'b0;
reg [7:0] BLUE_EdgePoint_1 = 8'b0;
reg [7:0] BLUE_EdgePoint_2 = 8'b0;
reg [7:0] BLUE_CNT_EdgePoint = 8'b0;
// Red edge Points
reg [7:0] RED_EdgePoint_0 = 8'b0;
reg [7:0] RED_EdgePoint_1 = 8'b0;
reg [7:0] RED_EdgePoint_2 = 8'b0;
reg [7:0] RED_CNT_EdgePoint = 8'b0;

always @(posedge CLK) begin 
	if(VGA_VSYNC_NEG) begin 
		// If the camera is read as RGB323 (For more blue value) No need for green
		// PIXEL_IN[2:0] = Blue
		// PIXEL_IN[7:5] = Red
	
		// The difference of comparing [blue] with [red + 2'b10] and [red + 2'b01] is that [red + 2'b10] might exceed 3 bit. Thus, blue (3 bits) will always be larger than 4'b1000 or larger, 
		//	Blue usually has dark blue instead of bright, therefore the blue value threshold has to be smaller
		if(PIXEL_IN[2:0] > 3'b001 && PIXEL_IN[2:0] > (PIXEL_IN[7:5] + 2'b10) && PIXEL_IN[2:0] > (PIXEL_IN[7:5] + 2'b01)  ) begin 
			countBLUE = countBLUE + 16'd1; 
			
			if ( lastY == VGA_PIXEL_Y && BLUE_EdgePoint_0 == 8'b0) begin
				BLUE_EdgePoint_0 = VGA_PIXEL_X;
				end
			// Continue as long as the second edge point is [bigger] than the first edge point
			else if ( lastY == VGA_PIXEL_Y && (BLUE_EdgePoint_1 == 8'b0 || VGA_PIXEL_X > BLUE_EdgePoint_0) ) begin
				BLUE_EdgePoint_1 = VGA_PIXEL_X;
				end
			// Continue as long as the third edge point is [smaller] than the second edge point
			else if ( lastY == VGA_PIXEL_Y && (BLUE_EdgePoint_2 == 8'b0 || BLUE_EdgePoint_2 < VGA_PIXEL_X) ) begin
				BLUE_EdgePoint_2 = VGA_PIXEL_X;
				end
				
			end 
		else begin 
			countNULL = countNULL + 16'd1; 
			end
		
		// Usually, we have brighter red, so the red value threshold is higher.
		if(PIXEL_IN[7:5] > 3'b100 && PIXEL_IN[7:5] > (PIXEL_IN[2:0] + 2'b10) && PIXEL_IN[7:5] > (PIXEL_IN[2:0] + 2'b01) ) begin 
			countRED = countRED + 16'd1; 
			end 
		else begin 
			countNULL = countNULL + 16'd1; 
			end 
			
		/*--------------------- Testing Edge Point -------------------------*/
		if (BLUE_EdgePoint_0 != 8'b0) begin 
			reg_result[0] = 1'b1; 
			end
		else if (BLUE_EdgePoint_1 != 8'b0) begin 
			reg_result[1] = 1'b1; 
			end
		else if (BLUE_EdgePoint_2 != 8'b0) begin 
			reg_result[2] = 1'b1; 
			end
		/*--------------------- Testing Edge Point -------------------------*/
		
		/*--------------------- Testing Treasure Shape -------------------------*/
		// reg_result[2] : Triangle
		// reg_result[1] : Square
		// reg_result[0] : Diamand
//		if (BLUE_EdgePoint_1 > BLUE_EdgePoint_0 && BLUE_EdgePoint_1 > BLUE_EdgePoint_2) begin
//			reg_result[0] = 1'b1; 
//			end
//		else if ( (BLUE_EdgePoint_0 + 5'd20) > ((BLUE_EdgePoint_1 + BLUE_EdgePoint_0) >> 1)) begin
//			reg_result[1] = 1'b1; 
//			end
//		else begin
//			reg_result[2] = 1'b1; 
//			end
		/*--------------------- Testing Treasure Shape -------------------------*/
		
		/*----------------------- Testing color ---------------------------*/
		if(FRAME_THRESHOLD > countBLUE && countBLUE > B_CNT_THRESHOLD) begin 
			reg_result[0] = 1'b1; 
			end 
		if(FRAME_THRESHOLD > countRED && countRED > R_CNT_THRESHOLD) begin 
			reg_result[1] = 1'b1; 
			end 
		//// Even if countBLUE is bigger than B_CNT_THRESHOLD, the reg_result[2] is still set to true. 
		if( (countBLUE <= B_CNT_THRESHOLD) && (countRED <= R_CNT_THRESHOLD) ) begin 
			reg_result[2] = 1'b1;
			end 
		/*----------------------- Testing color ---------------------------*/
	end 
	
	if(VGA_VSYNC_NEG == 1'b0 && lastsync == 1'b1) begin //negedge VSYNC 
		countBLUE = 16'b0; 
		countRED = 16'b0; 
		countNULL = 16'b0; 
		reg_result = 3'b0;
		end 
	lastsync = VGA_VSYNC_NEG; 
	lastY = VGA_PIXEL_Y;
end 


endmodule