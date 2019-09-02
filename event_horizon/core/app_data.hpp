//
// Created by Dado on 2019-08-02.
//

#pragma once

#include <core/name_policy.hpp>
#include <core/names.hpp>

JSONDATA( AppRenderSettings, shadowOverBurnCofficient, indoorSceneCoeff, shadowZFightCofficient )
    float shadowOverBurnCofficient = 2.0f;
    float indoorSceneCoeff = 1.0f;
    float shadowZFightCofficient = 0.002f;
};

struct AppEntities {
JSONSERIAL( AppEntities, key, value );
    std::string key;
    std::vector<std::string> value;
};

class AppData : public Keyable<> {
public:
    AppData() = default;
JSONSERIAL( AppData, renderSettings, mKey, entities );

    [[nodiscard]] size_t firstTierResourceCount() const {
        size_t ret = 0;
        for ( const auto& ent : entities ) {
            if ( ent.key == S::COLORS ) {
                ret += ent.value.size();
            }
            if ( ent.key == S::MATERIALS ) {
                ret += ent.value.size();
            }
            if ( ent.key == S::PROFILES ) {
                ret += ent.value.size();
            }
            if ( ent.key == S::IMAGES ) {
                ret += ent.value.size();
            }
            if ( ent.key == S::FONTS ) {
                ret += ent.value.size();
            }
        }
        return ret;
    };

    [[nodiscard]] size_t secondTierResourceCount() const {
        size_t ret = 0;
        for ( const auto& ent : entities ) {
            if ( ent.key == S::GEOMS ) {
                ret += ent.value.size();
            }
        }
        return ret;
    }

    [[nodiscard]] size_t customTierResourceCount() const {
        size_t ret = 0;
        for ( const auto& ent : entities ) {
            if ( ent.key != S::COLORS && ent.key != S::MATERIALS && ent.key != S::PROFILES && ent.key != S::IMAGES &&
                ent.key != S::FONTS && ent.key != S::GEOMS) {
                ret += ent.value.size();
            }
        }
        return ret;
    };

    [[nodiscard]] size_t totalResourceCount() const {
        return firstTierResourceCount() + secondTierResourceCount() + customTierResourceCount();
    }

    [[nodiscard]] std::vector<std::string> Geoms() {
        for ( const auto& ent : entities ) {
            if ( ent.key == S::GEOMS ) { return ent.value; }
        }
        return {};
    }
    [[nodiscard]] std::vector<std::string> Materials() {
        for ( const auto& ent : entities ) {
            if ( ent.key == S::MATERIALS ) { return ent.value; }
        }
        return {};
    }
    [[nodiscard]] std::vector<std::string> Profiles() {
        for ( const auto& ent : entities ) {
            if ( ent.key == S::PROFILES ) { return ent.value; }
        }
        return {};
    }
    [[nodiscard]] std::vector<std::string> RawImages(){
        for ( const auto& ent : entities ) {
            if ( ent.key == S::IMAGES ) { return ent.value; }
        }
        return {};
    }
    [[nodiscard]] std::vector<std::string> Fonts(){
        for ( const auto& ent : entities ) {
            if ( ent.key == S::FONTS ) { return ent.value; }
        }
        return {};
    }
    [[nodiscard]] std::vector<std::string> Custom( const std::string& _key ){
        for ( const auto& ent : entities ) {
            if ( ent.key == _key ) { return ent.value; }
        }
        return {};
    }

//
//    void addGeom         ( CResourceRef _value ) { geoms.emplace_back( _value);  }
//    void addMaterialColor( CResourceRef _value ) { colors.emplace_back( _value);  }
//    void addMaterial     ( CResourceRef _value ) { materials.emplace_back( _value);  }
//    void addProfile      ( CResourceRef _value ) { profiles.emplace_back( _value);  }
//    void addRawImage     ( CResourceRef _value ) { images.emplace_back( _value);  }
//    void addFont         ( CResourceRef _value ) { fonts.emplace_back( _value);  }

protected:
    AppRenderSettings        renderSettings;
    std::vector<AppEntities> entities;
};

