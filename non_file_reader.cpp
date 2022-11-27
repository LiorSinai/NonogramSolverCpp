/*
 *  Created on: 23 October, 2020
 *      Author: Lior Sinai
 * Description: read in nonogram files
 */

#include "non_file_reader.h"

Runs read_non_file(std::string file_name)
{
    std::string ext = file_name.substr(file_name.find('.') + 1, file_name.length() - 1);
    if (ext == "non" || ext == "txt")
    {
        return read_non_ext(file_name);
    }
    else if (ext == "sgriddler")
    {
        return read_sgriddler_ext(file_name);
    }
    else
    {
        std::cout << "Unrecognised file extension " << ext << ". Please use \"txt\", \"non\" or \"sgriddler\" files";
        throw "WARNING no file read";
    }
}

Runs read_non_ext(std::string file_name)
{
    std::string line;
    std::ifstream infile(file_name);
    if (infile.fail())
    {
        std::cout << "reading file " << file_name << " failed";
        throw "WARNING no file read";
    }

    std::vector<std::vector<int>> runs_row{};
    std::vector<std::vector<int>> runs_col{};

    std::vector<std::vector<int>> *active_runs = &runs_row;

    int read_mode{0};

    while (std::getline(infile, line))
    {
        std::vector<int> runs;
        std::stringstream ss(line);
        switch (read_mode)
        {
        case 0: // read in words
            if (line == "rows")
            {
                active_runs = &runs_row;
                read_mode = 1;
            }
            else if (line == "columns")
            {
                active_runs = &runs_col;
                read_mode = 1;
            }
            break;
        case 1:
            // double check if there are more words
            if (line == "")
            {
                read_mode = 0;
                break;
            }
            else if (line == "rows")
            {
                active_runs = &runs_row;
                read_mode = 1;
                break;
            }
            else if (line == "columns")
            {
                active_runs = &runs_col;
                read_mode = 1;
                break;
            }
            // Parse the string
            for (int i; ss >> i;)
            {
                runs.push_back(i);
                if (ss.peek() == ',')
                    ss.ignore();
            }
            active_runs->push_back(runs);
            break;
        }
    }
    if (runs_row.empty() || runs_col.empty())
    {
        std::cout << "WARNING no runs read" << std::endl;
        throw "WARNING no file read";
    }
    else
    {
        std::cout << "file loaded" << std::endl;
    }

    return Runs{runs_row, runs_col};
}

Runs read_sgriddler_ext(std::string file_name)
{
    std::string line;
    std::ifstream infile(file_name);
    if (infile.fail())
    {
        std::cout << "reading file " << file_name << " failed";
        throw "WARNING no file read";
    }

    std::vector<std::vector<int>> runs_row{};
    std::vector<std::vector<int>> runs_col{};

    std::getline(infile, line); // title
    std::getline(infile, line); // \n
    std::getline(infile, line); // width, height
    auto mid = line.find(' ');
    int width = std::stoi(line.substr(0, mid));
    int height = std::stoi(line.substr(mid + 1, line.length() - 1));

    std::getline(infile, line); // \n

    // read solution
    while (std::getline(infile, line))
    {
        if (line == "" || line == "\n")
        {
            break;
        }
    }
    // read columns (in reverse)
    for (int j{0}; j < width; j++)
    {
        std::getline(infile, line);
        std::vector<int> runs;
        if (line == "" || line == "\n")
        {
            runs = {0};
        }
        else
        { // Parse the string
            std::stringstream ss(line);
            for (int i; ss >> i;)
            {
                runs.insert(runs.begin(), i);
                if (ss.peek() == ' ')
                    ss.ignore();
            }
        }
        runs_col.push_back(runs);
    }
    std::getline(infile, line); // \n
    // read rows (in reverse)
    for (int j{0}; j < height; j++)
    {
        std::getline(infile, line);
        std::vector<int> runs;
        if (line == "" || line == "\n")
        {
            runs = {0};
        }
        else
        { // Parse the string
            std::stringstream ss(line);
            for (int i; ss >> i;)
            {
                runs.insert(runs.begin(), i);
                if (ss.peek() == ',')
                    ss.ignore();
            }
        }
        runs_row.push_back(runs);
    }

    if (runs_row.empty() || runs_col.empty())
    {
        std::cout << "WARNING no runs read" << std::endl;
        throw "WARNING no file read";
    }
    else
    {
        std::cout << "file loaded" << std::endl;
    }

    return Runs{runs_row, runs_col};
}
