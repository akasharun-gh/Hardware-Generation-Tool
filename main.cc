/**************************************************
AUTHORS : SANKAR RACHURU, AKASH ARUN
FILE NAME : main.cc
ESE 507 Project 3
**************************************************/



#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <assert.h>
#include <math.h>
using namespace std;

void printUsage();
void genLayer(int N, int M, int T, int P, vector<int>& constvector, string modName, ofstream &os);
void genAllLayers(int N, int M1, int M2, int M3, int T, int A, vector<int>& constVector, string modName, ofstream &os);
void readConstants(ifstream &constStream, vector<int>& constvector);
void genROM(vector<int>& constVector, int bits, string modName, ofstream &os);
int* factors(int L);




int main(int argc, char* argv[]) {

   // If the user runs the program without enough parameters, print a helpful message
   // and quit.
   if (argc < 7) {
      printUsage();
      return 1;
   }

   int mode = atoi(argv[1]);

   ifstream const_file;
   ofstream os;
   vector<int> constVector;

   //----------------------------------------------------------------------
   // Look here for Part 1 and 2
   if ((mode == 1) && (argc == 7)) {
      // Mode 1: Generate one layer with given dimensions and one testbench

      // --------------- read parameters, etc. ---------------
      int N = atoi(argv[2]);
      int M = atoi(argv[3]);
      int T = atoi(argv[4]);
      int P = atoi(argv[5]);
      const_file.open(argv[6]);
      if (const_file.is_open() != true) {
         cout << "ERROR reading constant file " << argv[6] << endl;
         return 1;
      }

      // Read the constants out of the provided file and place them in the constVector vector
      readConstants(const_file, constVector);

      string out_file = "conv_" + to_string(N) + "_" + to_string(M) + "_" + to_string(T) + "_" + to_string(P) + ".sv";

      os.open(out_file);
      if (os.is_open() != true) {
         cout << "ERROR opening " << out_file << " for write." << endl;
         return 1;
      }
      // -------------------------------------------------------------

      // call the genLayer function you will write to generate this layer
      string modName = "conv_" + to_string(N) + "_" + to_string(M) + "_" + to_string(T) + "_" + to_string(P);
      genLayer(N, M, T, P, constVector, modName, os); 

   }
   //--------------------------------------------------------------------


   // ----------------------------------------------------------------
   // Look here for Part 3
   else if ((mode == 2) && (argc == 9)) {
      // Mode 2: Generate three layer with given dimensions and interconnect them

      // --------------- read parameters, etc. ---------------
      int N  = atoi(argv[2]);
      int M1 = atoi(argv[3]);
      int M2 = atoi(argv[4]);
      int M3 = atoi(argv[5]);
      int T  = atoi(argv[6]);
      int A  = atoi(argv[7]);
      const_file.open(argv[8]);

	  if(A<3)
	  {
	   	  cout << "  ERROR: minimum number of multipliers required is 3" << endl;
		  return 1;
	  }
	
      if (const_file.is_open() != true) {
         cout << "ERROR reading constant file " << argv[8] << endl;
         return 1;
      }
      readConstants(const_file, constVector);

      string out_file = "multi_" + to_string(N) + "_" + to_string(M1) + "_" + to_string(M2) + "_" + to_string(M3) + "_" + to_string(T) + "_" + to_string(A) + ".sv";


      os.open(out_file);
      if (os.is_open() != true) {
         cout << "ERROR opening " << out_file << " for write." << endl;
         return 1;
      }
      // -------------------------------------------------------------

      string mod_name = "multi_" + to_string(N) + "_" + to_string(M1) + "_" + to_string(M2) + "_" + to_string(M3) + "_" + to_string(T) + "_" + to_string(A);

      // call the genAllLayers function
      genAllLayers(N, M1, M2, M3, T, A, constVector, mod_name, os);

   }
   //-------------------------------------------------------

   else {
      printUsage();
      return 1;
   }

   // close the output stream
   os.close();

}

// Read values from the constant file into the vector
void readConstants(ifstream &constStream, vector<int>& constvector) {
   string constLineString;
   while(getline(constStream, constLineString)) {
      int val = atoi(constLineString.c_str());
      constvector.push_back(val);
   }
}

