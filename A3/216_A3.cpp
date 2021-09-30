#include <bits/stdc++.h>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;

struct LabelTable {
	string label;
	int32_t address;
};

struct MemoryElement {
	string label;
	int32_t value;
};

struct Simulator{

	string Registers[32]; // store register names
	int RegisterValues[32]; // register values
	int PC;  // program counter
	string InstructionSet[9]; // instructions allowed
	int numberOfInstructions;
	set<string>Labels;
	vector<string>inputProgram;	
	map<string,int> freqOfEachInstruction;
	string currentInstruction; 
	int r[3]; // to store register names, values etc for the instruction

	void add();
	void addi();
	void sub();
	void mul();
	void slt();
	void lw();
	void sw();
	void beq();
	void bne();
	void j();
	void convertIntegerToHexadecimal(int x);
	void printStatistics();

	void init(string infilename){
		Registers[0] = "zero";
		Registers[1] = "at";
		Registers[2] = "v0";
		Registers[3] = "v1";
		Registers[4] = "a0";
		Registers[5] = "a1";
		Registers[6] = "a2";
		Registers[7] = "a3";
		Registers[8] = "t0";
		Registers[9] = "t1";
		Registers[10] = "t2";
		Registers[11] = "t3";
		Registers[12] = "t4";
		Registers[13] = "t5";
		Registers[14] = "t6";
		Registers[15] = "t7";
		Registers[16] = "s0";
		Registers[17] = "s1";
		Registers[18] = "s2";
		Registers[19] = "s3";
		Registers[20] = "s4";
		Registers[21] = "s5";
		Registers[22] = "s6";
		Registers[23] = "s7";
		Registers[24] = "t8";
		Registers[25] = "t9";
		Registers[26] = "k0";
		Registers[27] = "k1";
		Registers[28] = "gp";
		Registers[29] = "sp";
		Registers[30] = "s8";
		Registers[31] = "ra";

		for(int i=0; i<32; i++){
			RegisterValues[i] = 0;
		}

		InstructionSet[0] = "add";
		InstructionSet[1] = "sub";
		InstructionSet[2] = "mul";
		InstructionSet[3] = "beq";
		InstructionSet[4] = "bne";
		InstructionSet[5] = "slt";
		InstructionSet[6] = "j";
		InstructionSet[7] = "lw";
		InstructionSet[8] = "sw";
		InstructionSet[9] = "addi";

		ifstream inputfile;
		inputfile.open(infilename.c_str(),ios::in);
		if(!inputfile){
			cout << "No file given";
			exit(1);
		}

		string x;
		while(getline(inputfile,x)){
			inputProgram.push_back(x);
		}
		numberOfInstructions = inputProgram.size();
		/*for(int i=0; i<10; i++){
			freqOfEachInstruction.insert({InstructionSet[i],0});
		}*/
	}
	//function to find out which instruction is to be executed and and populate values in r[]
	void parseInstruction(string currentInstruction){
		int i=0; int j=0;
		if(currentInstruction.find(":") == -1){
			Labels.insert(currentInstruction);
		} // label
		if(currentInstruction.size() < 4){
			cout << "Parse Error" << " " << currentInstruction << '\n';
			exit(1);
		}
		while(j < 4){
			if(j == ' ' || j == '\t') break;
			j++;
		}
		string op = currentInstruction.substr(0,j);
		op = RemoveSpaces(op);
		if(op == "add" || op == "sub" || op == "mul" || op == "slt"){
			string currInstr = RemoveSpaces(currentInstruction);
			int len = currInstr.size()-1;
			if(currInstr[3] != '$'){
				cout << "0!" << '\n';
				cout << "Parse Error" << '\n';
				exit(1);
			} 
			int k = findPosOfFirstComma(currInstr);
			string rd = currInstr.substr(5,k-1);
			if(currInstr[k+1] != '$'){
				cout << "1!" << '\n';
				cout << "Parse Error" << '\n';
				exit(1);
			}
			int k1 = findPosOfFirstComma(currInstr);
			string rs1 = currInstr.substr(k+2,k1-1);
			if(currInstr[k1+1] != '$'){
				cout << "2!" << '\n';
				cout << rs1 << '\n';
				cout << "Parse Error" << '\n';
				exit(1);
			}
			string rs2 = currInstr.substr(k1+2,len);
			if(!findMyRegister(rs1) || !findMyRegister(rs2) || !findMyRegister(rd)){
				cout << "3!" << '\n';
				cout << "rd " << rd << '\n';
				cout << "rs1 " << rs1 << '\n';
				cout << "rs2 " << rs2 << '\n';
				cout << "Parse Error" << '\n';
				exit(1);
			}
		}else if(op == "beq" || op == "bne"){
			string currInstr = RemoveSpaces(currentInstruction);
			int len = currInstr.size()-1;
			if(currInstr[3] != '$'){
				cout << "Parse Error" << '\n';
				exit(1);
			}
			int k = findPosOfFirstComma(currInstr);
			string rs1 = currInstr.substr(5,k-1);
			if(currInstr[k+1] != '$'){
				cout << "Parse Error" << '\n';
				exit(1);
			}
			int k1 = findPosOfFirstComma(currInstr.substr(k+1,len));
			string rs2 = currInstr.substr(k+2,k1-1);
			if(currInstr[k1+1] != '$'){
				cout << "Parse Error" << '\n';
				exit(1);
			}
			string label = currInstr.substr(k1+2,len);
			if(!findMyRegister(rs1) || !findMyRegister(rs2) || !findMyLabel(label)){
				cout << "Parse Error" << '\n';
				exit(1);
			}
		}else if(op == "j"){
			string currInstr = RemoveSpaces(currentInstruction);
			int len = currInstr.size();
			string label = currInstr.substr(1,len);
			if(!findMyLabel(label)){
				cout << "Parse Error " << label << " not found" << '\n';
				exit(1);
			}
		}else if(op == "lw"){
			string currInstr = RemoveSpaces(currentInstruction);
			int len = currInstr.size() - 1;
			if(currInstr[2] != '$'){
				cout << "Parse Error" << '\n';
				exit(1);
			}
			int k = findPosOfFirstComma(currInstr);
			string rd = currInstr.substr(0,k-1);
			string memoryAddress = currInstr.substr(k+1,len);
		}else if(op == "sw"){
			string currInstr = RemoveSpaces(currentInstruction);
			int len = currInstr.size() - 1;
			int k = findPosOfFirstComma(currInstr);
			string memoryAddress = currInstr.substr(2,k-1);
			string rd = currInstr.substr(k+1,len);
			if(currInstr[k+1] != '$'){
				cout << "Parse Error" << '\n';
				exit(1);
			}
		}else if(op == "addi"){
			string currInstr = RemoveSpaces(currentInstruction);
			int len = currInstr.size()-1;
			if(currInstr[4] != '$'){
				cout << "Parse Error" << '\n';
				exit(1);
			}
			int k = findPosOfFirstComma(currInstr);
			string rd = currInstr.substr(5,k-1);
			if(currInstr[k+1] != '$'){
				cout << "Parse Error" << '\n';
				exit(1);
			}
			int k1 = findPosOfFirstComma(currInstr.substr(k+1,len));
			string rs = currInstr.substr(k+2,k1-1);
			if(currInstr[k1+1] != '$'){
				cout << "Parse Error" << '\n';
				exit(1);
			}
			string value = currInstr.substr(k1+2,len);
			int val = stoi(value);
			if(!findMyRegister(rd) || !findMyRegister(rs)){
				cout << "Parse Error" << '\n';
				exit(1);
			}
		}else{
			cout << "Parse Error:" << ' ' << "Invalid instructions " << op;
			exit(1);
		}
	}

