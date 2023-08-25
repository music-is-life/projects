#include "sortwords.hpp"

int main(int argc, char **argv)
{
    char order;
    char delimiter;
    std::cout << "Enter a for acsending, or d for Descending order: \n";
    scanf(" %c",&order);

    std::cout << "Enter delimiter in WRITING file: \n";
    scanf(" %c",&delimiter);

    SortWords sw(delimiter, order);

    sw.SortFiles(argv);
}