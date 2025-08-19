#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <random>
#include <memory>
#include "atomsAndMolecules.hh"

#define MAX_TOKEN_AGGREGATED 5000
#include "utf8.hh"

/* ############### Molecule TEMPLATE IMPLEMENTATION ############### */

/* ############### MoleculeModel TEMPLATE IMPLEMENTATION ############### */

template <typename A>
class MoleculeModel
{
private:
    std::vector<size_t> lengthsFrequencies;
    std::vector<std::map<Molecule<A>, std::map<A, size_t>>> maps;
    const unsigned int contextSize;
    size_t totalWordsLearned;
    float end_ratio;
    const A _begin;
    const A _end;
    const float _chaos;
    const unsigned int _contextVariation;
    const unsigned int _seriousness;

public:
    MoleculeModel(int contextSize, float endRatio, float chaos, unsigned int contextVariation, unsigned int seriousness, A beginAtom, A endAtom);
    void addStr(Molecule<A> str, A c);
    void addLength(int length);
    Molecule<A> aggregateWordGen(Molecule<A> begin);
    void printMaps();
};

int randint(int min, int max)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

template <typename A>
void MoleculeModel<A>::printMaps()
{
    int size = 0;
    for (const std::map<Molecule<A>, std::map<A, size_t>> &ms : maps)
    {
        printf("Context size=%d\n", size);
        for (auto it = ms.begin(); it != ms.end(); ++it)
        {
            std::cout << "CONTEXT: " << it->first << std::endl;
            for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
            {
                std::cout << "|> '" << it2->first << "': " << it2->second << std::endl;
            }
            printf("\n");
        }
        size++;
        printf("\n------------------------\n\n");
    }
}

template <typename A>
MoleculeModel<A>::MoleculeModel(int contextSize, float endRatio, float chaos, unsigned int contextVariation, unsigned int seriousness, A beginAtom, A endAtom) : lengthsFrequencies(20), maps(contextSize), contextSize(contextSize), _chaos(1.0 - chaos), end_ratio(endRatio), _contextVariation(contextVariation), _seriousness(seriousness), _begin(beginAtom), _end(endAtom)
{
}

template <typename A>
void MoleculeModel<A>::addStr(Molecule<A> ctx, A c)
{
    size_t sizeOfStr = ctx.size(); // utf8_length(ctx);
    if (sizeOfStr < contextSize)
        ctx = Molecule<A>(_begin) + ctx;
    else
        sizeOfStr--;
    // int a = std::max(sizeOfStr - _contextVariation, 0UL);
    for (size_t cxsz = std::max((int)sizeOfStr - (int) _contextVariation, 0); cxsz <= sizeOfStr; cxsz++)
    {
        Molecule<A> sctx(ctx.subMolecule(sizeOfStr - cxsz, sizeOfStr + 1));
        if (maps.at(cxsz).count(sctx))
        {
            if (maps.at(cxsz)[sctx].count(c))
                maps.at(cxsz)[sctx][c]++;
            else
                maps.at(cxsz)[sctx].insert(std::make_pair(c, 1));
        }
        else
        {
            maps.at(cxsz).insert(std::make_pair(sctx, std::map<A, size_t>()));
            maps.at(cxsz)[sctx].insert(std::make_pair(c, 1));
        }
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
    size_t sizeOfStr = begin.size(); // utf8_length(begin);
    Molecule<A> ctxSearch;
    int ctxSize = contextSize - 1;
    if (sizeOfStr < contextSize)
    {
        ctxSearch = Molecule<A>(_begin) + begin;
        ctxSize = sizeOfStr;
    }
    else
    {
        // int beginIndex = std::max(0, (int)sizeOfStr - (int)contextSize);
        // ctxSearch = ""; // begin.substr(beginIndex, contextSize);
        // for (size_t i = 0; i < contextSize; i++)
        // {
        //     ctxSearch += utf8_char_at(begin, beginIndex + i);
        // }
        int beginIndex = std::max(0, (int) sizeOfStr - (int) contextSize);
        ctxSearch = begin.subMolecule(beginIndex, beginIndex + contextSize);
    }
    if (!maps.at(ctxSize).count(ctxSearch) || maps.at(ctxSize)[ctxSearch].empty() || sizeOfStr > MAX_TOKEN_AGGREGATED)
        return begin + _end;

    size_t sum(0), numberOfEOL(0), sizeCtx(ctxSearch.size());
    // std::cout << ">>> " << ctxSearch << "\n";
    for (size_t cxsz = std::max((int)sizeCtx - (int) _contextVariation - 1, 0); cxsz < sizeCtx; cxsz++)
    {
        Molecule<A> subCtxSearch(ctxSearch.subMolecule(sizeCtx - cxsz - 1, sizeCtx));
        for (auto it = maps.at(cxsz)[subCtxSearch].begin(); it != maps.at(cxsz)[subCtxSearch].end(); ++it)
        {
            // std::cout << "- Key: " << it->first << ", Value: " << it->second << std::endl;
            unsigned int weight = (1 + (_chaos * (it->second - 1))) * std::pow(cxsz + 1, _seriousness);
            if (it->first.compare(_end) == 0)
                numberOfEOL += weight;
            else
                sum += weight;
        }
    }

    // printf("\n");

    size_t numberOfBiggerMolecule(0);
    for (size_t indSumIndex = sizeOfStr; indSumIndex < lengthsFrequencies.size(); indSumIndex++)
    {
        numberOfBiggerMolecule += lengthsFrequencies[indSumIndex];
    }
    float ratioPhase = end_ratio * (totalWordsLearned - numberOfBiggerMolecule) / totalWordsLearned;
    float EOLMultiplierFactor = numberOfEOL == 0 ? 0 : (ratioPhase * sum) / ((1 - ratioPhase) * numberOfEOL);
    sum += EOLMultiplierFactor * numberOfEOL;

    if (sum == 0)
        return begin + _end;

    size_t indexCharChosen = randint(0, sum - 1);
    for (size_t cxsz = std::max((int)sizeCtx - (int) _contextVariation - 1, 0); cxsz < sizeCtx; cxsz++)
    {
        Molecule<A> subCtxSearch(ctxSearch.subMolecule(sizeCtx - cxsz - 1, sizeCtx));
        for (auto it = maps.at(cxsz)[subCtxSearch].begin(); it != maps.at(cxsz)[subCtxSearch].end(); ++it)
        {
            unsigned int weight = (1 + (_chaos * (it->second - 1))) * std::pow(cxsz + 1, _seriousness);
            // std::string current = it->first;
            if (it->first.compare(_end) == 0)
            {
                if (indexCharChosen < weight * EOLMultiplierFactor)
                    return begin + it->first;
                indexCharChosen -= weight * EOLMultiplierFactor;
            }
            else
            {
                // std::cout << "Key: " << it->first << ", Value: " << weight << std::endl;
                if (indexCharChosen < weight)
                    return begin + it->first;
                indexCharChosen -= weight;
            }
        }
    }

    return begin + _end;
}