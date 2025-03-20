#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <random>

#include "utf8.hh"
#include "WordModel.hh"

int randint(int min, int max)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

WordModel::WordModel(int contextSize, float endRatio) : lengthsFrequencies(20), maps(contextSize), contextSize(contextSize), end_ratio(endRatio)
{
}

WordModel::~WordModel()
{
}

void WordModel::addStr(std::string ctx, std::string c)
{
    size_t sizeOfStr = utf8_length(ctx);
    if (sizeOfStr < contextSize)
        ctx = " " + ctx;
    else
        sizeOfStr--;
    if (maps.at(sizeOfStr).count(ctx))
    {
        if (maps.at(sizeOfStr)[ctx].count(c))
            maps.at(sizeOfStr)[ctx][c]++;
        else
            maps.at(sizeOfStr)[ctx].insert(std::make_pair(c, 1));
    }
    else
    {
        maps.at(sizeOfStr).insert(std::make_pair(ctx, std::map<std::string, size_t>()));
        maps.at(sizeOfStr)[ctx].insert(std::make_pair(c, 1));
    }
}

void WordModel::addLength(int length)
{
    totalWordsLearned++;
    for (int i = lengthsFrequencies.size(); i < length; i++)
    {
        lengthsFrequencies.emplace_back(0);
    }

    lengthsFrequencies.at(length - 1)++;
}

std::string WordModel::aggregateWordGen(std::string begin)
{
    size_t sizeOfStr = utf8_length(begin);
    std::string ctxSearch;
    int ctxSize = contextSize - 1;
    if (sizeOfStr < contextSize)
    {
        ctxSearch = " " + begin;
        ctxSize = sizeOfStr;
    }
    else
    {
        int beginIndex = std::max(0, (int)sizeOfStr - (int)contextSize);
        ctxSearch = (""); // begin.substr(beginIndex, contextSize);
        for (size_t i = 0; i < contextSize; i++)
        {
            ctxSearch += utf8_char_at(begin, beginIndex + i);
        }
    }
    if (!maps.at(ctxSize).count(ctxSearch) || maps.at(ctxSize)[ctxSearch].empty())
        return begin + "\n";

    size_t sum(0), numberOfEOL(0);
    for (auto it = maps.at(ctxSize)[ctxSearch].begin(); it != maps.at(ctxSize)[ctxSearch].end(); ++it)
    {
        // std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
        if (it->first.compare("\n") == 0)
            numberOfEOL += it->second;
        else
            sum += it->second;
    }

    size_t numberOfBiggerWords(0);
    for (size_t indSumIndex = sizeOfStr; indSumIndex < lengthsFrequencies.size(); indSumIndex++)
    {
        numberOfBiggerWords += lengthsFrequencies[indSumIndex];
    }
    float ratioPhase = end_ratio * (totalWordsLearned - numberOfBiggerWords) / totalWordsLearned;
    float EOLMultiplierFactor = numberOfEOL == 0 ? 0 : (ratioPhase * sum) / ((1 - ratioPhase) * numberOfEOL);
    sum += EOLMultiplierFactor * numberOfEOL;

    if (sum == 0)
        return begin + "\n";

    size_t indexCharChosen = randint(0, sum - 1);
    for (auto it = maps.at(ctxSize)[ctxSearch].begin(); it != maps.at(ctxSize)[ctxSearch].end(); ++it)
    {
        // std::string current = it->first;
        if (it->first.compare("\n") == 0)
        {
            if (indexCharChosen < it->second * EOLMultiplierFactor)
                return begin + it->first;
            indexCharChosen -= it->second * EOLMultiplierFactor;
        }
        else
        {
            // std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
            if (indexCharChosen < it->second)
                return begin + it->first;
            indexCharChosen -= it->second;
        }
    }

    return begin + "\n";
}