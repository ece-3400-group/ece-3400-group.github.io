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
reg [7:0]	pixel_data_RGB332 = 8'd0;

///// READ/WRITE ADDRESS /////
reg [14:0] X_ADDR;
reg [14:0] Y_ADDR;
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
	.clk_W(CLOCK_50),
	.clk_R(c1_sig),
	.output_data(MEM_OUTPUT)
);

///////* VGA Module *///////
VGA_DRIVER driver (
	.RESET(VGA_RESET),
	.CLOCK(c1_sig),
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

reg [2:0] red;
reg [2:0] green;
reg [1:0] blue;
wire [7:0] color;
assign color[7:5] = red;
assign color[4:2] = green;
assign color[1:0] = blue;

// Downsampling variables
assign D0 = GPIO_1_D[20];
assign D1 = GPIO_1_D[21];
assign D2 = GPIO_1_D[22];
assign D3 = GPIO_1_D[23];
assign D4 = GPIO_1_D[24];
assign D5 = GPIO_1_D[25];
assign D6 = GPIO_1_D[26];
assign D7 = GPIO_1_D[27];
assign PCLK = GPIO_1_D[30];
assign HREF = GPIO_1_D[31];
assign VSYNC = GPIO_1_D[32];

always @ (VGA_PIXEL_X, VGA_PIXEL_Y) begin
		READ_ADDRESS = (VGA_PIXEL_X + VGA_PIXEL_Y*`SCREEN_WIDTH);
		if(VGA_PIXEL_X>(`SCREEN_WIDTH-1) || VGA_PIXEL_Y>(`SCREEN_HEIGHT-1))begin
				VGA_READ_MEM_EN = 1'b0;
		end
		else begin
				VGA_READ_MEM_EN = 1'b1;
		end
end

reg HREF_change, HREF_1;
assign HREF_change = HREF_1 ^ HREF;
reg VSYNC_change, VSYNC_1;
assign VSYNC_change = VSYNC_1 ^ VSYNC;

reg flag = 1'b0;
reg is_image_started = 1'b1;
reg is_new_row = 1'b0;
always @ (posedge PCLK) begin
   // Handling when an image frame starts or ends
   if (VSYNC_change & VSYNC_1) begin // Image TX on falling edge started
	     W_EN = 1'b1;
        X_ADDR = 15'd0;
	     Y_ADDR = 15'd0;
	     //is_image_started = VSYNC;
   end
	else begin // Image TX ends on rising edge
	     is_image_started = 1'b0;
		  W_EN = 1'b0;
	end
	
	// Handle when Camera sends a new row of images
	if (change & HREF_1) begin // new row TX on rising edge
	     //is_new_row = 1'b1;
		  X_ADDR = X_ADDR + 15'd1;
	end
	else begin // row TX ends on falling edge, must be a new row
	     //is_new_row = 1'b0;
		  X_ADDR = 15'd0;
		  Y_ADDR = Y_ADDR + 15d'1;
	end
	
	// Safe current HREF for the next HREF_1 use, (HREF ^ HREF_1) & HREF_1 will detect the posedge of HREF.
	if (VSYNC == 1'b1) begin	//SYNC indicates the end (rising edge) of a frame of an image
		HREF_1 <= 0;			
	end
	else begin
		HREF_1 <= HREF;
	end
	
	// Safe current HREF for the next HREF_1 use, (HREF ^ HREF_1) & HREF_1 will detect the posedge of HREF.
	if (VGA_RESET) begin	//SYNC indicates the end (rising edge) of a frame of an image
		VSYNC <= 0;			
	end
	else begin
		VSYNC_1 <= VSYNC;
	end
	
	// = is blocking statement. In an always block, the line of code will be executed only after it's previous line has executed. 
	// Hence, they happens one after the other, just like combinatoral logics in loop.
	// <= is non-blocking in nature. This means that in an always block, every line will be executed in parallel. 
	// Hence leading to implementation of sequential elements.
	
    // Gather the color data (D0-D7) from camera
    if (flag == 1'b0) begin
		red = {D6, D5, D4};
		green[2:1] = {D1, D0};
		blue = blue;
		flag = 1'b0;
    end
	else begin
		red = red;
		green[0] = {D7};
		blue = {D4, D3};
		flag = 1'b1;
	end 
	 
	 // Now write the color to memory (since write address has been setup)
	 pixel_data_RGB332 = color;
end
	
endmodule 