// Generate a ROM based on values constVector.
// Values should each be "bits" number of bits.
void genROM(vector<int>& constVector, int bits, string modName, ofstream &os) {

      int numWords = constVector.size();
      int addrBits = ceil(log2(numWords));

      os << "module " << modName << "(clk, addr, z);" << endl;
      os << "   input clk;" << endl;
      os << "   input [" << addrBits-1 << ":0] addr;" << endl;
      os << "   output logic signed [" << bits-1 << ":0] z;" << endl;
      os << "   always_ff @(posedge clk) begin" << endl;
      os << "      case(addr)" << endl;
      int i=0;
      for (vector<int>::iterator it = constVector.begin(); it < constVector.end(); it++, i++) {
         if (*it < 0)
            os << "        " << i << ": z <= -" << bits << "'d" << abs(*it) << ";" << endl;
         else
            os << "        " << i << ": z <= "  << bits << "'d" << *it      << ";" << endl;
      }
      os << "      endcase" << endl << "   end" << endl << "endmodule" << endl << endl;
}

// Parts 1 and 2
// Here is where you add your code to produce a neural network layer.
void genLayer(int N, int M, int T, int P, vector<int>& constVector, string modName, ofstream &os) {
  
	int adr_x_bits = ceil(log2(N)); 
	int adr_f_bits = ceil(log2(M)); 
	int L = N-M+1;
	int i;
	os << "module " << modName << "(clk, reset, s_valid, m_ready, data_in, m_valid, s_ready, data_out);" << endl;
	os << "    parameter ADDR_X_SIZE = " <<  adr_x_bits << ", ADDR_F_SIZE = " << adr_f_bits << ", N = " << N << " , M = " << M << ", T = " << T << ";" << endl ;
	os << "    input clk, reset, s_valid, m_ready;" << endl ;
	os << "    input signed [T-1:0] data_in;" << endl ;
	os << "    output signed [T-1:0] data_out;" << endl ;
	os << "    output logic m_valid;" << endl ;
    os << "    output logic s_ready;" << endl;
	os << "    logic signed [T-1:0] dout2;" << endl ;	
	os << "    logic read_en;" << endl;
	os << "    logic [ADDR_F_SIZE-1:0] addr_f;" << endl << endl;
	os << "    logic en_mux;" << endl;
	os << "    logic wr_en_x;" << endl;
		for(i = 1; i <= P; i++)
		{
			os << "    logic wr_en_x_"<< i << ", clear_acc_"<< i << ", en_acc_"<< i << ", s_ready_"<< i <<", en_mux_"<< i << ";" << endl ;	
		}
		for(i = 1; i <= P; i++)
		{
			os << "    logic [ADDR_X_SIZE-1:0] addr_x_"<< i << ";" << endl ;
		}
		for(i = 1; i <= P; i++)
		{
			os << "    logic [ADDR_F_SIZE-1:0] addr_f_"<< i << ";" << endl ;
		}
		for(i = 1; i <= P; i++)
		{
			os << "    logic signed [T-1:0] dout1_"<< i << ";" << endl ;
		}
		for(i = 1; i <= P; i++)
		{
			os << "    logic signed [T-1:0] add_"<< i << ", acc_"<< i << ", mult_"<< i << ";" << endl;
		}	
		for(i = 1; i <= P; i++)
		{
		    os << "    logic signed [T-1:0] data_out_" << i << " ; " << endl;
		}
		for(i = 1; i <= P; i++)
		{		
			os << "    controlBlock_" << N << " #(T, N, M, ADDR_X_SIZE, ADDR_F_SIZE, "<< i << ", " << P <<") cblk" << i << "(.clk(clk), .reset(reset), .s_valid(s_valid), .read_en(read_en), .addr_x(addr_x_"<< 			i << "), .wr_en_x(wr_en_x_"<< i << "), .addr_f(addr_f_"<< i<<")," << endl ;
			os << "		       .clear_acc(clear_acc_"<< i << "), .en_acc(en_acc_"<< i << "), .s_ready(s_ready_"<< i <<"), .data_out(data_out_"<< i << ")," << endl ; 
			os << "		       .add(add_"<< i << "), .acc(acc_"<< i << "), .mult(mult_"<< i << "), .din1(dout1_"<< i << "), .din2(dout2), .en_mux(en_mux_"<< i <<"));" << endl <<endl ;
		}
		for(i = 1; i <= P; i++)
		{
			os << "    memory_" << N << " #(T, N, ADDR_X_SIZE) M_x_"<< i<<"(.clk(clk), .data_in(data_in), .data_out(dout1_"<< i <<"), .addr(addr_x_"<< i << "), .wr_en(wr_en_x));" << endl <<endl;
		}
		for(i = 1; i <= P; i++)
		{
			os << "    dataPath_" << N << " dpath_"<< i << "(.reset(reset), .clear_acc(clear_acc_"<< i << "), .en_acc(en_acc_"<< i << "), .add(add_"<< i << "), .acc(acc_"<< i << "), .mult(mult_"<< i << "));" 			<< endl <<  endl;
		}
	os << "    mux_" << N << " #(T) Mux(.clk(clk), .reset(reset), .read_en(read_en), .m_ready(m_ready), .m_valid(m_valid), .en_mux(en_mux), " ;
	    for(i = 1; i <= P; i++)
		{
			os << ".data_out_" << i << "(data_out_" << i << "), " ;
		}
	os << 	 ".data_out(data_out)); " << endl << endl;
	os << "    " << modName << "_f_rom fROM(.clk(clk), .addr(addr_f), .z(dout2));" << endl << endl;
	os << "    ready_control_" << N << " #(ADDR_F_SIZE) rc(.clk(clk), .addr_f(addr_f), " ;
        for(i = 1; i <= P; i++)
		{
			os << ".addr_f_" << i << "(addr_f_" << i << "), " ;
		}
        for(i = 1; i <= P; i++)
		{
			os << ".s_ready_" << i << "(s_ready_" << i << "), " ;
		}
		 for(i = 1; i <= P; i++)
		{
			os << ".en_mux_" << i << "(en_mux_" << i << "), " ;
		}
		 for(i = 1; i <= P; i++)
		{
			os << ".wr_en_x_" << i << "(wr_en_x_" << i << "), " ;
		}
	os << ".en_mux(en_mux), .wr_en_x(wr_en_x), " << endl;
	os << "		.s_ready(s_ready));" << endl << endl;
	os << "endmodule" << endl << endl;

	os << "module controlBlock_" << N << " (clk, reset, s_valid, addr_x, wr_en_x, addr_f, clear_acc, en_acc, s_ready, add, data_out, acc, mult, din1, din2, en_mux, read_en);" << endl << endl;

	os << "    parameter  T= 20, N = 16, M = 4, ADDR_X_SIZE = 4, ADDR_F_SIZE = 2, I = 1, P = "<< P << "; " << endl;
	os << "    parameter L = N-M+1; " << endl;
	os << "    input clk, reset, s_valid, read_en; " << endl;
	os << "    input signed [T-1:0] din1; " << endl;
	os << "    input signed [T-1:0] din2; " << endl;
	os << "    input signed [T-1:0] acc; " << endl;
	os << "    output logic wr_en_x, clear_acc, en_acc, s_ready, en_mux; " << endl;
	os << "    output logic [ADDR_X_SIZE-1:0] addr_x; " << endl;
	os << "    output logic [ADDR_F_SIZE-1:0] addr_f; " << endl;
	os << "    output logic signed[T-1:0] add; " << endl;
	os << "    output logic signed [T-1:0] data_out; " << endl;
	os << "    output logic signed [T-1:0] mult; " << endl;
	os << "    logic read, flag;" << endl;
	int inc_x = ceil(log2(L));
	os << "    logic ["<< inc_x-1 << ":0] init;" << endl;
	os << "    logic [ADDR_X_SIZE:0] adrCountx, countx; " << endl; 
	os << "    logic [ADDR_F_SIZE:0] countf; " << endl << endl;

	os << "    always_ff@(posedge clk)" << endl;
	os << "    begin" << endl;
	os << "    if(reset)" << endl ;
	os << "     begin " << endl;
	os << " 	    addr_x <= 0;" << endl;
	os << " 	    addr_f <= 0;" << endl;  
	os << " 	    wr_en_x <= 1;" << endl;
	os << " 	    clear_acc <= 1;" << endl;
	os << " 		en_acc <= 0;" << endl;
	os << " 		s_ready <= 1;" << endl;
	os << " 		flag <= 0;" << endl;
	os << " 		read <= 0;" << endl;
	os << " 		init <= I-1;" << endl;
	os << " 		adrCountx <= 0;" << endl;
	os << " 		countx <= 0;" << endl;
	os << " 		countf <= 0;" << endl;
	os << " 		add <= 0;" << endl;
	os << " 		mult <= 0;" << endl;
	os << " 	    en_mux <= 0;" << endl; 
	os << " 	end" << endl;
						
	os << " 	if(read)" << endl;	
	os << " 	begin" << endl;
	os << "			if(countx <= L)" << endl;
	os << " 			begin" << endl;
	os << " 				if(addr_x == N-(P-I+1) && addr_f == M-1)" << endl; 
	os << " 			    begin " << endl;
	os << " 					addr_x <= 0;" << endl;
	os << " 					addr_f <= 0;" << endl;
	os << " 					countx <= countx + P;" << endl; 
	os << " 					countf <= countf + 1;" << endl;
	os << " 					add <= acc;" << endl;
	os << " 					mult <= din1 * din2;" << endl;
	os << " 				end" << endl;
	os << " 			    else" << endl;
	os << " 			    begin" << endl;
	os << " 					if(countf == M+1)" << endl;
	os << " 					begin" << endl;
	os << " 						if(read_en)" << endl;
	os << " 						begin" << endl;
	os << " 							countf <= 0;" << endl;
	os << "								en_mux <= 0;" << endl;
	os << "								if(countx == L)" << endl;
	os << "								countx <= countx + P;" << endl;
	os << " 						end" << endl;
	os << " 						else"  <<endl ;
	os << " 						begin" << endl;
	os << " 							if(flag == 0)" << endl ;
	os << " 							begin" << endl;
	os << " 								data_out <= acc;" << endl;
	os << " 								flag <= 1;" << endl;
	os << " 							end" << endl;
	os << " 						en_acc <= 0;" << endl;
	os << "                         en_mux <= 1;" << endl;
	os << " 						addr_f <= 0;" << endl;
	os << " 						addr_x <= init;" << endl;
	os << " 						clear_acc <= 1;" << endl;
	os << " 						end" << endl;
	os << " 					end" << endl;
	os << " 					else if(countf == M)" << endl;
	os << " 					begin" << endl;
	os << " 						mult <= din1 * din2;" << endl;
	os << " 						add <= acc;" << endl;
	os << " 						countf <= countf+1;" << endl;
	os << " 					end" << endl;
	os << " 					else if(addr_f == M-1)" << endl;
	os << " 					begin" << endl;						
	os << " 						init <= init + P;" << endl; 
	os << " 						countf <= countf + 1;" << endl;
	os << " 						countx <= countx + P;" << endl;
	os << " 						en_acc <= 1;" << endl;
	os << " 						add<= acc;" << endl;
	os << " 						mult <= din1 * din2;" << endl;						
	os << " 					end" << endl;
	os << " 					else " << endl;
	os << " 					begin" << endl;
	os << " 						if(countf == 1)" << endl; 
	os << " 						begin" << endl;
	os << " 							en_acc <= 1;" << endl;
	os << " 							clear_acc <= 0;" << endl;
	os << " 						end" << endl;
	os << " 					flag <= 0;" << endl;
	os << " 					add <= acc;" << endl;
	os << " 					addr_x <= addr_x + 1;" << endl;
	os << " 					addr_f <= addr_f + 1;" << endl;
	os << " 					countf <= countf + 1;" << endl;
	os << " 					mult <= din1 * din2;" << endl;
	os << " 					end" << endl;
	os << " 			    end" << endl;
	os << " 			end " << endl; 
	os << " 			else " << endl;
	os << " 			begin" << endl;
	os << " 				addr_x <= 0;" << endl;
	os << " 				addr_f <= 0;" << endl;
	os << " 				wr_en_x <= 1;" << endl;
	os << " 				clear_acc <= 1;" << endl;
	os << " 				en_acc <= 0;" << endl;
	os << " 				s_ready <= 1;" << endl;
	os << " 				flag <= 0;" << endl;
	os << " 				read <= 0;" << endl;
	os << " 				init <= I-1;" << endl;
	os << " 				adrCountx <= 0;" << endl;
	os << " 				countx <= 0;" << endl;
	os << " 				countf <= 0;" << endl;
	os << " 				add <= 0;" << endl;
	os << " 				mult <= 0;" << endl;	
	os << "				    en_mux <= 0;" << endl;
	os << " 			end" << endl;
	os << " 	end" << endl;

	os << "	else " << endl;
	os << "		begin " << endl;
	os << "			if(adrCountx <= N) " << endl;
	os << "			begin " << endl;
	os << "				if(wr_en_x == 0 && adrCountx <= N-1) " << endl;
	os << "				begin " << endl;
	os << "					wr_en_x <= 1; " << endl;
	os << "					s_ready <= 1; " << endl;
	os << "				end " << endl;
	os << "				else " << endl;
	os << "				begin " << endl;
	os << "					if(adrCountx == N) " << endl;
	os << "					begin " << endl;
	os << "						adrCountx <= adrCountx + 1; " << endl;
	os << "						addr_x <= I-1; " << endl;
	os << "						s_ready <= 0; " << endl;
	os << "						wr_en_x <= 0; " << endl;
	os << "						read <= 1; " << endl;
	os << "					end " << endl;
	os << "					else  " << endl;
	os << "					begin " << endl;
	os << "							if(adrCountx <= N-1) " << endl;
	os << "							begin " << endl;
	os << "									if(s_valid) " << endl;
	os << "									begin " << endl;
	os << "										addr_x <= addr_x + 1; " << endl;
	os << "										adrCountx <= adrCountx + 1; " << endl;
	os << "										wr_en_x <= 0; " << endl;
	os << "										s_ready <= 0; " << endl;
	os << "									end " << endl;
	os << "							end " << endl;
	os << "					end " << endl;
	os << "				end	 " << endl;
	os << "			end  " << endl;
	os << "		end  " << endl;
	os << "	end	 " << endl;
	os << " endmodule " << endl << endl;
					
// the datapath module
	os << "module dataPath_" << N << "( reset, clear_acc, en_acc, add, acc, mult);" << endl << endl;

	os << "    input clear_acc, en_acc, reset;" << endl;
	os << "    input logic signed[" << T-1 <<":0] add, mult;" << endl;
	os << "    output logic signed[" << T-1 <<":0] acc;" << endl << endl;
	
	os << "    always_comb begin " << endl;
	os << "        if(reset) " << endl;
	os << "            acc = 0; " << endl;
	os << "		   else " << endl;
	os << "		   begin " << endl;
	os << "			   if(en_acc == 1) " << endl;
	os << "		       begin " << endl;
	os << "			       acc = mult + add; " << endl;
	os << "		       end " << endl;
	os << "		       else if(clear_acc == 1) " << endl;
	os << "		       begin " << endl;
	os << "			       acc = 0; " << endl;
	os << "		       end " << endl;
	os << "		       else  " << endl;
	os << "			       acc = 0; " << endl;
	os << "		       end " << endl;
	os << "	       end " << endl;
	os << "endmodule " << endl << endl;


// the memory module
	os << " module memory_" << N << "(clk, data_in, data_out, addr, wr_en); " << endl << endl;
	   
	os << "    parameter WIDTH=8, SIZE=64, LOGSIZE=6; " << endl;
	os << "    input [WIDTH-1:0] data_in; " << endl;
	os << "    output logic [WIDTH-1:0] data_out; " << endl;
	os << "    input [LOGSIZE-1:0] addr; " << endl;
	os << "    input clk, wr_en; " << endl;
	os << "    logic [SIZE-1:0][WIDTH-1:0] mem; " << endl << endl;

	os << "    always_ff @(posedge clk) begin " << endl;
	os << "        data_out <= mem[addr]; " << endl;
	os << " 	       if (wr_en) " << endl;
	os << " 	           mem[addr] <= data_in; " << endl;
	os << "    end " << endl;
	os << " endmodule " << endl << endl;
	
	
// the mux module required to output one data at a time
	os << "module mux_" << N << "(clk, reset, read_en, m_ready, m_valid, en_mux,  ";
	    for(i = 1; i <= P; i++)
		{
			os << "data_out_" << i << ", " ;
		}
	os << "	data_out);" << endl;
	os << "    parameter T = 1;" << endl;
	for(i = 1; i <= P; i++)
		{
			os << "    input signed [ T-1 : 0] data_out_" << i << ";" << endl;
		}
	os << "    input m_ready, clk, reset, en_mux; " << endl;
	os << "    output logic m_valid, read_en; " << endl;
	os << "    output logic signed [T-1:0] data_out; " << endl;
	os << "    logic ["<< P <<" : 0] sel; " << endl<<endl;

	os << "	   always_ff@(posedge clk) " << endl;
	os << "    begin " << endl;
	os << "	     if(reset == 1) " << endl;
	os << "	     begin " << endl;
	os << "			sel <= 0; " << endl;
	os << "			m_valid <= 0; " << endl;
	os << "			read_en <= 0; " << endl;
	os << "		 end " << endl;
	os << "	     else  " << endl;
	os << "    	 begin " << endl;
	os << " 		if(read_en)" << endl;
	os << " 			read_en <= 0;" << endl;
	os << "			else  " << endl;
	os << "			begin " << endl;
	os << "				if(en_mux) " << endl;
	os << "				begin " << endl;
	os << "					if(m_ready == 1) " << endl;
	os << "					begin " << endl;			
	os << "					case(sel)  " << endl;
	
		int j = 0;
	    for(i = 1; i < P; i++)
		{
			os << "					"<<	j << ": begin " << endl;
			os << "							m_valid <= 1;" << endl;	
			os << "					    	data_out <= data_out_" << i << " ; " << endl;
			os << "							sel <= sel + 1;" << endl;
			os << "						end" << endl;
			j++;
		}
			os << "					"<<	j << ": begin " << endl;	
			os << "      					m_valid <= 1;" << endl;
			os << "					    	data_out <= data_out_" << i << " ; " << endl;
			os << "					    	sel <= " << P << " ; " << endl;
			os << "						end" << endl;
			
	os << "                		default : begin " << endl;
	os << "									sel <= 0; " << endl;
	os << "									m_valid <= 0; " << endl;
	os << "									read_en <= 1; " << endl;
	os << "							   end " << endl;
	os << "					endcase " << endl;
	os << "				end " << endl;
	os << "			end " << endl;
	os << "		end " << endl;
	os << "	 end " << endl;		
	os << "end " << endl;
	os << "endmodule " << endl << endl;	

// the ready_control_ module for mapping multiple signals into one
	os << "module ready_control_" << N << "(clk, s_ready, " ;
		for(i = 1; i <= P; i++)
		{
			os << "s_ready_" << i << ", " ;
		}
    	for(i = 1; i <= P; i++)
		{
			os << "en_mux_" << i << ", " ;
		}
		for(i = 1; i <= P; i++)
		{
			os << "addr_f_" << i << ", " ;
		}
		 for(i = 1; i <= P; i++)
		{
			os << "wr_en_x_" << i << ", " ;
		}
		os << "en_mux, " ;
		os << "wr_en_x, ";
		os << "addr_f);" << endl ;
		os << " parameter ADDR_F_SIZE = 2;" << endl;

		for(i = 1; i <= P; i++)
		{
			os << "    input s_ready_" << i << ";" << endl;
			
		}
		for(i = 1; i <= P; i++)
		{
			os << "    input en_mux_" << i << ";" << endl;
			
		}
		for(i = 1; i <= P; i++)
		{
			os << "    input [ADDR_F_SIZE-1:0]addr_f_" << i << ";" << endl;
			
		}
		for(i = 1; i <= P; i++)
		{
			os << "    input wr_en_x_" << i << ";" << endl;
			
		}
		os << "    input clk;" << endl;
		os << "    output logic s_ready, en_mux, wr_en_x;" << endl ;
		os << "    output logic [ADDR_F_SIZE-1:0] addr_f;" << endl ;
		os << "    always_comb" << endl;
		os << "    begin " << endl;
		os << "        if(" ;
		for(i = 1; i <= P; i++)
		{
			if(i == P)
			os << "s_ready_" << i << ")" << endl;
			else
			os << "s_ready_" << i << " && " ;
		}
		os << "            s_ready = 1;" << endl;
		os << "        else " << endl;
		os << "            s_ready = 0;" << endl;
		
		os << "        if(" ;
		for(i = 1; i <= P; i++)
		{
			if(i == P)
			os << "en_mux_" << i << ")" << endl;
			else
			os << "en_mux_" << i << " && " ;
		}
		os << "            en_mux = 1;" << endl;
		os << "        else " << endl;
		os << "            en_mux = 0;" << endl;
		os << "        if(" ;
		for(i = 1; i <= P; i++)
		{
			if(i == P)
			os << "wr_en_x_" << i << ")" << endl;
			else
			os << "wr_en_x_" << i << " && " ;
		}
		os << "            wr_en_x = 1;" << endl;
		os << "        else " << endl;
		os << "            wr_en_x = 0;" << endl;
		os << "     addr_f = addr_f_1;" << endl;
		os << "    end" << endl;
		os << "endmodule" << endl << endl;


   // Check there are enough values in the constant file.
   if (M > constVector.size()) {
      cout << "ERROR: constVector does not contain enough data for the requested design" << endl;
      cout << "The design parameters requested require " << M << " numbers, but the provided data only have " << constVector.size() << " constants" << endl;
      assert(false);
   }

   // Generate a ROM for f with constants in constVector, T bits, and the given name
   string romModName = modName + "_f_rom";
   genROM(constVector, T, romModName, os);

}


