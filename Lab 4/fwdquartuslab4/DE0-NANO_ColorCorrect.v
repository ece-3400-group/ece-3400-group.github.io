`define SCREEN_WIDTH 176
`define SCREEN_HEIGHT 144

///////* DON'T CHANGE THIS PART *///////
module DE0_NANO(
	CLOCK_50,
	GPIO_0_D,
	GPIO_1_D,
	KEY
);

//=======================================================
//  PARAMETER declarations
//=======================================================
localparam RED = 8'b111_000_00;
localparam GREEN = 8'b000_111_00;
localparam BLUE = 8'b000_000_11;

//=======================================================
//  PORT declarations
//=======================================================

//////////// CLOCK - DON'T NEED TO CHANGE THIS //////////
input 		          		CLOCK_50;

//////////// GPIO_0, GPIO_0 connect to GPIO Default //////////
output 		    [33:0]		GPIO_0_D;
//////////// GPIO_0, GPIO_1 connect to GPIO Default //////////
input 		    [33:20]		GPIO_1_D;
input 		     [1:0]		KEY;

///// PIXEL DATA /////
reg [7:0]	pixel_data_RGB332 = 8'b111_111_11;

///// READ/WRITE ADDRESS /////
reg [14:0] X_ADDR = 0;
reg [14:0] Y_ADDR = 0;
wire [14:0] WRITE_ADDRESS;
reg [14:0] READ_ADDRESS; 

assign WRITE_ADDRESS = X_ADDR + Y_ADDR*(`SCREEN_WIDTH);

///// VGA INPUTS/OUTPUTS /////
wire 			VGA_RESET;
wire [7:0]	VGA_COLOR_IN;
wire [9:0]	VGA_PIXEL_X;
wire [9:0]	VGA_PIXEL_Y;
wire [7:0]	MEM_OUTPUT;
wire			VGA_VSYNC_NEG;
wire			VGA_HSYNC_NEG;
reg			VGA_READ_MEM_EN;
reg  [7:0]  PIXEL_COLOR;

assign GPIO_0_D[5] = VGA_VSYNC_NEG;
assign VGA_RESET = ~KEY[0];

///// I/O for Img Proc /////
wire [8:0] RESULT;

/* WRITE ENABLE */
reg W_EN;

///////* CREATE ANY LOCAL WIRES YOU NEED FOR YOUR PLL *///////

///////* INSTANTIATE YOUR PLL HERE *///////
///////* INSTANTIATE YOUR PLL HERE *///////
sweetPLL	sweetPLL_inst (
	.inclk0 ( CLOCK_50 ),
	.c0 ( c0_sig ), // 24 MHz
	.c1 ( c1_sig ), // 25 MHz
	.c2 ( c2_sig ) // 50 MHz
	);

///////* M9K Module *///////
Dual_Port_RAM_M9K mem(
	.input_data(pixel_data_RGB332),
	.w_addr(WRITE_ADDRESS),
	.r_addr(READ_ADDRESS),
	.w_en(W_EN),
	.clk_W(c2_sig),
	.clk_R(c1_sig),
	.output_data(MEM_OUTPUT)
);

///////* VGA Module *///////
VGA_DRIVER driver (
	.RESET(VGA_RESET),
	.CLOCK(c1_sig),
	//.PIXEL_COLOR_IN(COLOR),
	.PIXEL_COLOR_IN(VGA_READ_MEM_EN ? MEM_OUTPUT : BLUE),
	.PIXEL_X(VGA_PIXEL_X),
	.PIXEL_Y(VGA_PIXEL_Y),
	.PIXEL_COLOR_OUT({GPIO_0_D[9],GPIO_0_D[11],GPIO_0_D[13],GPIO_0_D[15],GPIO_0_D[17],GPIO_0_D[19],GPIO_0_D[21],GPIO_0_D[23]}),
   .H_SYNC_NEG(GPIO_0_D[7]),
   .V_SYNC_NEG(VGA_VSYNC_NEG)
);

///////* Image Processor *///////
IMAGE_PROCESSOR proc(
	.PIXEL_IN(MEM_OUTPUT),
	.CLK(c1_sig),
	.VGA_PIXEL_X(VGA_PIXEL_X),
	.VGA_PIXEL_Y(VGA_PIXEL_Y),
	.VGA_VSYNC_NEG(VGA_VSYNC_NEG),
	.RESULT(RESULT)
);


///////* Update Read Address *///////
//always @ (VGA_PIXEL_X, VGA_PIXEL_Y) begin
//		READ_ADDRESS = (VGA_PIXEL_X + VGA_PIXEL_Y*`SCREEN_WIDTH);
//		if(VGA_PIXEL_X>(`SCREEN_WIDTH-1) || VGA_PIXEL_Y>(`SCREEN_HEIGHT-1))begin
//				PIXEL_COLOR = BLUE;
//		end
//		else begin
//				PIXEL_COLOR = RED;
//		end
//end

