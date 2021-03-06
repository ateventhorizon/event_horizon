//
// Created by Dado on 2019-02-21.
//

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <core/htypes_shared.hpp>

using SerializeDependencyHash      = std::string;

struct SerializeHeader {
    SerializeDependencyHash hash;
    uint64_t                version = 0;
    std::string             entityGroup;
    uint64_t                size = 0;
};

using SerializationDependencyMap   = std::map<SerializeDependencyHash, SerializableContainer>;