// Part 3: Generate a hardware system with three layers interconnected.
// Layer 1: Input length: N, filter length: M1, output length: L1 = N-M1+1
// Layer 2: Input length: L1, filter length: M2, output length: L2 = L1-M2+1
// Layer 3: Input length: M2, filter length: M3, output length: L3 = L2-M3+1
// T is the number of bits
// A is the number of multipliers your overall design may use.
// Your goal is to build the highest-throughput design that uses A or fewer multipliers
// constVector holds all the constants for your system (all three layers, in order)
void genAllLayers(int N, int M1, int M2, int M3, int T, int A, vector<int>& constVector, string modName, ofstream &os) {

   // Here you will write code to figure out the best values to use for P1, P2, and P3, given
   // mult_budget.

   int P1, P2, P3, Q;
   int T1, T2, T3;
   int L1 = N-M1+1;
   int L2 = L1-M2+1;
   int L3 = L2-M3+1;
   int A1_count = 0; // A1_count is the number of factors stored the array A1. 
   int A2_count = 0; // A2_count is the number of factors stored the array A2.
   int A3_count = 0; // A3_count is the number of factors stored the array A3.

   int* A1 = factors(L1);  // calls the factor function to determine the factors of passed value  
   for(int i=0; i<(L1/2); i++)
	{
		if(A1[i] != 0)
		{
			A1_count++;
		}
	}

   int* A2 = factors(L2);
   for(int i=0; i<(L2/2); i++)
	{
		if(A2[i] != 0)
		{
			A2_count++;
		}
	}

   int* A3 = factors(L3);
   for(int i=0; i<(L3/2); i++)
	{
		if(A3[i] != 0)
		{
			A3_count++;
		}
	}
 	// the multiplier budget cannot be less than 3 as each of the convolution blocks must have a minimum parallelism of 1,
	// hence an error is thrown when A<3. 
	if(A<3)
	{
	   	cout << "  ERROR: minimum number of multipliers required is 3" << endl;
		return;
	}
	// when multiplier budget is 3, each of the convolution blocks get paralellism =1
 
	else if(A==3)
	{
   		 P1 = 1; 
   		 P2 = 1; 
  		 P3 = 1; 		
	}
	// when multiplier budget is more than 3, the algorithm below decides the optimal values for P1, P2 and P3
	// to obtain the best throughput for the three layer convolution system, while taking into consideration that
	// the values of P1, P2 & P3 can only be factors of L1, L2 & L3 respectively. 
	else 
	{
			int tp1, tp2, tp3;
			int R1 = (M1*L1); // R1, R2 and R3 are the M*L products which represent the number of cycles
			int R2 = (M2*L2); // taken by each convoluton block to generate all the outputs
			int R3 = (M3*L3);
			int i = 0;
			int j = 0;
			int k = 0;
			while(true)
			{
				tp1 = R1/A1[i];  // tp1, tp2 & tp3 store the throughput values of each combinational block
				tp2 = R2/A2[j];  // respectively, iterating through different parallelism values and is used 
				tp3 = R3/A3[k];  // to determine the optimal P values for best case throughput.
				if(tp1 > tp2 && tp1 > tp3)
				{
					i++;
					if((A1[i]+A2[j]+A3[k]) < A && i < A1_count)
					continue;
					else 
					{
						i--;
						break;
					}
				}
				else if(tp2 > tp1 && tp2 > tp3)
				{
					j++;
					if((A1[i]+A2[j]+A3[k]) < A && j < A2_count)
					continue;
					else 
					{
						j--;
						break;
					}
				}
				else
				{
					k++;
					if((A1[i]+A2[j]+A3[k]) < A && k < A3_count)
					continue;
					else 
					{
						k--;
						break;
					}
				} // end else
			} // end while
			P1 = A1[i];
			P2 = A2[j];
			P3 = A3[k];
			cout << "P1 = " << P1 << endl;
			cout << "P2 = " << P2 << endl;
			cout << "P3 = " << P3 << endl;

	}  // end else



   // output top-level module
   string subModName1 = "layer1_" + to_string(N) + "_" + to_string(M1) + "_" + to_string(T) + "_" + to_string(P1);
   string subModName2 = "layer2_" + to_string(L1) + "_" + to_string(M2) + "_" + to_string(T) + "_" + to_string(P2);
   string subModName3 = "layer3_" + to_string(L2) + "_" + to_string(M3) + "_" + to_string(T) + "_" + to_string(P3);

   os << "module " << modName << "(clk, reset, s_valid, m_ready, data_in, m_valid, s_ready, data_out);" << endl;
   os << "parameter T = " << T << ";" << endl ;
   os << "	  input clk, reset, s_valid, m_ready;" << endl;
   os << "    input signed [T-1:0] data_in;" << endl;
   os << "    output logic m_valid, s_ready;" << endl;
   os << "    output signed [T-1:0] data_out;" << endl ;
   os << "    logic m_valid1, m_valid2, m_ready1, m_ready2;" << endl;
   os << "    logic signed [T-1:0] data_out1, data_out2;" << endl;
	
   os << subModName1 << " layer1(.clk(clk), .reset(reset), .s_valid(s_valid), .m_ready(m_ready1), .data_in(data_in), .m_valid(m_valid1), .s_ready(s_ready), .data_out(data_out1));" << endl;
   os << subModName2 << " layer2(.clk(clk), .reset(reset), .s_valid(m_valid1), .m_ready(m_ready2), .data_in(data_out1), .m_valid(m_valid2), .s_ready(m_ready1), .data_out(data_out2));" << endl;
   os << subModName3 << " layer3(.clk(clk), .reset(reset), .s_valid(m_valid2), .m_ready(m_ready), .data_in(data_out2), .m_valid(m_valid), .s_ready(m_ready2), .data_out(data_out));" << endl;
   os << "endmodule" << endl;





   // -------------------------------------------------------------------------
   // Split up constVector for the three layers
   int start = 0;
   int stop = M1;
   vector<int> constVector1(&constVector[start], &constVector[stop]);

   // layer 2's W matrix is M2 x M1 and its B vector has size M2
   start = stop;
   stop = start+M2;
   vector<int> constVector2(&constVector[start], &constVector[stop]);

   // layer 3's W matrix is M3 x M2 and its B vector has size M3
   start = stop;
   stop = start+M3;
   vector<int> constVector3(&constVector[start], &constVector[stop]);

   if (stop > constVector.size()) {
      cout << "ERROR: constVector does not contain enough data for the requested design" << endl;
      cout << "The design parameters requested require " << stop << " numbers, but the provided data only have " << constVector.size() << " constants" << endl;
      assert(false);
   }
   // --------------------------------------------------------------------------


   // generate the three layer modules
   
   genLayer(N, M1, T, P1, constVector1, subModName1, os);

   genLayer(L1, M2, T, P2, constVector2, subModName2, os);

   genLayer(L2, M3, T, P3, constVector3, subModName3, os);
}

   // --------------------------------------------------------------------------

// this function returns the starting address of an array that stores the factors of the passed value
int *factors(int L) 
{					
 	 int *arrL = new int[(L/2)] ();
	int k = 0;
	for(int i=1; i<=L; i++)
	{
		if(L%i == 0)
		{
			arrL[k] = i;
			k++;
		}
	}
	
	return arrL;		 
}


void printUsage() {
   cout << "Usage: ./gen MODE ARGS" << endl << endl;

   cout << "   Mode 1: Produce one convolution module (Part 1 and Part 2)" << endl;
   cout << "      ./gen 1 N M T P const_file" << endl;
   cout << "      See project description for explanation of parameters." << endl;
   cout << "      Example: produce a convolution with input vector of length 16, filter of length 4, parallelism 1" << endl;
   cout << "               and 16 bit words, with constants stored in file const.txt" << endl;
   cout << "                   ./gen 1 16 4 16 1 const.txt" << endl << endl;

   cout << "   Mode 2: Produce a system with three interconnected convolution module (Part 3)" << endl;
   cout << "      Arguments: N, M1, M2, M3, T, A, const_file" << endl;
   cout << "      See project description for explanation of parameters." << endl;
   cout << "              e.g.: ./gen 2 16 4 5 6 15 16 const.txt" << endl << endl;
}