reg [2:0] red;   // 3 bits
reg [2:0] green; // 3 bits
reg [1:0] blue;  // 2 bits
wire [7:0] color;
assign color[7:5] = red;
assign color[4:2] = green;
assign color[1:0] = blue;

// Downsampling variables
assign D0 = GPIO_1_D[20];
assign D1 = GPIO_1_D[21];
//assign D0 = 1'b1;
//assign D1 = 1'b1;
assign D2 = GPIO_1_D[22];
assign D3 = GPIO_1_D[23];
assign D4 = GPIO_1_D[24];
assign D5 = GPIO_1_D[25];
assign D6 = GPIO_1_D[26];
//assign D4 = 1'b1;
//assign D5 = 1'b1;
//assign D6 = 1'b1;
assign D7 = GPIO_1_D[27];
assign PCLK = GPIO_1_D[28];
assign HREF = GPIO_1_D[29];
assign VSYNC = GPIO_1_D[30];

assign GPIO_0_D[0] = c0_sig;
//assign GPIO_0_D[33] = X_ADDR[0];


always @ (VGA_PIXEL_Y, VGA_PIXEL_X) begin
		READ_ADDRESS = (VGA_PIXEL_X + VGA_PIXEL_Y*`SCREEN_WIDTH);
		if(VGA_PIXEL_X>(`SCREEN_WIDTH-1) || VGA_PIXEL_Y>(`SCREEN_HEIGHT-1))begin
				VGA_READ_MEM_EN = 1'b0;
		end
		else begin
				VGA_READ_MEM_EN = 1'b1;
		end
end

reg flag = 0;
reg reach2btye = 0;
assign GPIO_0_D[1] = flag;
reg is_image_started = 1'b1;
reg is_new_row = 1'b0;
reg is_new_byte = 1'b1;
reg [7:0] COLOR = BLUE;

reg prev_href = 1'b0;
reg prev_vsync = 1'b0;

always @ (posedge PCLK) begin
// // Gather the color data (D0-D7) from camera
//			 if (flag == 1'b0) begin
//				  red = {D6, D5, D4};
//				  green = {D1, D0};
//				  blue = 0;
//				  flag = 1'b1;
//				  W_EN = 1'b0;
//				   X_ADDR = X_ADDR;
//				  Y_ADDR =Y_ADDR;
//				  pixel_data_RGB332 [7:3] = {D0, D1, D4, D5, D6};
////	 X_ADDR = X_ADDR + 15'd1;
////	  if (X_ADDR == 15'd176) begin
////	  Y_ADDR = Y_ADDR + 1'b1;
////	  end
////	  	W_EN = 1'b1;
////	  COLOR = RED;
////	  pixel_data_RGB332 = COLOR;
//			 end
//			 else begin
//				  red = red;
//				  green = green;
//				  blue = {D4, D3, D2};
//				  flag = 1'b0;
//				  pixel_data_RGB332 [2:0]= {D2, D3, D4}; // Now write the color to memory (since write address has been setup)
//
//				  X_ADDR = X_ADDR + 15'd1;
//				  Y_ADDR =Y_ADDR;
//				   W_EN = 1'b1;
// //X_ADDR = X_ADDR + 15'd1;
////	  if (X_ADDR == 15'd176) begin
////	  Y_ADDR = Y_ADDR + 1'b1;
////	  end
////	  	W_EN = 1'b1;
////	  COLOR = GREEN;
////	  pixel_data_RGB332 = COLOR;
//			 end 

// start here
//
     //W_EN = 1'b0;
//	  X_ADDR = X_ADDR + 15'd1;
//	  if (X_ADDR == 15'd176) begin
//	  Y_ADDR = Y_ADDR + 1'b1;
//	  end
//	  W_EN = 1'b1;
//	  COLOR = RED;
//	  pixel_data_RGB332 = COLOR;
//     pixel_data_RGB332 = 8'd0;

   // Handling when an image frame starts or ends
   if (VSYNC & ~HREF & ~prev_href & ~prev_vsync) begin // Image TX on falling edge started
  // if (VSYNC == 1'b1) begin // Image TX on falling edge started
	    // W_EN = 1'b0;
		  X_ADDR = 0;
	     Y_ADDR = 0;
		  flag = 1'b0; //reset flag

//	  X_ADDR = X_ADDR + 15'd1;
//	  if (X_ADDR == 15'd176) begin
//	  Y_ADDR = Y_ADDR + 1'b1;
//	  end
//	  W_EN = 1'b1;
//	  COLOR = BLUE;
//	  pixel_data_RGB332 = COLOR;
		  
	end
	
	// Handle when Camera sends a new row of images
	else begin
//	   if (Y_ADDR >= 15'd176) begin
//	   	W_EN <= 1'b0;
//	   end
		
   	if (~HREF & prev_href) begin // row TX ends on falling edge, must be a new row (HREF == 1'b0)
		     X_ADDR = 0;
		     Y_ADDR = Y_ADDR + 1; 
	   end
	   else if (HREF) begin  // new row TX on rising edge
			 // Gather the color data (D0-D7) from camera
			 if (~flag) begin
			     W_EN = 1'b0;
				  //red = {D6, D5, D4};
				  //red = {1'b0, 1'b0, 1'b0};
				  //green[2:1] = {D1, D0};
				  //red[2:0] <= {1'b0, 1'b0, 1'b0}; //{D7, D6, D5};
				  //green[2:0] <= {1'b0, 1'b0, 1'b0}; 
				  //green[2:1] <= {1'b1, 1'b1};
				  //blue = blue;
				  flag = 1'b1;
				  X_ADDR = X_ADDR;
				  Y_ADDR = Y_ADDR;
				  pixel_data_RGB332[2:0] = {D7, D4, D3};
//	 X_ADDR = X_ADDR + 15'd1;
//	  if (X_ADDR == 15'd176) begin
//	  Y_ADDR = Y_ADDR + 1'b1;
//	  end
//	  	W_EN = 1'b1;
//	  COLOR = RED;
//	  pixel_data_RGB332 = COLOR;
			 end
			 else begin
				  //red[2:0] = red;
				 // green[0] = {D7};
				 // green[2:0] = {1'b0, 1'b0, 1'b0};
				  //blue[1:0] = {1'b0, 1'b0};
				  //green[0] <= 1'b1;
				  //blue = {D4, D3};
				  //blue[1:0] <= {1'b1, 1'b1};
				  flag = 1'b0;
				  pixel_data_RGB332[7:3] = {D6, D5, D4, D1, D0}; // Now write the color to memory (since write address has been setup)
					X_ADDR = X_ADDR + 1;
				  Y_ADDR = Y_ADDR;
//				  if (X_ADDR < 10'b01111111) begin
//				   W_EN = 1'b1;
//					end
//					else begin W_EN = 1'b0; end
			end 
					

 //X_ADDR = X_ADDR + 15'd1;
//	  if (X_ADDR == 15'd176) begin
//	  Y_ADDR = Y_ADDR + 1'b1;
//	  end
//	  	W_EN = 1'b1;
//	  COLOR = GREEN;
//	  pixel_data_RGB332 = COLOR;
		 end
	end
	X_ADDR = X_ADDR;
	Y_ADDR = Y_ADDR;
	prev_href = HREF;
	prev_vsync = VSYNC;
	W_EN = 1'b1;
end
///////* Update Read Address *///////
//always @ (posedge PCLK) begin
//	if(VSYNC) begin
//			X_ADDR = 0;
//	end
//	else begin
//		if(HREF) begin			
//			if(flag == 0) begin 
//				pixel_data_RGB332[7:2] = {D0,D1,D2,D5,D6,D7};
//				flag = 1;
//				W_EN = 0;
//			end
//			else begin
//				pixel_data_RGB332[1:0] = {D3,D4};
//				flag = 0;
//				X_ADDR = X_ADDR + 1;
//				W_EN = 1;
//			end				
//		end
//		else begin
//			X_ADDR = 0;
//		end
//	end
//end
//
//always @ (negedge HREF or posedge VSYNC) begin
//	if (VSYNC) begin
//		Y_ADDR = 0;
//	end
//	else begin
//		Y_ADDR = Y_ADDR + 1;
//	end
//end
//	
endmodule 
