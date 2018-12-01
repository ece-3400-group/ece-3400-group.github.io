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
	RESULT,
	FirstLine,
	LastLine
);


//=======================================================
//  PORT declarations
//=======================================================
input	[7:0]	PIXEL_IN;
input 		CLK;

input [9:0] VGA_PIXEL_X;
input [9:0] VGA_PIXEL_Y;
input			VGA_VSYNC_NEG;

output [3:0] RESULT;
output [9:0] FirstLine;
output [9:0] LastLine;

reg [9:0] firstline;
reg [9:0] lastline;
assign FirstLine = firstline;
assign LastLine = lastline;

reg [3:0] reg_result;
assign RESULT = reg_result;

reg [15:0] countBLUE; 
reg [15:0] countRED; 
reg [15:0] countNULL; 
reg [8:0] Trian_CNT = 9'b0;
reg [8:0] Sqr_CNT = 9'b0;
reg [8:0] Diam_CNT = 9'b0;
reg [15:0] FRAME_THRESHOLD = 16'd15000;
reg [15:0] R_CNT_THRESHOLD = 16'd1500; 
reg [15:0] B_CNT_THRESHOLD = 16'd1500; 	// 6000 for square (bigger area), 3000 for smaller triangle (smaller area)
reg lastsync = 1'b0; 
reg [7:0] lastY = 8'b0;

// Blue edge Points
reg [7:0] BLUE_LINE_0 = 8'b0;
reg [7:0] BLUE_LINE_1 = 8'b0;
reg [7:0] BLUE_LINE_2 = 8'b0;
reg [7:0] BLUE_LINE_3 = 8'b0;
reg [7:0] BLUE_LINE_4 = 8'b0;
// Red edge Points
reg [7:0] RED_LINE_0 = 8'b0;
reg [7:0] RED_LINE_1 = 8'b0;
reg [7:0] RED_LINE_2 = 8'b0;
reg [7:0] RED_LINE_3 = 8'b0;
reg [7:0] RED_LINE_4 = 8'b0;

// averaging enough frames for concluding the shape
reg [12:0] frameCNT = 13'd0;
// First Line for detection, which has enough pixels
reg [9:0] First_redLine = 10'b0;
reg [9:0] Prev_First_redLine = 10'b0;
reg First_redLine_found = 1'b0;

reg [9:0] Last_redLine = 10'b0;
reg [9:0] Prev_Last_redLine = 10'b0;
reg Last_redLine_found = 1'b0;

// Last line for detection, which doesn't have enough pixel
reg [7:0] First_blueLine = 8'b0;
reg First_blueLine_found = 1'b0;

// Preious Y address for HREF
reg [9:0] prev_X = 10'b0;
reg [9:0] prev_Y = 10'b0;

