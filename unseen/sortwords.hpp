#include <iostream>
#include <map>
#include <string>
#include <mutex>

class SortWords
{
public:
    SortWords(const char &delimiter, const char &order);
    bool SortFiles(char **argv);
    bool MapFile(const char *path);
    bool WriteToFile(const char *path);

private:
    std::map<std::string, int> dict;
    std::mutex mutex;
    char m_delimiter;
    char m_order;
};