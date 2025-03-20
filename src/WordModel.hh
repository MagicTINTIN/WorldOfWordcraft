#pragma once
#include <string>
#include <vector>
#include <map>

class WordModel
{
private:
    std::vector<size_t> lengthsFrequencies;
    std::vector<std::map<std::string, std::map<std::string, size_t>>> maps;
    unsigned int contextSize;
    size_t totalWordsLearned;
    float end_ratio;

public:
    WordModel(int contextSize, float endRatio);
    ~WordModel();
    void addStr(std::string str, std::string c);
    void addLength(int length);
    std::string aggregateWordGen(std::string begin);
};