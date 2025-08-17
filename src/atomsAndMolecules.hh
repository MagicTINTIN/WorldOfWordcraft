#pragma once
#include <string>
#include <vector>
#include <iostream>

class CharAtom
{
private:
public:
    std::string value;
    CharAtom(std::string v) : value(v) {}
    // bool empty() const { return value.empty(); }
    // std::string back() const { return  std::string(1,value.back()); }
    int compare(const CharAtom &other) const { return value != other.value; }
    std::string to_string() const { return value; }

    bool operator<(const CharAtom &atom) const
    {
        return value < atom.value;
    }
};

std::ostream &operator<<(
    std::ostream &stream,
    const CharAtom &atom)
{
    return std::operator<<(stream, atom.value);
}

class WordAtom
{
private:
public:
    std::string value;
    WordAtom(std::string v) : value(v) {}
    // bool empty() const { return value.empty(); }
    // std::string back() const { return  std::string(1,value.back()); }
    int compare(const WordAtom &other) const { return value != other.value; }
    std::string to_string() const { return value; }

    bool operator<(const WordAtom &atom) const
    {
        return value < atom.value;
    }
};

std::ostream &operator<<(
    std::ostream &stream,
    const WordAtom &atom)
{
    return std::operator<<(stream, atom.value);
}

template <typename A>
class Molecule
{
private:
    std::vector<A> atoms;

    std::string str;

public:
    Molecule() : atoms() {};
    Molecule(A atom) : atoms{atom} {};
    Molecule(std::vector<A> atoms_array) : atoms(atoms_array) {};
    size_t size() const { return atoms.size(); };
    std::vector<A> getAtoms() const { return atoms; }
    Molecule<A> operator+(const Molecule<A> &molecule) const
    {
        Molecule<A> copy;
        copy.atoms = atoms;
        copy.atoms.insert(copy.atoms.end(), molecule.atoms.begin(), molecule.atoms.end());
        copy.str = str + molecule.str;
        return copy;
    }
    bool operator<(const Molecule<A> &molecule) const
    {
        return str < molecule.str;
    }

    Molecule<A> operator+(const A &atom) const
    {
        Molecule<A> copy;
        copy.atoms = atoms;
        copy.atoms.emplace_back(atom);
        copy.str = str + atom.to_string();
        return copy;
    }
    Molecule<A> operator+=(const A &atom)
    {
        atoms.emplace_back(atom);
        str += atom.to_string();
        return *this;
    }
    Molecule<A> subMolecule(size_t size)
    {
        Molecule<A> sub;
        sub.atoms.insert(sub.atoms.end(), atoms.begin(), atoms.begin() + std::min(size, atoms.size()));
        
        // std::string ret;
        // for (A a : atoms)
        //     ret += a.to_string();

        sub.str = str.substr(std::min(size, str.size()));
        return sub;
    }
    Molecule<A> subMolecule(size_t begin, size_t end)
    {
        // if (begin > end)
        // {
        //     tmp = begin;
        //     begin = end;
        //     end = tmp;
        // }

        Molecule<A> sub;
        sub.atoms.insert(sub.atoms.end(), atoms.begin() + std::max(0, (int) begin), atoms.begin() + std::min(end, atoms.size()));
        
        std::string sub_str;
        for (A a : sub.atoms)
            sub_str += a.to_string();

        sub.str = sub_str;//str.substr(std::max(0, (int) begin), std::min(end, str.size()) - 1);
        return sub;
    }
    std::string to_string() const
    {
        return str;
    }

    bool empty() const { return atoms.empty(); }
    A back() const { return atoms.back(); }
    size_t size() { return atoms.size(); }
};

template <typename A>
std::ostream &operator<<(
    std::ostream &stream,
    const Molecule<A> &molecule)
{
    return std::operator<<(stream, molecule.to_string());
}

template <typename A>
bool wordIn(Molecule<A> &molecule, std::vector<Molecule<A>> &list)
{
    for (size_t i = 0; i < list.size(); i++)
    {
        if (list.at(i).to_string().compare(molecule.to_string()) == 0)
            return true;
    }
    return false;
}