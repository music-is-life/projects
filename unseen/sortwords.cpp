#include <cstdio>
#include <thread>
#include <vector>
#include <cstring>
#include "sortwords.hpp"


char WhichDelimiter(char delim)
{
    switch(delim)
    {
        case 's':
            return ' ';
        case 'c':
            return ',';
        case 'n':
            return '\n';

        default:
            return ',';
    }
}

void toLower(std::string &s)
{
    size_t i = 0;
    for(i = 0; i < s.size(); ++i)
    {
        s[i] = tolower(s[i]);
    }
}

SortWords::SortWords(const char &delimiter, const char &order): m_delimiter(WhichDelimiter(delimiter)), m_order(order){}

bool SortWords::SortFiles(char **argv)
{
    const int num_of_threads = 3;

    size_t i = 0;
    std::vector<std::thread> thread_vec;

    for(i = 0; i < num_of_threads; ++i)
    {
        thread_vec.push_back(std::thread(&SortWords::MapFile, this, argv[i+1]));
    }

    for(i = 0; i < num_of_threads; ++i)
    {
        thread_vec[i].join();
    }

    std::thread write_thread = std::thread(&SortWords::WriteToFile, this, "/home/daniel/projects/unseen/f4.txt");
    write_thread.join();

    return false;
}

bool SortWords::MapFile(const char *path)
{
    char c;
    std::string word;
    FILE *fd = fopen(path, "r");
    if(!fd)
    {
        return true;
    }
    
    while( (c = getc(fd)) != EOF) //potential problem: starting with
    {
        if(isalpha(c))
        {
            word += c;
        }
        else if(word.size() > 0)
        {
            toLower(word);
            mutex.lock();
            ++dict[word];
            mutex.unlock();
            word = "";
        }
    }
    
    return false;
}

bool SortWords::WriteToFile(const char *path)
{
    FILE *fd = fopen(path, "w");
    std::map<std::string, int>::iterator max = dict.begin();
    std::map<std::string, int>::iterator it;

    if(!fd)
    {
        return true;
    }

    if(m_order == 'a')
    {
        for(it = dict.begin(); it != dict.end(); ++it)
        {
            if(it->second > max->second)
            {
                max = it;
            }
            fwrite(it->first.c_str(), it->first.size(), 1, fd);
            fwrite(&m_delimiter, 1, 1, fd);
        }
    }
    else
    {
        for(it = dict.end(); it != dict.begin(); --it)
        {
            if(it->second > max->second)
            {
                max = it;
            }
            fwrite(it->first.c_str(), it->first.size(), 1, fd);
            fwrite(&m_delimiter, 1, 1, fd);
        }
    }
    /* Writing last line.*/
    char buffer[30];
    sprintf(buffer, "%d", max->second);

    std::string text = "\n\n The most freuent word in the text is: ";

    text += max->first.c_str();
    text += ", count: ";
    text += buffer;

    fwrite(text.c_str(), text.size(), 1, fd);

    return false;
}

