#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <random>
#include <memory>

#include "utf8.hh"

class WordAtom
{
private:
public:
    std::string value;
    WordAtom(std::string v) : value(v) {}
    // bool empty() const { return value.empty(); }
    // std::string back() const { return  std::string(1,value.back()); }
    std::string to_string() const { return value; }
};

std::ostream &operator<<(
    std::ostream &stream,
    const WordAtom &atom)
{
    return std::operator<<(stream, atom.to_string());
}

template <typename A>
class Molecule
{
private:
    std::vector<A> atoms;

public:
    Molecule() : atoms() {};
    size_t size() { return atoms.size(); };
    std::vector<A> getAtoms() { return atoms; }
    Molecule<A> operator+(const Molecule<A> &molecule)
    {
        std::vector<A> copy = atoms;
        copy.insert(copy.end(), molecule.getAtoms().begin(), molecule.getAtoms().end());
        return copy;
    }
    Molecule<A> operator+(const A &atom)
    {
        std::vector<A> copy = atoms;
        copy.emplace_back(atom);
        return copy;
    }
    Molecule<A>& operator+=(const A &atom)
    {
        atoms.emplace_back(atom);
        return *this;
    }
    std::string to_string() {
        std::string ret;
        for (A a : atoms)
            ret+= a.to_string();
        return to_string();
    }

    bool empty() const { return atoms.empty(); }
    A back() const { return atoms.back(); }
};

template <typename A>
std::ostream &operator<<(
    std::ostream &stream,
    const Molecule<A> &molecule)
{
    return std::operator<<(stream, molecule.to_string());
}

/* ############### Molecule TEMPLATE IMPLEMENTATION ############### */

/* ############### MoleculeModel TEMPLATE IMPLEMENTATION ############### */

template <typename A>
class MoleculeModel
{
private:
    std::vector<size_t> lengthsFrequencies;
    std::vector<std::map<Molecule<A>, std::map<A, size_t>>> maps;
    unsigned int contextSize;
    size_t totalWordsLearned;
    float end_ratio;

public:
    MoleculeModel(int contextSize, float endRatio);
    void addStr(Molecule<A> str, A c);
    void addLength(int length);
    Molecule<A> aggregateWordGen(Molecule<A> begin);
};

int randint(int min, int max)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

template <typename A>
MoleculeModel<A>::MoleculeModel(int contextSize, float endRatio) : lengthsFrequencies(20), maps(contextSize), contextSize(contextSize), end_ratio(endRatio)
{
}

template <typename A>
void MoleculeModel<A>::addStr(Molecule<A> ctx, A c)
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
void MoleculeModel<A>::addLength(int length)
{
    totalWordsLearned++;
    for (int i = lengthsFrequencies.size(); i < length; i++)
    {
        lengthsFrequencies.emplace_back(0);
    }

    lengthsFrequencies.at(length - 1)++;
}

template <typename A>
Molecule<A> MoleculeModel<A>::aggregateWordGen(Molecule<A> begin)
{
    size_t sizeOfStr = utf8_length(begin);
    Molecule<A> ctxSearch;
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