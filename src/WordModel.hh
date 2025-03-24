#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <random>
#include <memory>

#include "utf8.hh"

class ModelAtom
{
private:
public:
    ModelAtom();
    ModelAtom get();
    static ModelAtom endAtom() { return ModelAtom(); }
    static ModelAtom startAtom() { return ModelAtom(); }

};

class utf8Atom : ModelAtom
{
private:
    std::string c;
public:
    utf8Atom(std::string content) : c(content) {}
    static utf8Atom endAtom() { return utf8Atom("\n"); }
    static utf8Atom startAtom() { return utf8Atom(" "); }
};

ModelAtom::ModelAtom()
{
}

template <typename A>
class ModelObject
{
private:
    std::vector<A> atoms;

public:
    ModelObject() : atoms() {};
    size_t size() { return atoms.size(); };
    std::vector<A> getAtoms() { return atoms; }
    ModelObject<A> operator+(ModelObject<A> const &object)
    {
        std::vector<A> copy = atoms;
        copy.insert(copy.end(), object.getAtoms().begin(), object.getAtoms().end());
        return copy;
    }
};

/* ############### MODELOBJECT TEMPLATE IMPLEMENTATION ############### */

/* ############### WORDMODEL TEMPLATE IMPLEMENTATION ############### */

template <typename A>
class WordModel
{
private:
    std::vector<size_t> lengthsFrequencies;
    std::vector<std::map<ModelObject<A>, std::map<A, size_t>>> maps;
    unsigned int contextSize;
    size_t totalWordsLearned;
    float end_ratio;

public:
    WordModel(int contextSize, float endRatio);
    void addStr(ModelObject<A> str, A c);
    void addLength(int length);
    ModelObject<A> aggregateWordGen(ModelObject<A> begin);
};

int randint(int min, int max)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

template <typename A>
WordModel<A>::WordModel(int contextSize, float endRatio) : lengthsFrequencies(20), maps(contextSize), contextSize(contextSize), end_ratio(endRatio)
{
}

template <typename A>
void WordModel<A>::addStr(ModelObject<A> ctx, A c)
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
        maps.at(sizeOfStr).insert(std::make_pair(ctx, std::map<A, size_t>()));
        maps.at(sizeOfStr)[ctx].insert(std::make_pair(c, 1));
    }
}

template <typename A>
void WordModel<A>::addLength(int length)
{
    totalWordsLearned++;
    for (int i = lengthsFrequencies.size(); i < length; i++)
    {
        lengthsFrequencies.emplace_back(0);
    }

    lengthsFrequencies.at(length - 1)++;
}

template <typename A>
ModelObject<A> WordModel<A>::aggregateWordGen(ModelObject<A> begin)
{
    size_t sizeOfStr = utf8_length(begin);
    ModelObject<A> ctxSearch;
    int ctxSize = contextSize - 1;
    if (sizeOfStr < contextSize)
    {
        ctxSearch = " " + begin;
        ctxSize = sizeOfStr;
    }
    else
    {
        int beginIndex = std::max(0, (int)sizeOfStr - (int)contextSize);
        ctxSearch = ""; // begin.substr(beginIndex, contextSize);
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