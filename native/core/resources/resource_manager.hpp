//
// Created by Dado on 2019-03-20.
//

#pragma once

#include <unordered_set>
#include <boost/signals2.hpp>
#include <core/names.hpp>
#include <core/resources/resource_utils.hpp>
#include <core/resources/entity_factory.hpp>

using ResourceMapperDict = std::unordered_map<std::string, ResourceRef>;
using DependencyDict = std::unordered_map<std::string, std::unordered_map<std::string, bool>>;
using ResourceDependencyMap = std::unordered_map<std::string, std::string>;

template<typename T, typename C = ResourceManagerContainer<T>, typename RV = ResourceVersioning<T>>
class ResourceManager {
public:
    std::shared_ptr<T> exists( const std::string& _key ) const {
        if ( auto res = resourcesMapper.find( _key ); res != resourcesMapper.end() ) {
            return resources.at(res->second);
        }
        return nullptr;
    };

    ResourceRef hash( const std::string& _key ) const {
        if ( auto res = resourcesMapper.find( _key ); res != resourcesMapper.end() ) {
            return res->second;
        }
        return "";
    };

    std::shared_ptr<T> hashExists( const std::string& _hash ) const {
        if ( auto res = resources.find( _hash ); res != resources.end() ) {
            return res->second;
        }
        return nullptr;
    };

    std::shared_ptr<T> clone( const std::string& _hash ) const {
        if ( auto res = resources.find( _hash ); res != resources.end() ) {
            return EF::clone( res->second );
        }
        return nullptr;
    }

    void clear() {
        resources.erase(resources.begin(), resources.end());
        resourcesMapper.erase(resourcesMapper.begin(), resourcesMapper.end());
        dependencyDict.erase(dependencyDict.begin(), dependencyDict.end());
    }

    void update() {
        for ( const auto& s : signalAddElements ) {
            addSignal(s);
        }
        signalAddElements.clear();
    }

    void addToSignal( SignalsDeferredContainer<T>& _container, const ResourceTransfer<T>& _elem ) {
        _container.emplace(_elem);
    }

    void add( std::shared_ptr<T> _elem, const std::string& _name,
              const std::string& _hash, AddResourcePolicy _arp, const std::string& _aliasKey = "",
              const std::string& _key = {},
              HttpResourceCB _ccf = nullptr ) {
        if ( _arp == AddResourcePolicy::Deferred ) {
            addDeferred( _elem, _name, _hash, _aliasKey, _key, _ccf );
        } else {
            addImmediate( _elem, _name, _hash, _aliasKey, _key );
        }
    }

    void update( CResourceRef _hash ) {
        updateSignal( { resources[_hash], _hash } );
    }

    void addImmediate( std::shared_ptr<T> _elem, const std::string& _name,
                       const std::string& _hash, const std::string& _aliasKey = {}, const std::string& _key = {} ) {
        add( _elem, _name, _hash, _aliasKey, _key );
        addSignal( { _elem, _hash, {_hash, _name, _aliasKey, _key} } );
    }

    void addDeferred( std::shared_ptr<T> _elem, const std::string& _name,
                      const ResourceRef& _hash, const std::string& _aliasKey = "", const std::string& _key = "",
                      HttpResourceCB _ccf = nullptr) {
        add( _elem, _name, _hash, _aliasKey, _key );
        addToSignal( signalAddElements, { _elem, _hash, { _hash, _name, _aliasKey, _key }, _ccf } );
    }

    std::shared_ptr<T> getFromHash( const std::string& _hash ) {
        return resources[_hash];
    }

    ResourceRef getHash( const std::string& _key ) {
        if ( auto res = resourcesMapper.find(_key); res != resourcesMapper.end() ) {
            return res->second;
        }
//        LOGRS("Resource " << _key << " unmapped, returning empty hash");
        return {};
    }

    std::tuple<ResourceRef, T*> getHashAndPointer( const std::string& _key ) {
        if ( auto res = resourcesMapper.find(_key); res != resourcesMapper.end() ) {
            return std::make_tuple(res->second, resources[res->second].get());
        }
        return std::make_tuple(std::string{}, nullptr);
    }

    std::shared_ptr<T> get( const std::string& _key ) {
        if ( auto res = resourcesMapper.find(_key); res != resourcesMapper.end() ) {
            return resources[res->second];
        }
//        LOGRS("Resource " << _key << " unmapped and mamanger empty, returning null");
        return nullptr;
    }

    void addKey( const std::string& _key, const std::string& _hash ) {
        resourcesMapper[_key] = _hash;
    }

    std::vector<std::string> getNames( const std::string& _key ) {
        std::vector<std::string> ret{};
        for ( const auto& [k,v] : resourcesMapper ) {
            if ( v == _key && k != _key ) ret.emplace_back( k );
        }
        return ret;
    }

    void add( std::function<void (const ResourceTransfer<T>&)> _slot ) {
        addSignal.connect( _slot );
    }

    void update( std::function<void (const ResourceTransfer<T>&)> _slot ) {
        updateSignal.connect( _slot );
    }

    std::vector<std::shared_ptr<T>> list() const {
        std::vector<std::shared_ptr<T>> ret;
        for ( const auto& [k, v] : Resources() ) {
            ret.push_back(v);
        }
        return ret;
    }

    const C& listResources() const {
        return Resources();
    }

    void addDependencyHook( const std::string& _k, const std::string& _v ) {
        dependencyDict[_k].emplace(_v, false);
    }

    void tagDependencyLoaded( const std::string& _k, const std::string& _v ) {
        if ( auto it = dependencyDict.find(_k); it != dependencyDict.end() ) {
            it->second[_v] = true;
        }
    }

    bool checkDependencyCompleted( const std::string& _k, ResourceDependencyMap& _ret ) {
        if ( auto it = dependencyDict.find(_k); it != dependencyDict.end() ) {
            for ( const auto& [k,v] : it->second ) {
                if ( !v ) return false;
                _ret.emplace( k, hash(k) );
            }
            // returns true, after deleting the entry
            dependencyDict.erase(_k);
            return true;
        }
        return false;
    }

//    void publish( std::shared_ptr<T> elem, const std::string& _name ) {
//        auto fname = _name.empty() ? S::makeImaginary() : _name;
//        Http::post( Url{ HttpFilePrefix::entities + RV::Prefix() + "/" + fname}, elem->serialize() );
//    }

protected:
    void add( std::shared_ptr<T> _elem, const std::string& _name,
              const std::string& _hash, const std::string& _aliasKey = {}, const std::string& _key = {} ) {
        resources[_hash] = _elem;
        resourcesMapper[_hash] = _hash;
        if ( !_name.empty() ) resourcesMapper[_name] = _hash;
        if ( !_aliasKey.empty() ) resourcesMapper[_aliasKey] = _hash;
        if ( !_key.empty() ) resourcesMapper[_key] = _hash;
    }

    C& Resources() {
        return resources;
    }
    const C& Resources() const {
        return resources;
    }
private:
    C resources;
    ResourceMapperDict resourcesMapper;
    DependencyDict dependencyDict;

    SignalsDeferredContainer<T> signalAddElements;
    boost::signals2::signal<void(const ResourceTransfer<T>&)> addSignal;

    SignalsDeferredContainer<T> signalUpdateElements;
    boost::signals2::signal<void(const ResourceTransfer<T>&)> updateSignal;

    SignalsDeferredContainer<T> signalRemoveElement;
    boost::signals2::signal<void(const ResourceTransfer<T>&)> removeSignal;

};
