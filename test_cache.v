`default_nettype none
  
module test_cache;
  parameter PERIOD_CLK = 40;
  parameter DUMP_VCD = 0;
  
  parameter WD = 32;
  parameter WA = 20;
  
  reg CLK;
  reg RST_X;

  initial begin
    CLK = 0;
    forever #PERIOD_CLK CLK = ~CLK;
  end

  integer cycles;
  
  reg init_done;
  reg sim_done;

  initial begin
    $cache_initialize;
    cycles = 0;
    init_done = 0;
    sim_done = 0;
    
    RST_X = 1;
    #100;
    RST_X = 0;
    #100;
    RST_X = 1;
    
    init_done = 1;
  end

  initial begin
    #10;
    wait(init_done);
    wait(DONE);
    sim_done = 1;
  end

  always @(posedge CLK) begin
    if(init_done) $cache_step;
  end
  
  always @(posedge CLK) begin
    if(init_done && !sim_done) cycles = cycles + 1;
  end

  initial begin
    #10;
    wait(sim_done);
    $cache_finalize;
    $display("Simulation Cycles : %d", cycles);
    $finish;
  end

  initial begin
    if(DUMP_VCD) begin
      $dumpfile("dump.vcd");
      $dumpvars(0, top, init_done, sim_done);
    end
  end 
  
  wire [WA-1:0] MEM_A;
  wire MEM_RE, MEM_WE;
  wire [WD-1:0] MEM_D;
  reg [WD-1:0] MEM_Q;
  wire MEM_BUSY;
  wire MEM_DONE;
  wire DONE;

  TOP #(.WD(WD), .WA(WA))
  top(.CLK(CLK), .RST_X(RST_X), 
      .MEM_A(MEM_A), .MEM_RE(MEM_RE), .MEM_WE(MEM_WE), 
      .MEM_D(MEM_D), .MEM_Q(MEM_Q), .MEM_BUSY(MEM_BUSY), .MEM_DONE(MEM_DONE),
      .SIM_DONE(DONE));

  reg cache_busy;
  reg cache_done;

  assign MEM_BUSY = cache_busy;
  assign MEM_DONE = cache_done;
  
  reg cache_read_busy;
  reg cache_write_busy;
  reg cache_read_busy_ht;
  reg cache_write_busy_ht;
  
  initial begin
    cache_busy = 0;
    cache_done = 0;
    cache_read_busy = 0;
    cache_write_busy = 0;
  end

  reg [31:0] read_valid;
  reg [31:0] write_valid;
  reg [31:0] read_busy;
  reg [31:0] write_busy;
  
  always @(posedge CLK) begin
    cache_done = 0;
    if(!cache_done && !cache_busy && MEM_RE) begin
      cache_busy = 1;
      cache_read_busy = 1;
      $cache_read_busy(read_busy);
      if(!read_busy) begin
        $cache_read(MEM_A, WD/8, MEM_Q, read_valid);
        if(read_valid) begin
          cache_busy = 0;
          cache_read_busy = 0;
          cache_done = 1;
        end
      end
    end else if(!cache_done && !cache_busy && MEM_WE) begin
      cache_busy = 1;
      cache_write_busy = 1;
      $cache_write_busy(write_busy);
      if(!write_busy) begin
        $cache_write(MEM_A, WD/8, MEM_D, write_valid);
        if(write_valid) begin
          cache_busy = 0;
          cache_write_busy = 0;
          cache_done = 1;
        end
      end
    end 
    if(cache_read_busy) begin
      $cache_read_busy(read_busy);
      if(!read_busy) begin
        $cache_read(MEM_A, WD/8, MEM_Q, read_valid);
        if(read_valid) begin
          cache_busy = 0;
          cache_read_busy = 0;
          cache_done = 1;
        end
      end
    end else if(cache_write_busy) begin      
      $cache_write_busy(write_busy);
      if(!write_busy) begin
        $cache_write(MEM_A, WD/8, MEM_D, write_valid);
        if(write_valid) begin
          cache_busy = 0;
          cache_write_busy = 0;
          cache_done = 1;
        end
      end
    end
  end
endmodule

module TOP(CLK, RST_X, 
           MEM_A, MEM_RE, MEM_WE, MEM_D, MEM_Q, MEM_BUSY, MEM_DONE,
           SIM_DONE
           );
  input CLK, RST_X;
  output [WA-1:0] MEM_A;
  output MEM_RE, MEM_WE;
  output [WD-1:0] MEM_D;
  input [WD-1:0] MEM_Q;
  input MEM_BUSY;
  input MEM_DONE;

  output reg SIM_DONE;

  parameter WD = 32;
  parameter WA = 20;

  reg [WA-1:0] MEM_A;
  reg MEM_RE, MEM_WE;
  reg [WD-1:0] MEM_D;

  reg [7:0] cnt;
  reg [3:0] state;
  
  always @(posedge CLK or negedge RST_X) begin
    if(!RST_X) begin
      state <= 0;
      SIM_DONE <= 0;
    end else begin
      if(state == 0) begin
        MEM_A <= 0;
        MEM_RE <= 0;
        MEM_WE <= 0;
        MEM_D <= 0;
        cnt <= 0;
        state <= 1;
      end else if(state == 1) begin
        MEM_RE <= 0;
        MEM_WE <= 0;
        state <= 2;
      end else if(state == 2) begin
        MEM_A <= MEM_A + 4;
        MEM_D <= MEM_D + 1;
        $display("write: %x", MEM_D);
        MEM_WE <= 1;
        state <= 3;
      end else if(state == 3) begin
        if(MEM_DONE) begin
          MEM_WE <= 0;
          state = 5;
        end
        else if(MEM_BUSY) begin
          MEM_WE <= 0;
          state <= 4;
        end
      end else if(state == 4) begin
        if(MEM_DONE) begin
          state <= 5;
        end
      end else if(state == 5) begin
        MEM_RE <= 1;
        state <= 6;
      end else if(state == 6) begin
        if(MEM_DONE) begin
          $display("Read: %x", MEM_Q);
          MEM_RE <= 0;
          state <= 8;
        end else
        if(MEM_BUSY) begin
          MEM_RE <= 0;
          state <= 7;
        end
      end else if(state == 7) begin
        if(MEM_DONE) begin
          $display("Read: %x", MEM_Q);
          MEM_RE <= 0;
          state <= 8;
        end
      end else if(state == 8) begin
        cnt <= cnt + 1;
        if(cnt == 128) state <= 9;
        else state <= 1;
      end else if(state == 9) begin
        SIM_DONE <= 1;
        state <= 9;
      end
    end
  end
endmodule
