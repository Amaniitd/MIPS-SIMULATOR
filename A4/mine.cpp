#include <bits/stdc++.h>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;

string Registers[32];   // store register names
int RegisterValues[32]; // register values
int counter = 0;
map<string, int> Labels; //to store all the branches/labels
vector<string> inputProgram;
map<string, int> freqOfEachInstruction;
string currentInstruction;
int r[3];
int StackMemory[262114];
int clockTrash = 1;
int rowBuffer = 0;
bool activated = false;
const int rowAccessDelay = 10;
const int columnAccessDelay = 2;
int clockCycle = 1;
int registerInUse[32];
int cache[4 * rowAccessDelay + 2 * columnAccessDelay - 1];
string changedRegister[4 * rowAccessDelay + 2 * columnAccessDelay - 1];
int changedValue[4 * rowAccessDelay + 2 * columnAccessDelay - 1];
bool isEmpty = true;
int maxCache = 0;
int mid = 2 * rowAccessDelay + columnAccessDelay - 1;
int leftStart = 0;
int rightStart = mid + 1;
int tempValue = 0;
string tempString = "";
int rowBufferUpdates = 0;

void printCache()
{
    int randomName = 0;
    for (int i = 0; i < mid; i++)
    {
        if (cache[i] != -1)
        {
            cout << "Clock cycle no. : " << clockCycle << endl;
            cout << inputProgram[cache[i]] << "    " << changedRegister[i] << " -> " << changedValue[i] << endl;
            clockCycle++;
            randomName++;
        }
        else if (randomName < maxCache)
        {
            clockCycle++;
            randomName++;
        }
    }
    cout << "Clock cycle no. : " << clockCycle << endl;
    cout << inputProgram[cache[mid]] << "    " << changedRegister[mid] << " -> " << changedValue[mid] << endl;
    clockCycle++;
    for (int i = mid + 1; i < 2 * mid + 1; i++)
    {
        if (cache[i] != -1)
        {
            cout << "Clock cycle no. : " << clockCycle << endl;
            cout << inputProgram[cache[i]] << "    " << changedRegister[i] << " -> " << changedValue[i] << endl;
            clockCycle++;
        }
        else
        {
            break;
        }
    }
}

void addInCache()
{
    if (isEmpty)
    {
        cout << "Clock cycle no. :" << clockCycle << endl;
        cout << inputProgram[counter] << "     " << Registers[r[0]] << " -> " << tempValue << endl;
        clockCycle++;
    }
    else if (maxCache == leftStart || maxCache == rightStart - mid - 1)
    {
        printCache();
        isEmpty = true;
        cout << "Clock cycle no. :" << clockCycle << endl;
        cout << inputProgram[counter] << "     " << Registers[r[0]] << " -> " << tempValue << endl;
        clockCycle++;
    }
    else if (registerInUse[r[1]] < clockCycle)
    {
        cache[leftStart] = counter;
        changedRegister[leftStart] = Registers[r[0]];
        changedValue[leftStart] = tempValue;
        leftStart++;
    }
    else
    {
        cache[rightStart] = counter;
        changedRegister[rightStart] = Registers[r[0]];
        changedValue[rightStart] = tempValue;
        rightStart++;
        registerInUse[r[1]] = clockCycle;
    }
}

void initializeCache()
{
    for (int i = 0; i < mid; i++)
    {
        cache[i] = -1;
    }
    for (int i = mid + 1; i < 2 * mid + 1; i++)
    {
        cache[i] = -1;
    }
}

bool checkSize(int x)
{
    return x > -2147483648 && x < 2147483647;
}
bool validAddress(int x)
{

    if (x < 0 || x >= 1048576 || x % 4 != 0)
    {
        return false;
    }
    return true;
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
        if (x[i] != ' ' && x[i] != '\t')
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
        tempValue = temp;
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
        tempValue = temp;
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
        tempValue = temp;
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
        tempValue = temp;
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
    tempValue = RegisterValues[r[1]] < RegisterValues[r[2]];
}

void lw()
{
    if (validAddress(r[1]))
    {
        RegisterValues[r[0]] = StackMemory[r[1] / 4];

        if (isEmpty)
        {
            cache[mid] = counter;
            initializeCache();
            isEmpty = false;
        }
        else
        {
            printCache();
            cache[mid] = counter;
            initializeCache();
        }
        tempString = Registers[r[0]];
        tempValue = StackMemory[r[1] / 4];
        changedRegister[mid] = tempString;
        changedValue[mid] = RegisterValues[r[0]];
        cout << "Clock cycle no. : " << clockCycle << endl;
        cout << "DRAM request issued" << endl;
        clockCycle++;
        if (activated)
        {
            if (rowBuffer != r[1] / 1024)
            {
                rowBufferUpdates++;
                clockTrash = 2 * rowAccessDelay + 2;
                maxCache = mid;
                rowBuffer = r[1] / 1024;
                cout << "Copying the row number " << r[1] / 1024 << " in Row Buffer" << endl;
            }
            else
            {

                maxCache = columnAccessDelay - 1;
            }
        }
        else
        {
            rowBufferUpdates++;
            clockTrash = rowAccessDelay + 2;
            maxCache = mid - rowAccessDelay;
            activated = true;
            rowBuffer = r[1] / 1024;
            cout << "Copying the row number " << r[1] / 1024 << " in Row Buffer" << endl;
        }
        clockTrash = clockTrash + columnAccessDelay - 1;
        registerInUse[r[0]] = clockCycle + clockTrash - 2;
        leftStart = 0;
        rightStart = mid + 1;
    }
    else
    {
        cout << "Invalid address! " << r[1] << endl;
    }
}

