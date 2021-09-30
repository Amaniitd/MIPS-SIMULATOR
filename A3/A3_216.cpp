#include <bits/stdc++.h>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;

string Registers[32];	   // store register names
int RegisterValues[32];	   // register values
string InstructionSet[10]; // instructions allowed
int counter = 0;
map<string, int> Labels; //to store all the branches/labels
vector<string> inputProgram;
map<string, int> freqOfEachInstruction;
map<string, int> Memory;
string currentInstruction;
int r[3];
string address;
string currReg = "";

bool checkSize(int x)
{
	if (currReg == "sp")
	{
		if (x < 0 || x > 4194304)
		{
			return false;
		}
		return true;
	}
	return x > -2147483648 || x < 2147483647;
}
bool findMyRegister(string x)
{
	for (int i = 0; i < 32; i++)
	{
		if (x == Registers[i])
		{
			return true;
		}
	}
	return false;
}
bool ReservedRegisters(string x)
{
	if (x == "at" || x == "k1" || x == "k2" || x == "zero")
		return true;
	else
		return false;
}
bool findMyLabel(string x)
{
	if (Labels.find(x) == Labels.end())
		return false;
	else
		return true;
}
int findPosOfFirstComma(string x)
{
	for (int i = 0; i < x.size(); i++)
	{
		if (x[i] == ',')
			return i;
	}
	return INT_MAX;
}
string RemoveSpaces(string x)
{
	string t = "";
	for (int i = 0; i < x.size(); i++)
	{
		if (x[i] != ' ')
		{
			t += x[i];
		}
	}
	return t;
}
int getValueFromRegister(string x)
{
	for (int i = 0; i < 32; i++)
	{
		if (Registers[i] == x)
		{
			return RegisterValues[i];
		}
	}
	return INT_MAX;
}
void LoadValueIntoRegister(string s, int x)
{
	for (int i = 0; i < 32; i++)
	{
		if (Registers[i] == s)
		{
			RegisterValues[i] = x;
		}
	}
}
int giveIndexOfMyRegister(string x)
{
	for (int i = 0; i < 32; i++)
	{
		if (Registers[i] == x)
		{
			return i;
		}
	}
	return INT_MAX;
}

void add()
{
	int temp = RegisterValues[r[1]] + RegisterValues[r[2]];
	if (checkSize(temp))
	{
		RegisterValues[r[0]] = temp;
	}
	else
	{
		cout << "Out of bound " << counter << endl;
		exit(1);
	}
}

void addi()
{
	int temp = RegisterValues[r[1]] + r[2];
	if (checkSize(temp))
	{
		RegisterValues[r[0]] = temp;
	}
	else
	{
		cout << "Out of bound " << counter << endl;
		exit(1);
	}
}

void sub()
{
	int temp = RegisterValues[r[1]] - RegisterValues[r[2]];
	if (checkSize(temp))
	{
		RegisterValues[r[0]] = temp;
	}
	else
	{
		cout << "Out of bound " << counter << endl;
		exit(1);
	}
}

void mul()
{
	int temp = RegisterValues[r[1]] * RegisterValues[r[2]];
	if (checkSize(temp))
	{
		RegisterValues[r[0]] = temp;
	}
	else
	{
		cout << "Out of bound " << counter << endl;
		exit(1);
	}
}

void slt()
{
	RegisterValues[r[0]] = RegisterValues[r[1]] < RegisterValues[r[2]];
}

void lw()
{
	RegisterValues[r[0]] = Memory[address];
}

void sw()
{
	Memory[address] = RegisterValues[r[0]];
}

void beq()
{
	if (RegisterValues[r[0]] == RegisterValues[r[1]])
	{
		counter = r[2];
	}
}

void bne()
{
	if (RegisterValues[r[0]] != RegisterValues[r[1]])
	{
		counter = r[2];
	}
}

void j()
{
	counter = r[0];
}

void decToHexa(int n)
{
	char hexaDeciNum[100];
	int i = 0;
	while (n != 0)
	{
		int temp = 0;
		temp = n % 16;
		if (temp < 10)
		{
			hexaDeciNum[i] = temp + 48;
			i++;
		}
		else
		{
			hexaDeciNum[i] = temp + 55;
			i++;
		}

		n = n / 16;
	}
	for (int j = i - 1; j >= 0; j--)
		cout << hexaDeciNum[j];
}