always @(posedge CLK) begin 
	if(VGA_VSYNC_NEG) begin 
		// Count mass point of the shape for centerring the three line for shape detection, which means the height would change dynamically
		// Mx = sum(all x) / Pixel numbers
		// My = sum(all Y) / Pixel numbers
		frameCNT  = frameCNT + 1'b1;
	
	
		// If the camera is read as RGB323 (For more blue value) No need for green
		// PIXEL_IN[2:0] = Blue
		// PIXEL_IN[7:5] = Red
	
		// The difference of comparing [blue] with [red + 2'b10] and [red + 2'b01] is that [red + 2'b10] might exceed 3 bit. Thus, blue (3 bits) will always be larger than 4'b1000 or larger, 
		//	Blue usually has dark blue instead of bright, therefore the blue value threshold has to be smaller
		if (((PIXEL_IN[1:0] > 3'b000) && (PIXEL_IN[1:0] > (PIXEL_IN[7:5] >> 1)) && PIXEL_IN[4:2] < 2'd3) && prev_X != VGA_PIXEL_X) begin 	// && PIXEL_IN[2:0] > (PIXEL_IN[7:5] + 2'b10)
			countBLUE = countBLUE + 16'd1; 
			/*
			if ( VGA_PIXEL_Y == First_blueLine) begin
				BLUE_LINE_0 = BLUE_LINE_0 + 1'b1;
				if (BLUE_LINE_0 > 9'd10) begin
					First_blueLine = VGA_PIXEL_Y;
					First_blueLine_found = 1'b1;
					end
				else if (BLUE_LINE_0 <= 9'd10 && First_blueLine_found != 1'b0) begin
					First_blueLine = First_blueLine + 1'b1;
					RED_LINE_0 = 8'b0;
					end
				end
			if ( VGA_PIXEL_Y == (First_blueLine + 6'd10) ) begin
				BLUE_LINE_1 = BLUE_LINE_1 + 1'b1;
				end
			if ( VGA_PIXEL_Y == (First_blueLine + 6'd20) ) begin
				BLUE_LINE_2 = BLUE_LINE_2 + 1'b1;
				end
			*/
			if ( VGA_PIXEL_Y == 52) begin
				BLUE_LINE_0 = BLUE_LINE_0 + 1'b1;
				end
			if ( VGA_PIXEL_Y == 72) begin
				BLUE_LINE_1 = BLUE_LINE_1 + 1'b1;
				end
			if ( VGA_PIXEL_Y == 92) begin
				BLUE_LINE_2 = BLUE_LINE_2 + 1'b1;
				end
					
			end 
		else begin 
			countNULL = countNULL + 16'd1; 
			end
		
		// Usually, we have brighter red, so the red value threshold is higher.
		if ( ((PIXEL_IN[7:5] > 3'd3) && (PIXEL_IN[7:5] > PIXEL_IN[1:0] + 2'b01) && (PIXEL_IN[4:2] < 3'd3)) && prev_X != VGA_PIXEL_X) begin 
			countRED = countRED + 16'd1; 
					
			if ((prev_Y != VGA_PIXEL_Y) && First_redLine_found && (Last_redLine_found == 0)) begin
				Last_redLine = VGA_PIXEL_Y + 1'b1;
				RED_LINE_4 = 10'b0;
				//reg_result[0] = 1'b1;
				end
			
			if((VGA_PIXEL_Y == Last_redLine) && First_redLine_found) begin 
				RED_LINE_4 = RED_LINE_4 + 1'b1;
				//reg_result[1] = 1'b1;
				if (RED_LINE_4 < 9'd30) begin
					Last_redLine_found = 1'b1;
					//reg_result[2] = 1'b1;
					end
				end
				
			if ((prev_Y != VGA_PIXEL_Y) && (First_redLine_found != 1'b1)) begin
				First_redLine = First_redLine + 1'b1;
				//RED_LINE_0 = 8'b0;
				end
			
			//if (prev_Y != VGA_PIXEL_Y) begin 
			//	end
			
			if ( VGA_PIXEL_Y == First_redLine) begin
				RED_LINE_0 = RED_LINE_0 + 1'b1;
				if (RED_LINE_0 > 9'd30) begin
					First_redLine_found = 1'b1;
					end
				end
		
				
			if ( VGA_PIXEL_Y == (Prev_First_redLine + ((Prev_First_redLine - Prev_Last_redLine)*2/6)) ) begin
				RED_LINE_1 = RED_LINE_1 + 1'b1;
				end
				
			if ( VGA_PIXEL_Y == (Prev_First_redLine + ((Prev_First_redLine - Prev_Last_redLine)*3/6)) ) begin
				RED_LINE_2 = RED_LINE_2 + 1'b1;
				end
			
			if ( VGA_PIXEL_Y == (Prev_First_redLine + ((Prev_First_redLine - Prev_Last_redLine)*4/6)) ) begin
				RED_LINE_3 = RED_LINE_3 + 1'b1;
				end
			
			
			
			/*
			if ( VGA_PIXEL_Y == 8'd52 ) begin
				RED_LINE_0 = RED_LINE_0 + 1'b1;
				end
			if ( VGA_PIXEL_Y == 8'd72 ) begin
				RED_LINE_1 = RED_LINE_1 + 1'b1;
				end
			if ( VGA_PIXEL_Y == 8'd92 ) begin
				RED_LINE_2 = RED_LINE_2 + 1'b1;
				end
			*/
			end 
		else begin 
			countNULL = countNULL + 16'd1; 
			end 
				// Storing last Y address	
	  prev_X = VGA_PIXEL_X;
	  prev_Y = VGA_PIXEL_Y;
	end
			
	/*--------------------- Testing Line Number -------------------------
	if (RED_LINE_0 != 8'b0) begin 
		reg_result[0] = 1'b1; 
		end
	if (RED_LINE_1 != 8'b0) begin 
		reg_result[1] = 1'b1; 
		end
	if (RED_LINE_2 != 8'b0) begin 
		reg_result[2] = 1'b1; 
		end		
	if (Prev_Last_redLine != 8'b0) begin 
		reg_result[3] = 1'b1; 
		end
	--------------------- Testing Edge Point -------------------------*/
	
	/*--------------------- Testing Treasure Shape -------------------------*/
	// reg_result[2] : Triangle
	// reg_result[1] : Square
	// reg_result[0] : Diamand
	if (((RED_LINE_1 > 5'd10) && (RED_LINE_2 > 5'd10) && (RED_LINE_3 > 5'd10))
			|| ((BLUE_LINE_1 > 5'd10) && (BLUE_LINE_2 > 5'd10) && (BLUE_LINE_3 > 5'd10))) begin
		
		if ((RED_LINE_2 > RED_LINE_1) && (RED_LINE_2 > RED_LINE_3)) begin
			Diam_CNT = Diam_CNT + 1;
			end
		else if ( (RED_LINE_3 > RED_LINE_1) && (RED_LINE_3 > RED_LINE_2)) begin
			Trian_CNT = Trian_CNT + 1;
			end
		else if ( (((RED_LINE_2 - RED_LINE_1) < 5'd10) || ((RED_LINE_1 - RED_LINE_2) < 5'd10)) && (((RED_LINE_3 - RED_LINE_2) < 5'd10) 
			|| ((RED_LINE_2 - RED_LINE_3) < 5'd10)) && (countRED > R_CNT_THRESHOLD) ) begin
			// ((RED_LINE_1 - RED_LINE_0) < 5'd20 || (RED_LINE_0 - RED_LINE_1) < 5'd20) || ((RED_LINE_2 - RED_LINE_1) < 5'd20 || (RED_LINE_1 - RED_LINE_2) < 5'd20) ||
			Sqr_CNT = Sqr_CNT + 1;
			end
			
		if ((BLUE_LINE_1 > BLUE_LINE_0) && (BLUE_LINE_1 > BLUE_LINE_2)) begin

			Diam_CNT = Diam_CNT + 1;
			end
		if ( (BLUE_LINE_2 > BLUE_LINE_0) && (BLUE_LINE_2 > BLUE_LINE_1 )) begin

			Trian_CNT = Trian_CNT + 1;
			end
		if ( (((BLUE_LINE_1 - BLUE_LINE_0) < 5'd10) || ((BLUE_LINE_0 - BLUE_LINE_1) < 5'd10) ) && ( ((BLUE_LINE_2 - BLUE_LINE_1) < 5'd10) 
			|| ((BLUE_LINE_1 - BLUE_LINE_2) < 5'd10) ) && (countBLUE > B_CNT_THRESHOLD) ) begin
			Sqr_CNT = Sqr_CNT + 1;
			end
	end
	/*-------------------- Testing Treasure Shape -------------------------*/
	
	/*----------------------- Testing color ---------------------------
	if(FRAME_THRESHOLD > countRED && countRED > R_CNT_THRESHOLD) begin 
		reg_result[1] = 1'b1; 
		end 
	else if(FRAME_THRESHOLD > countBLUE && countBLUE > B_CNT_THRESHOLD) begin 
		reg_result[0] = 1'b1; 
		end 
	//// Even if countBLUE is bigger than B_CNT_THRESHOLD, the reg_result[2] is still set to true. 
	else if( (countBLUE <= B_CNT_THRESHOLD) && (countRED <= R_CNT_THRESHOLD) ) begin 
		reg_result[2] = 1'b1;
		end 
	---------------------- Testing color ---------------------------*/
		

	
	
	if((VGA_VSYNC_NEG == 1'b0) && (lastsync == 1'b1)) begin //negedge VSYNC 
		countBLUE = 16'b0; 
		countRED = 16'b0; 
		countNULL = 16'b0; 
		BLUE_LINE_0 = 8'b0;
		BLUE_LINE_1 = 8'b0;
		BLUE_LINE_2 = 8'b0;
		BLUE_LINE_3 = 8'b0;
		BLUE_LINE_4 = 8'b0;
		
		RED_LINE_0 = 8'b0;
		RED_LINE_1 = 8'b0;
		RED_LINE_2 = 8'b0;
		RED_LINE_3 = 8'b0;
		RED_LINE_4 = 8'b0;
		
		// RED
		Prev_First_redLine = First_redLine;
		//firstline = First_redLine;
		First_redLine = 8'b0;
		
		Prev_Last_redLine = Last_redLine;
		//lastline = Last_redLine;
		Last_redLine = 8'b0;
		
		First_redLine_found = 1'b0;
		Last_redLine_found = 1'b0;
		
		// 
		frameCNT = frameCNT + 1;
		//countRED = countRED / frameCNT;
		end 
	
	if ( (frameCNT > 1000) && (VGA_VSYNC_NEG == 1'b0) && (lastsync == 1'b1) ) begin
		if ((Sqr_CNT > Diam_CNT) && (Sqr_CNT > Trian_CNT)) begin
			reg_result[0] = 1'b0; 
			reg_result[1] = 1'b1;
			reg_result[2] = 1'b0;
			end
		else if ( (Trian_CNT > Diam_CNT) && (Trian_CNT > Sqr_CNT) ) begin
			reg_result[0] = 1'b0; 
			reg_result[1] = 1'b0;
			reg_result[2] = 1'b1; 	
			end
		else if ( (Diam_CNT > Trian_CNT) && (Diam_CNT > Sqr_CNT) ) begin
			reg_result[0] = 1'b1; 
			reg_result[1] = 1'b0;
			reg_result[2] = 1'b0; 	
			end
		else begin
			reg_result[0] = 1'b0; 
			reg_result[1] = 1'b0;
			reg_result[2] = 1'b0;
			end
		Trian_CNT = 0;
		Sqr_CNT = 0;
		Diam_CNT = 0;
		frameCNT = 0;
		
		countRED = 16'b0; 
		
		firstline = Prev_First_redLine;
		lastline = Prev_Last_redLine;
		//reg_result = 3'b0;
	end
	

lastsync = VGA_VSYNC_NEG; 

end 

endmodule
