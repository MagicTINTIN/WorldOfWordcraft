#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <random>

#include "atomsAndMolecules.hh"
#include "utf8.hh"
#include "WordModel.hh"

#define DEFAULT_CONTEXT 3
#define DEFAULT_END_CHANCE_RATIO_PERCENTAGE 0.7
#define MAX_GENERATION_TRIES 500

bool wordIn(const std::string &word, const std::vector<std::string> &list)
{
    for (size_t i = 0; i < list.size(); i++)
    {
        if (list.at(i).compare(word) == 0)
            return true;
    }
    return false;
}

int main(int argc, char const *argv[])
{
    if (argc < 4 || argc > 6)
    {
        std::cerr << "Usage: \n";
        std::cerr << argv[0] << " <source_file> <output_file> <number_of_words_to_generate> [context_size] [end_ratio]\n";
        return 1;
    }

    int contextSize = DEFAULT_CONTEXT;
    if (argc >= 5)
    {
        contextSize = atoi(argv[4]);
    }

    float end_ratio = DEFAULT_END_CHANCE_RATIO_PERCENTAGE;
    if (argc >= 6)
    {
        end_ratio = atof(argv[5]);
    }

    MoleculeModel<WordAtom> model(contextSize, end_ratio);

    std::ifstream infile(argv[1]);
    std::ofstream outfile(argv[2]);

    std::string line;

    long unsigned int generatedNumber = atoi(argv[3]);

    unsigned int lines(0);
    printf("Reading source file...\nLine n°0");
    fflush(stdout);
    while (std::getline(infile, line))
    {
        lines++;
        printf("\rLine n°%d", lines);
        fflush(stdout);
        // printf("Str=%s : len=%ld, size=%ld, utf8=%ld | ", line.c_str(), line.length(), line.size(), utf8_length(line));
        std::transform(line.begin(), line.end(), line.begin(), ::tolower);
        // printf("len=%ld, size=%ld, utf8=%ld >>> ", line.length(), line.size(), utf8_length(line));
        std::string cleaned = deleteChar(line, 13) + "\n";
        // printf("len=%ld, size=%ld, utf8=%ld\n", cleaned.length(), cleaned.size(), utf8_length(cleaned));
        size_t length = utf8_length(cleaned);
        model.addLength(length);
        for (size_t lastc = 0; lastc < length; lastc++)
        {
            Molecule<WordAtom> ctx;
            for (size_t i = std::max(0, (int)lastc - contextSize); i < lastc && lastc > 0; i++)
            {
                ctx += WordAtom(utf8_char_at(line, i));
            }
            std::string charToPut = utf8_char_at(line, lastc, "\n");
            model.addStr(ctx, charToPut);
        }
    }
    printf("\rStats generated.\nStart generating words...\n###########################################\n");
    std::vector<Molecule<WordAtom>> foundWords(0);
    int maxTries = 0;
    for (size_t i = 0; i < generatedNumber; i++)
    {
        Molecule<WordAtom> newWord;
        while (newWord.empty() || newWord.back().to_string() != "\n")
        {
            newWord = model.aggregateWordGen(newWord);
        }
        if (wordIn(newWord, foundWords))
        {
            maxTries++;
            if (maxTries < MAX_GENERATION_TRIES)
                i--;
        }
        else
        {
            foundWords.emplace_back(newWord);
            std::cout << newWord;
            // outfile << newWord;
            // std::cout << "size: " << utf8_length(newWord) << ", " << newWord;
        }
    }

    return 0;
}