void init(string infilename)
{
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

	for (int i = 0; i < 32; i++)
	{
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
	inputfile.open(infilename.c_str(), ios::in);
	if (!inputfile)
	{
		cout << "No file given or not able to open file";
		exit(1);
	}

	string x;
	while (getline(inputfile, x))
	{
		int tmp = 0;
		while (true)
		{
			if (x[tmp] == ' ' || x[tmp] == '\t')
			{
				tmp++;
			}
			else
				break;
		}
		x = x.substr(tmp);
		if (x != "" && x[0] != '#')
		{
			inputProgram.push_back(x);
		}
	}
}

void parseInstruction(string currentInstruction)
{

	int tmp = 0;
	while (true)
	{
		if (currentInstruction[tmp] == ' ' || currentInstruction[tmp] == '\t')
		{
			tmp++;
		}
		else
			break;
	}
	currentInstruction = currentInstruction.substr(tmp);
	if (currentInstruction.find("#") != -1)
	{
		currentInstruction = currentInstruction.substr(0, currentInstruction.find("#"));
	}

	if (currentInstruction == "")
	{
		counter++;
		return;
	}

	int pos = currentInstruction.find(':');
	if (pos != -1 && currentInstruction.length() > pos + 1)
	{
		currentInstruction = currentInstruction.substr(pos + 1);
	}
	else if (pos != -1)
	{
		counter++;
		return;
	}
	int s = 0;
	while (s < 4 && currentInstruction.length() > s + 1)
	{
		if (currentInstruction[s] == ' ' || currentInstruction[s] == '\t')
			break;
		s++;
	}
	string op = currentInstruction.substr(0, s);
	op = RemoveSpaces(op);
	if (op == "add" || op == "sub" || op == "mul" || op == "slt")
	{
		string currInstr = RemoveSpaces(currentInstruction);
		if (currInstr[3] != '$')
		{
			cout << "Error at Line(after removing comments and empty lines)   : " << counter << endl;
			exit(1);
		}
		int k = findPosOfFirstComma(currInstr);
		string rd = currInstr.substr(4, 2);
		if (currInstr[k + 1] != '$')
		{
			cout << "Error at Line(after removing comments and empty lines)   : " << counter << endl;
			exit(1);
		}
		int k1 = findPosOfFirstComma(currInstr.substr(k + 1)) + k + 1;
		string rs1 = currInstr.substr(k + 2, k1 - 2 - k);
		if (currInstr[k1 + 1] != '$')
		{
			cout << "Error at Line(after removing comments and empty lines)   : " << counter << endl;
			exit(1);
		}
		string rs2 = currInstr.substr(k1 + 2);
		if (!findMyRegister(rs1) || !findMyRegister(rs2) || !findMyRegister(rd) || !ReservedRegisters(rd))
		{
			cout << "Error at Line(after removing comments and empty lines)   : " << counter << endl;
			exit(1);
		}
		r[0] = giveIndexOfMyRegister(rd);
		r[1] = giveIndexOfMyRegister(rs1);
		r[2] = giveIndexOfMyRegister(rs2);
		currReg = op;
		if (op == "add")
			add();
		else if (op == "sub")
			sub();
		else if (op == "mul")
			mul();
		else
			slt();
	}

	else if (op == "beq" || op == "bne")
	{
		string currInstr = RemoveSpaces(currentInstruction);
		int len = currInstr.size() - 1;
		if (currInstr[3] != '$')
		{
			cout << "Error at Line(after removing comments and empty lines)   : " << counter << endl;
			exit(1);
		}
		int k = findPosOfFirstComma(currInstr);
		string rs1 = currInstr.substr(4, k - 4);
		if (currInstr[k + 1] != '$')
		{
			cout << "Error at Line(after removing comments and empty lines)   : " << counter << endl;
			exit(1);
		}
		int k1 = findPosOfFirstComma(currInstr.substr(k + 1, len)) + k + 1;
		string rs2 = currInstr.substr(k + 2, k1 - 2 - k);

		string label = currInstr.substr(k1 + 1);
		if (!findMyRegister(rs1) || !findMyRegister(rs2) || !findMyLabel(label))
		{
			cout << "Error at Line(after removing comments and empty lines)   : " << counter << endl;
			exit(1);
		}
		r[0] = giveIndexOfMyRegister(rs1);
		r[1] = giveIndexOfMyRegister(rs2);
		r[2] = Labels[label];
		if (op == "beq")
			beq();
		else
			bne();
	}

	else if (op == "j")
	{
		string currInstr = RemoveSpaces(currentInstruction);
		string label = currInstr.substr(1);
		if (!findMyLabel(label) || Labels[label] == 0)
		{
			cout << "Error " << label << " not found" << endl;
			exit(1);
		}
		r[0] = Labels[label];
		j();
	}

	else if (op == "lw" || op == "sw")
	{
		string currInstr = RemoveSpaces(currentInstruction);
		if (currInstr[2] != '$')
		{
			cout << "Error at Line(after removing comments and empty lines)  : " << counter << endl;
			exit(1);
		}
		int k = findPosOfFirstComma(currInstr);
		if (k > 5)
		{
			cout << "Error at Line(after removing comments and empty lines)  : " << counter << endl;
			exit(1);
		}
		string rd = currInstr.substr(3, 2);
		string memoryAddress = currInstr.substr(6);
		if (!findMyRegister(rd))
		{
			cout << "Error at Line(after removing comments and empty lines)  : " << counter << endl;
			exit(1);
		}
		r[0] = giveIndexOfMyRegister(rd);
		address = memoryAddress;
		if (op == "lw")
			lw();
		else
			sw();
	}

	else if (op == "addi")
	{
		string currInstr = RemoveSpaces(currentInstruction);
		if (currInstr[4] != '$')
		{
			cout << "Error at Line(after removing comments and empty lines)  : " << counter << endl;
			exit(1);
		}
		int k = findPosOfFirstComma(currInstr);
		string rd = currInstr.substr(5, k - 5);
		if (currInstr[k + 1] != '$')
		{
			cout << "Error at Line(after removing comments and empty lines)  : " << counter << endl;
			exit(1);
		}
		int k1 = findPosOfFirstComma(currInstr.substr(k + 1)) + k + 1;
		string rs = currInstr.substr(k + 2, k1 - 2 - k);
		string value = currInstr.substr(k1 + 1);
		if (value == "")
		{
			cout << "Error at Line(after removing comments and empty lines)  : " << counter << endl;
			exit(1);
		}
		int val = stoi(value);
		if (!findMyRegister(rd) || !findMyRegister(rs) || ReservedRegisters(rd))
		{
			cout << "Error at Line(after removing comments and empty lines)  : " << counter << endl;
			exit(1);
		}
		r[0] = giveIndexOfMyRegister(rd);
		r[1] = giveIndexOfMyRegister(rs);
		r[2] = val;
		addi();
	}

	else
	{
		cout << "Error:" << ' ' << "Invalid instructions " << op;
		exit(1);
	}
	counter++;
}

int main()
{
	init("infile.txt");
	vector<string> ip = inputProgram;

	for (int i = 1; i < inputProgram.size(); i++)
	{
		if (inputProgram[i].find(':') != -1)
		{
			string tempLabel = inputProgram[i].substr(0, inputProgram[i].find(":"));
			if (Labels[tempLabel] != 0)
			{
				cout << "Multiple branch with same name " << endl;
				cout << tempLabel << endl;
				exit(1);
			}
			Labels[tempLabel] = i;
		}
	}

	int clock = 0;
	while (counter < inputProgram.size())
	{
		clock++;
		cout << "position: " << counter << endl;
		cout << inputProgram[counter] << endl;
		freqOfEachInstruction[inputProgram[counter]]++;
		parseInstruction(inputProgram[counter]);
		cout << endl;

		for (int a = 0; a < 16; a++)
		{
			cout << Registers[a]
				 << "\t";
		}
		cout << endl;
		cout << " ";
		for (int a = 0; a < 16; a++)
		{
			decToHexa(RegisterValues[a]);
			cout << "\t";
		}
		cout << endl;
		for (int a = 16; a < 32; a++)
		{
			cout << Registers[a]
				 << "\t";
		}
		cout << endl;
		cout << " ";
		for (int a = 16; a < 32; a++)
		{
			decToHexa(RegisterValues[a]);
			cout << "\t";
		}
		cout << endl;
		cout << endl;
	}
	cout << "Total clock cycles: " << clock << endl;
	cout << "----------------------------------------------------" << endl;
	for (int i = 1; i < inputProgram.size(); i++)
	{
		cout << "freq of "
			 << "\t" << inputProgram[i] << "\t"
			 << "is:"
			 << "\t" << freqOfEachInstruction[inputProgram[i]] << endl;
	}
}