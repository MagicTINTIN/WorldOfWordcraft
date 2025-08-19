#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map>
#include <random>

#include "atomsAndMolecules.hh"
#include "utf8.hh"
#include "crafter.hh"

#define DEFAULT_CONTEXT 4
#define DEFAULT_CONTEXT_VARIATION 1
#define DEFAULT_SERIOUSNESS 2
#define DEFAULT_END_CHANCE_RATIO_PERCENTAGE 0.5
#define DEFAULT_CHAOS 0.
#define MAX_GENERATION_TRIES 500

int main_chars(int argc, char const *argv[])
{
    int contextSize = DEFAULT_CONTEXT;
    if (argc >= 6)
    {
        contextSize = atoi(argv[5]);
    }

    float end_ratio = DEFAULT_END_CHANCE_RATIO_PERCENTAGE;
    if (argc >= 7)
    {
        end_ratio = atof(argv[6]);
    }

    float chaos = DEFAULT_CHAOS;
    if (argc >= 8)
    {
        chaos = atof(argv[7]);
    }

    float contextVariation = DEFAULT_CONTEXT_VARIATION;
    if (argc >= 9)
    {
        contextVariation = atof(argv[8]);
    }

    float seriousness = DEFAULT_SERIOUSNESS;
    if (argc >= 10)
    {
        seriousness = atof(argv[9]);
    }

    MoleculeModel<CharAtom> model(contextSize, end_ratio, chaos, contextVariation, seriousness, CharAtom(" "), CharAtom("\n"));

    std::ifstream infile(argv[2]);
    std::ofstream outfile(argv[3]);

    std::string line;

    long unsigned int generatedNumber = atoi(argv[4]);

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
            Molecule<CharAtom> ctx;
            for (size_t i = std::max(0, (int)lastc - contextSize); i < lastc && lastc > 0; i++)
            {
                ctx += CharAtom(utf8_char_at(line, i));
            }
            std::string charToPut = utf8_char_at(line, lastc, "\n");
            model.addStr(ctx, charToPut);
        }
    }
    printf("\n");
    model.printMaps();
    printf("\rStats generated.\nStart generating words...\n###########################################\n");
    std::vector<Molecule<CharAtom>> foundWords(0);
    int maxTries = 0;
    for (size_t i = 0; i < generatedNumber; i++)
    {
        Molecule<CharAtom> newWord;
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

int main_words(int argc, char const *argv[])
{
    int contextSize = DEFAULT_CONTEXT;
    if (argc >= 6)
    {
        contextSize = atoi(argv[5]);
    }

    float end_ratio = DEFAULT_END_CHANCE_RATIO_PERCENTAGE;
    if (argc >= 7)
    {
        end_ratio = atof(argv[6]);
    }

    float chaos = DEFAULT_CHAOS;
    if (argc >= 8)
    {
        chaos = atof(argv[7]);
    }

    float contextVariation = DEFAULT_CONTEXT_VARIATION;
    if (argc >= 9)
    {
        contextVariation = atof(argv[8]);
    }

    float seriousness = DEFAULT_SERIOUSNESS;
    if (argc >= 10)
    {
        seriousness = atof(argv[9]);
    }

    MoleculeModel<WordAtom> model(contextSize, end_ratio, chaos, contextVariation, seriousness, WordAtom("\t"), WordAtom("\n"));

    std::ifstream infile(argv[2]);
    std::ofstream outfile(argv[3]);

    std::string line;

    long unsigned int generatedNumber = atoi(argv[4]);

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
        std::stringstream cleaned(deleteChar(deleteChar(line, '.'), 13));
        std::string word;
        std::vector<std::string> words;

        while (std::getline(cleaned, word, ' '))
        {
            words.push_back(word + " ");
        }
            // words.push_back("\n");
        // printf("len=%ld, size=%ld, utf8=%ld\n", cleaned.length(), cleaned.size(), utf8_length(cleaned));
        size_t length = words.size();
        model.addLength(length);
        for (size_t lastw = 0; lastw < length; lastw++)
        {
            Molecule<WordAtom> ctx;
            for (size_t i = std::max(0, (int)lastw - contextSize); i < lastw && lastw > 0; i++)
            {
                ctx += WordAtom(words[i]);
            }
            // std::string charToPut = utf8_char_at(line, lastw, "\n");
            model.addStr(ctx, words[lastw]);
        }
    }
    // printf("\rMaps:\n");
    // model.printMaps();
    printf("\rStats generated.\nStart generating sentences...\n###########################################\n");
    std::vector<Molecule<WordAtom>> foundSentences(0);
    int maxTries = 0;
    for (size_t i = 0; i < generatedNumber; i++)
    {
        Molecule<WordAtom> newSentence;
        while (newSentence.empty() || newSentence.back().to_string() != "\n")
        {
            newSentence = model.aggregateWordGen(newSentence);
            // std::cout << "=== " << newSentence << "\n";
        }
        if (wordIn(newSentence, foundSentences))
        {
            maxTries++;
            if (maxTries < MAX_GENERATION_TRIES)
                i--;
        }
        else
        {
            foundSentences.emplace_back(newSentence);
            std::cout << newSentence;
            // outfile << newWord;
            // std::cout << "size: " << utf8_length(newWord) << ", " << newWord;
        }
    }

    return 0;
}

// bool wordIn(const std::string &word, const std::vector<std::string> &list)
// {
//     for (size_t i = 0; i < list.size(); i++)
//     {
//         if (list.at(i).compare(word) == 0)
//             return true;
//     }
//     return false;
// }

int main(int argc, char const *argv[])
{
    if (argc < 5 || argc > 10)
    {
        std::cerr << "Usage: \n";
        std::cerr << argv[0] << " <mode=char|word><source_file> <output_file> <number_of_words_to_generate> [context_size] [end_ratio] [chaos] [context_variation] [seriousness]\n";
        return 1;
    }

    printf("Token mode: %s\n", argv[1]);
    if (std::string(argv[1]) == "char")
        return main_chars(argc, argv);
    else if (std::string(argv[1]) == "word")
        return main_words(argc, argv);
    else
        fprintf(stderr, "Token mode '%s' is unkown.\nOnly 'char' and 'word' are currently supported.\n", argv[1]);
    return 7;
}
