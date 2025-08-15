#pragma once
#include <string>
#include <vector>
#include <iostream>

template <typename A>
bool wordIn(const Molecule<A> &molecule, const std::vector<Molecule<A>> &list)
{
    for (size_t i = 0; i < list.size(); i++)
    {
        if (list.at(i).to_string().compare(molecule.to_string()) == 0)
            return true;
    }
    return false;
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
    std::string to_string() const {
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
