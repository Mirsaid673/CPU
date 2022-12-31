#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <bitset>

#include "decoder.h"

int errors_count = 0;

void handle_ret_code(ReturnCode ret_code, int code_line, const std::string &cmd, const std::vector<std::string> &args);
std::string tolower(const std::string &str);
std::string trim(const std::string &str, char whitespace = ' ');
void print(const std::string &cmd, const std::vector<std::string> &args);
bool can_be_label(const std::string &str);
std::string get_lable(const std::string &str);
std::vector<uint16_t> decoded_instructions;

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        std::cout << "ERROR: invalid arguments count" << std::endl;
        return -1;
    }
    std::string o_file_name = "binary.out";
    std::string i_file_name;
    for (int i = 1; i < argc; i++)
    {
        if (std::string(argv[i]) == "-o")
        {
            if (++i == argc)
            {
                std::cout << "Error: no option after \"-o\"" << std::endl;
                return -1;
            }
            o_file_name = argv[i];
        }
        else
        {
            i_file_name = argv[i];
        }
    }
    if (i_file_name.empty())
    {
        std::cout << "Error: file name cannot be empty" << std::endl;
        return -1;
    }

    std::ifstream input_file(i_file_name);
    if (!input_file.is_open())
    {
        std::cout << "ERROR: cannot open file: " << i_file_name << std::endl;
        return -1;
    }
    std::ofstream output_file(o_file_name, std::ios::binary);
    if (!output_file.is_open())
    {
        std::cout << "ERROR: cannot open file: " << o_file_name << std::endl;
        return -1;
    }

    int exit_code = 0;
    int line_num = 0;
    while (!input_file.eof())
    {
        line_num++;
        std::string line; // args
        std::getline(input_file, line);

        std::stringstream sstr(line);
        std::getline(sstr, line, ';'); // deleteing all after ';' (comment symbol)
        line = trim(line);

        if (can_be_label(line))
        {
            std::string lable = trim(get_lable(line));
            if (!is_word(lable))
            {
                errors_count++;
                std::cout << "(" << errors_count << ")=====Error: invalid name for label on line: " << line_num << std::endl
                          << line << std::endl;
            }
            else
            {
                uint32_t addr = instruction_addr + 1;
                labels.emplace(lable, addr);
                for (auto comand = not_resolved_comands.begin(); comand != not_resolved_comands.end(); comand++)
                {
                    if(comand->lable == lable)
                    {
                        comand->instruction.setLimm(addr - comand->index);
                        decoded_instructions[comand->index] = comand->instruction.getInstr();

                        comand = not_resolved_comands.erase(comand);
                    }
                }
            }
            continue;
        }
        sstr.clear();
        sstr.str(line);

        std::string cmd;
        sstr >> cmd; // firs word is comand
        if (cmd.empty())
            continue;
        cmd = tolower(cmd);

        std::vector<std::string> args; // other words are arguments

        std::string tmp;
        while (getline(sstr, tmp, ','))
        {
            args.push_back(tolower(trim(tmp)));
        }

        uint16_t res;
        ReturnCode ret_code = decode(cmd, args, res);

        if (ret_code)
        {
            handle_ret_code(ret_code, line_num, cmd, args);
            exit_code = -1;
            continue;
        }

        decoded_instructions.push_back(res);
        /*print(cmd, args);
        std::cout << std::bitset<16>(res) << std::endl; */
    }
    if (!not_resolved_comands.empty())
    {
        exit_code = -1;
        for (const auto comand : not_resolved_comands)
        {
            errors_count++;
            std::cout << "(" << errors_count << ")===== Error: invalid label name: " << comand.lable << std::endl;
        }
    }
    for (uint16_t instr : decoded_instructions)
        output_file << std::hex << instr << std::endl;

    return exit_code;
}

std::string get_lable(const std::string &str)
{
    std::string lable = str;

    lable.erase(lable.size() - 1); // erasing that colon
    return lable;
}

bool can_be_label(const std::string &str)
{
    if (str.empty())
        return false;
    return str.back() == ':';
}

std::string trim(const std::string &str, char whitespace)
{
    const auto begin = str.find_first_not_of(whitespace);
    if (begin == std::string::npos)
        return "";

    const auto end = str.find_last_not_of(whitespace);
    const auto range = end - begin + 1;
    return str.substr(begin, range);
}

std::string tolower(const std::string &str)
{
    std::string tmp;
    tmp.resize(str.size());
    std::transform(str.begin(), str.end(), tmp.begin(), [](unsigned char c)
                   { return std::tolower(c); });
    return tmp;
}

void print(const std::string &cmd, const std::vector<std::string> &args)
{
    std::cout << cmd << ' ';
    if (args.empty())
        return;

    std::cout << args[0];
    for (int i = 1; i < args.size(); i++)
        std::cout << ", " << args[i];
    std::cout << std::endl;
}

void handle_ret_code(ReturnCode ret_code, int code_line, const std::string &cmd, const std::vector<std::string> &args)
{
    errors_count++;
    switch (ret_code)
    {
    case ReturnCode::INVALID_COMAND:
        std::cout << "(" << errors_count << ")===== Error: invalid comand on line: " << code_line << std::endl;
        break;

    case ReturnCode::INVALID_ARGUMENT_COUNT:
        std::cout << "(" << errors_count << ")===== Error: invalid argument count on line: " << code_line << std::endl;
        break;

    case ReturnCode::INVALID_ARGUMENT1:
        std::cout << "(" << errors_count << ")===== Error: invalid argument 1 on line: " << code_line << std::endl;
        break;

    case ReturnCode::INVALID_ARGUMENT2:
        std::cout << "(" << errors_count << ")===== Error: invalid argument 2 on line: " << code_line << std::endl;
        break;

    case ReturnCode::INVALID_ARGUMENT3:
        std::cout << "(" << errors_count << ")===== Error: invalid argument 3 on line: " << code_line << std::endl;
        break;
    }
    print(cmd, args);
}