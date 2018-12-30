//
// Created by Dado on 2018-12-30.
//

#pragma once

#include <core/callback_dependency.h>

using CommandResouceCallbackFunction = std::function<void(const std::vector<std::string>&)>;

#define RBUILDER( BuilderName, _pre, _post, dataType, buildQueryType, version ) \
BuilderName : public ResourceBuilder { \
public: \
    using ResourceBuilder::ResourceBuilder; \
    BuilderName() {} \
    virtual ~BuilderName() {} \
    static uint64_t Version() { \
        return version; \
    } \
    const std::string prefix() const override { \
        return std::string(#_pre) + "/"; \
    } \
    std::string postMake( const std::string& _str ) const { \
        if ( usesNotExactQuery() ) return ""; \
        if ( _str.empty() ) return ""; \
        return "."  + _str; \
    } \
    void defPrePosfixes() { \
        postFix = postMake(#_post); \
    } \
    static bool usesNotExactQuery() { return buildQueryType == BuilderQueryType::NotExact; } \
    BuilderName( const std::string& _name ) : \
        ResourceBuilder( _name) { \
        defPrePosfixes(); \
    } \
    BuilderName& cc( CommandResouceCallbackFunction _ccf, const std::vector<std::string>& _params ) { \
        ccf = _ccf; \
        params = _params; \
        return *this; \
    } \
    bool makeImpl( DependencyMaker& _md, uint8_p&& _data, const DependencyStatus _status ) override; \
    bool build( DependencyMaker& _md ) override { \
        if ( !_md.exists(Name()) || usesNotExactQuery() ) { \
            auto dkey = prefix() + Name() + postFix; \
            readRemote<BuilderName, HttpQuery::dataType>( dkey, *this, _md ); \
            return true; \
        } \
        return false; \
    }

class BaseBuilder {
public:
    BaseBuilder() = default;
    explicit BaseBuilder( const std::string& _name ) : name( _name ) {}

    static const std::string typeName()  { return ""; }
    const std::string& Name() const { return name; }
    void Name( const std::string& _name ) { name = _name; }

    void clearTags() { tags.clear();}
    void addTag( const std::string& _tag ) { tags.emplace(_tag); }
    const std::set<std::string>& Tags() const { return tags; }
    void Tags( const std::set<std::string>& _tags ) { tags = _tags; }

    std::set<std::string> generateTags() const;

private:
    std::string name;
    std::set<std::string> tags;
};

class ResourceBuilder : public BaseBuilder {
    using BaseBuilder::BaseBuilder;
public:
    virtual const std::string prefix() const = 0;

    const std::string NameKey() const { return prefix() + Name(); }
    virtual bool build( DependencyMaker& _md ) = 0;
    bool rebuild( DependencyMaker& _md ) {
        bReloading = true;
        return build(_md);
    };

    virtual bool evaluateDirectBuild( DependencyMaker& _md ) {
        return false;
    }

    bool make( DependencyMaker& _md, uint8_p&& _data, const DependencyStatus _status ) {
        bool bSucc = makeImpl( _md, std::move(_data), _status );
        if ( bSucc && ccf ) {
            ccf( params );
        }
        return bSucc;
    }

    bool isReloading() const {
        return bReloading;
    }

protected:
    virtual bool makeImpl( DependencyMaker& _md, uint8_p&& _data, const DependencyStatus _status ) = 0;
protected:
    std::vector<std::string> params;
    CommandResouceCallbackFunction ccf = nullptr;
    std::string postFix;
    bool bReloading = false;
};

class ResourceBuilderObservable : public BaseBuilder {
    using BaseBuilder::BaseBuilder;
public:
    virtual bool make( uint8_p&& _data, const DependencyStatus _status ) = 0;
};

class DependantBuilder : public BaseBuilder {
    using BaseBuilder::BaseBuilder;
public:
    virtual void assemble( [[maybe_unused]] DependencyMaker& _md ) {};
    void build( DependencyMaker& _md ) {
        if ( validate() ) {
            createDependencyList( _md );
        }
    }

    ddContainer& deps()  {
        return mDeps;
    }

protected:
    virtual bool validate() const = 0;

    template< typename B, typename D>
    void addDependencies( std::shared_ptr<B> _builder, D& _md ) {
        if ( mDeps.empty() ) {
            _builder->assemble( _md );
        } else {
            g_deps.push_back( std::make_shared<DependencyChainMap>( _builder, _md ));
        }
    };

    bool depExistTest( const std::string& _dname, DependencyMaker& _md ) {
        return (!_dname.empty() && !_md.exists(_dname));
    }

    template <typename BD>
    void push_dep( const std::string& _dname ) {
        std::string preFixToAdd = BD::usesNotExactQuery() ? "" : BD{}.prefix();
        mDeps.push_back( preFixToAdd + _dname );
    }

    template <typename BD>
    void addDependency( const std::string& _dname, DependencyMaker& _md ) {
        if ( depExistTest(_dname, _md) ) {
            BD{ _dname }.build( _md );
            push_dep<BD>( _dname );
        }
    }

    template <typename BD>
    void addDependency( BD& _builder, DependencyMaker& _md ) {
        if ( depExistTest( _builder.Name(), _md) ) {

            if ( !_builder.evaluateDirectBuild( _md ) ) {
                _builder.build( _md );
                push_dep<BD>( _builder.Name() );
            }
        }
    }

    template <typename BD, typename T, typename S>
    void addDependency( const std::string& _dname, const T& _type, const S& _subtype, DependencyMaker& _md ) {
        if ( depExistTest(_dname, _md) ) {
            BD{ _dname, _type, _subtype }.build( _md );
            push_dep<BD>( _dname );
        }
    }

    virtual void createDependencyList( DependencyMaker& _md ) = 0;

protected:
    ddContainer mDeps;
};