void sw()
{
    if (validAddress(r[1]))
    {
        StackMemory[r[1] / 4] = RegisterValues[r[0]];
        if (isEmpty)
        {
            cache[mid] = counter;
            initializeCache();
            isEmpty = false;
        }
        else
        {
            printCache();
            cache[mid] = counter;
            initializeCache();
        }
        tempString = "memory address ";
        std::string s = std::to_string(r[1] + 3);
        std::string s2 = std::to_string(r[1]);
        tempString += s2 + "-" + s;
        changedRegister[mid] = tempString;
        changedValue[mid] = RegisterValues[r[0]];
        cout << "Clock cycle no. : " << clockCycle << endl;
        cout << "DRAM request issued" << endl;
        clockCycle++;
        if (activated)
        {
            if (rowBuffer != r[1] / 1024)
            {
                rowBufferUpdates++;
                clockTrash = 2 * rowAccessDelay + 2;
                maxCache = mid;
                rowBuffer = r[1] / 1024;
                cout << "Copying the row number " << r[1] / 1024 << " in Row Buffer" << endl;
            }
            else
            {
                maxCache = columnAccessDelay - 1;
            }
        }
        else
        {
            rowBufferUpdates++;
            clockTrash = rowAccessDelay + 2;
            maxCache = mid - rowAccessDelay;
            activated = true;
            rowBuffer = r[1] / 1024;
            cout << "Copying the row number " << r[1] / 1024 << " in Row Buffer" << endl;
        }
        rowBufferUpdates++;
        clockTrash = clockTrash + columnAccessDelay - 1;
        registerInUse[r[0]] = clockCycle + clockTrash - 2;
        leftStart = 0;
        rightStart = mid + 1;
    }
    else
    {
        cout << "Invalid address! " << r[1] << endl;
    }
}

void beq()
{
    if (RegisterValues[r[0]] == RegisterValues[r[1]])
    {
        counter = r[2] - 1;
    }
}

void bne()
{
    if (RegisterValues[r[0]] != RegisterValues[r[1]])
    {
        counter = r[2] - 1;
    }
}

void j()
{
    counter = r[0] - 1;
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

    ifstream inputfile;
    inputfile.open(infilename.c_str(), ios::in);
    if (!inputfile)
    {
        cout << "No file given or not able to open file";
        exit(1);
    }

    string x;
    int i2;
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
        if (x.find(':') != -1)
        {
            string tempLabel = x.substr(0, x.find(":"));
            if (Labels[tempLabel] != 0)
            {
                cout << "Multiple branch with same name " << endl;
                cout << tempLabel << endl;
                exit(1);
            }
            Labels[tempLabel] = i2;
        }
        else if (x != "" && x[0] != '#')
        {
            if (x.find('#') != -1)
            {
                x = x.substr(0, x.find('#'));
            }
            i2++;
            inputProgram.push_back(x);
        }
    }
}

void parseInstruction(string currentInstruction)
{
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
        if (!findMyRegister(rs1) || !findMyRegister(rs2) || !findMyRegister(rd) || ReservedRegisters(rd))
        {
            cout << "Error at Line(after removing comments and empty lines)   : " << counter << endl;
            exit(1);
        }
        r[0] = giveIndexOfMyRegister(rd);
        r[1] = giveIndexOfMyRegister(rs1);
        r[2] = giveIndexOfMyRegister(rs2);
        if (op == "add")
            add();
        else if (op == "sub")
            sub();
        else if (op == "mul")
            mul();
        else
            slt();
        addInCache();
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
        if (memoryAddress == "")
        {
            cout << "Error at Line(after removing comments and empty lines)  : " << counter << endl;
            exit(1);
        }
        int val = stoi(memoryAddress);
        r[1] = val;
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
        addInCache();
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
    init("minor_part1/testcase2.txt");
    while (counter < inputProgram.size())
    {
        clockTrash = 1;
        // freqOfEachInstruction[inputProgram[counter]]++;
        parseInstruction(inputProgram[counter]);

        // -----------------------------------------------------------------//
        //     cout << endl;
        //     for (int a = 0; a < 16; a++)
        //     {
        //         cout << Registers[a]
        //              << "\t";
        //     }
        //     cout << endl;
        //     cout << " ";
        //     for (int a = 0; a < 16; a++)
        //     {
        //         decToHexa(RegisterValues[a]);
        //         cout << "\t";
        //     }
        //     cout << endl;
        //     for (int a = 0; a < 16; a++)
        //     {
        //         cout << registerInUse[a];
        //         cout << "\t";
        //     }
        //     cout << endl;
        //     for (int a = 16; a < 32; a++)
        //     {
        //         cout << Registers[a]
        //              << "\t";
        //     }
        //     cout << endl;
        //     cout << " ";
        //     for (int a = 16; a < 32; a++)
        //     {
        //         decToHexa(RegisterValues[a]);
        //         cout << "\t";
        //     }
        //     cout << endl;
        //     for (int a = 16; a < 32; a++)
        //     {
        //         cout << registerInUse[a];
        //         cout << "\t";
        //     }
        //     cout << endl;
        //     cout << endl;
        // }
        // cout << "Total clock cycles: " << clock + clockTrash << endl;
        // cout << "----------------------------------------------------" << endl;
        // for (int i = 0; i < inputProgram.size(); i++)
        // {
        // 	cout << "freq of "
        // 		 << "\t" << inputProgram[i] << "\t"
        // 		 << "is:"
        // 		 << "\t" << freqOfEachInstruction[inputProgram[i]] << endl;
    }
    printCache();
    cout << endl;
    cout << "Total clock cycles: " << clockCycle - 1 << endl;
    cout << "Number of row buffer updates: " << rowBufferUpdates << endl;
}
