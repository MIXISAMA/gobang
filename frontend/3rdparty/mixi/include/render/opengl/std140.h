#include "pch.h"

class Std140
{
protected:
    size_t offset_;
    size_t bytes_;
};

class Std140Basic : public Std140
{
public:
    enum class Type
    {
        FLOAT,
        INT,
    };
    Type type;
    Std140Basic(Type type);
};

class Std140Struct : public Std140
{
public:
    Std140Struct(const std::initializer_list<Std140>& members);
    const Std140 & operator[] (int pos) const;
protected:
    std::vector<Std140> members_;
};

class Std140Array : public Std140
{
public:
    Std140Array(const Std140& member, int size);
protected:
    Std140 member_;
    int size_;
};