	bool findMyRegister(string x){
		bool found = false;
		for(int i=0; i<32; i++){
			if(x == Registers[i]) found = true;
		}
		if(ReservedRegisters(x)) found = false;
		return found;
	}
	bool ReservedRegisters(string x){
		if(x == "at" || x == "k1" || x == "k2") return true;
		else return false;
	}
	bool findMyLabel(string x){
		if(Labels.find(x) == Labels.end()) return false;
		else return true;
	}
	int findPosOfFirstComma(string x){
		for(int i=0; i<x.size(); i++){
			if(x[i] == ',') return i;
		}
		return INT_MAX;
	}
	string RemoveSpaces(string x){
		string t = "";
		for(int i=0; i<x.size(); i++){
			if(x[i] != ' '){
				t += x[i];
			}
		}
		return t;
	}
	int getValueFromRegister(string x){
		for(int i=0; i<32; i++){
			if(Registers[i] == x){
				return RegisterValues[i];
			}
		}
		return INT_MAX;
	}
	void LoadValueIntoRegister(string s,int x){
		for(int i=0; i<32; i++){
			if(Registers[i] == s){
				RegisterValues[i] = x;
			}
		}
	}
	int giveIndexOfMyRegister(string x){
		for(int i=0; i<32; i++){
			if(Registers[i] == x){
				return i;
			}
		}
		return INT_MAX;
	}
	int findPosOfSecondComma(string x){
		int pos = 0;
		for(int i=0; i<32; i++){
			if(x[i] == ','){
				pos++;
				if(pos == 2) return i;
			}
		}
		return INT_MAX;
	}
};

int main(){
	Simulator smltr;
	smltr.init("infile.txt");
	vector<string> ip = smltr.inputProgram;
	for(auto c : smltr.inputProgram) cout << c << '\n';
	for(int i=0; i<smltr.inputProgram.size(); i++){
		smltr.parseInstruction(smltr.inputProgram[i]);
	}
}