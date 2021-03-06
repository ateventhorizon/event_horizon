//
// Created by Dado on 2019-02-23.
//

#pragma once

#include <string>
#include <core/hashable.hpp>
#include <core/taggable.hpp>

template <typename T = std::string>
class HashTaggable : public virtual Taggable<T>, public virtual Hashable<> {
public:
    void calcHash( const SerializableContainer& _data ) override {
        this->removeTag( Hash() );
        Hashable::calcHash(_data);
        this->addTag( Hash() );
    }
};