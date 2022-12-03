#include "writer.h"

void write_file(std::string file_name, std::string data)
{
    std::ofstream outfile;
    outfile.open(file_name);
    outfile << data;
    outfile.close();
    std::cout << "wrote solution to " << file_name << "\n";
}
