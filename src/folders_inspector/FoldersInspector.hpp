#pragma once
#include "configs/SearchConfig.hpp"
#include <memory>


class FoldersInspector {
public:

public:
    FoldersInspector();
    ~FoldersInspector();
    void walk();

private:
    struct Impl; 
    std::unique_ptr<Impl> pImpl; 
};

