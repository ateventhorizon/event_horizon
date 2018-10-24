//
//  serialization.hpp
//  sixthview
//
//  Created by Dado on 01/10/2017.
//
//

#pragma once

#include "megawriter_fs.hpp"
#include "megareader_fs.hpp"
#include "string_util.h"

//specialize a type for all of the STL containers.
namespace is_stl_container_impl {
template <typename T>       struct is_stl_container :std::false_type {};
template <typename... Args> struct is_stl_container<std::vector            <Args...>> :std::true_type {};
}

//type trait to utilize the implementation type traits as well as decay the type
template <typename T> struct is_stl_container {
	static constexpr bool const value = is_stl_container_impl::is_stl_container<std::decay_t<T>>::value;
};

template <typename T>
static void sseserialize( MegaWriter* visitor, const std::string& name, const T& value ) {
	visitor->serialize( name.c_str(), value );
}

template <typename V, typename T>
static void visit( const std::string& name, const T& value ) {
	V{}.visit( name.c_str(), value );
}

template <typename T>
static void sdeeserialize( const MegaReader& visitor, const std::string& name, T& value ) {
	visitor.deserialize( name.c_str(), value );
}

#define JSONDATA(CLASSNAME,...) \
	struct CLASSNAME { \
	CLASSNAME() {} \
	CLASSNAME( const MegaReader& reader ) { deserialize( reader ); } \
    template<typename TV> \
	void visit() const { traverseWithHelper<TV>( #__VA_ARGS__,__VA_ARGS__ ); } \
	inline void serialize( MegaWriter* visitor ) const { visitor->StartObject(); serializeWithHelper(visitor, #__VA_ARGS__, __VA_ARGS__ ); visitor->EndObject(); } \
	inline void deserialize( const MegaReader& visitor ) { deserializeWithHelper(visitor, #__VA_ARGS__, __VA_ARGS__ ); } 

#define JSONDATA_H(CLASSNAME, PARENTCLASS,...) \
	struct CLASSNAME : public PARENTCLASS { \
	CLASSNAME() {} \
	virtual ~CLASSNAME() {} \
	CLASSNAME( const MegaReader& reader ) { deserialize( reader ); } \
	template<typename TV> \
	void visit() const { traverseWithHelper<TV>( #__VA_ARGS__,__VA_ARGS__ ); } \
	inline void serialize( MegaWriter* visitor ) const { visitor->StartObject(); serializeWithHelper(visitor, #__VA_ARGS__, __VA_ARGS__ ); visitor->EndObject(); } \
	inline void deserialize( const MegaReader& visitor ) { deserializeWithHelper(visitor, #__VA_ARGS__, __VA_ARGS__ ); } 

#define JSONDATA_R_H(CLASSNAME, PARENTCLASS,...) \
	struct CLASSNAME : public PARENTCLASS { \
	CLASSNAME() {} \
	virtual ~CLASSNAME() {} \
	CLASSNAME( uint8_p& _data ) { \
		rapidjson::Document document; \
		document.Parse<rapidjson::kParseStopWhenDoneFlag>( cbToString(std::move(_data)).c_str() ); \
		MegaReader reader( document ); \
		if ( !reader.isEmpty() ) { deserialize( reader );} \
	} \
	CLASSNAME( const std::string& _name ) { load( _name ); } \
	CLASSNAME( const std::string& _name, const std::string& _key ) { load( _name, _key ); } \
	CLASSNAME( const MegaReader& reader ) { deserialize( reader ); bIsLoaded = true; } \
	template<typename TV> \
	void visit() const { traverseWithHelper<TV>( #__VA_ARGS__,__VA_ARGS__ ); } \
	void save() const { MegaWriterFS<CLASSNAME>( #CLASSNAME, *this ); } \
	void load( const std::string& _name, const std::string& _key = "name" ) { readFS<CLASSNAME>( #CLASSNAME, _name, _key, *this ); } \
	inline void serialize( MegaWriter* visitor ) const { visitor->StartObject(); serializeWithHelper(visitor, #__VA_ARGS__, __VA_ARGS__ ); visitor->EndObject(); } \
	inline void deserialize( const MegaReader& visitor ) { deserializeWithHelper(visitor, #__VA_ARGS__, __VA_ARGS__ ); } \
	bool bIsLoaded = false; \
    static bool usesNotExactQuery() { return false; }; \
    static std::string typeName() { return #CLASSNAME; }

#define JSONDATA_R(CLASSNAME,...) \
	struct CLASSNAME { \
	CLASSNAME() {} \
	CLASSNAME( const std::string& _name ) { load( _name ); } \
	CLASSNAME( uint8_p& _data ) { \
		rapidjson::Document document; \
		document.Parse<rapidjson::kParseStopWhenDoneFlag>( cbToString(std::move(_data)).c_str() ); \
		MegaReader reader( document ); \
		if ( !reader.isEmpty() ) { \
			if (reader.isArray() ) { \
				deserialize( reader.at( static_cast<rapidjson::SizeType>(0)) ); \
			} else { \
				deserialize( reader ); \
			} \
		} \
	} \
	CLASSNAME( const std::string& _name, const std::string& _key ) { load( _name, _key ); } \
	CLASSNAME( const MegaReader& reader ) { deserialize( reader ); bIsLoaded = true; } \
	template<typename TV> \
	void visit() const { traverseWithHelper<TV>( #__VA_ARGS__,__VA_ARGS__ ); } \
	void save() const { MegaWriterFS<CLASSNAME>( #CLASSNAME, *this ); } \
	void load( const std::string& _name, const std::string& _key = "name" ) { readFS<CLASSNAME>( #CLASSNAME, _name, _key, *this ); } \
	inline void serialize( MegaWriter* visitor ) const { visitor->StartObject(); serializeWithHelper(visitor, #__VA_ARGS__, __VA_ARGS__ ); visitor->EndObject(); } \
	inline void deserialize( const MegaReader& visitor ) { deserializeWithHelper(visitor, #__VA_ARGS__, __VA_ARGS__ ); } \
	bool bIsLoaded = false; \
    static bool usesNotExactQuery() { return false; }; \
	static std::string typeName() { return #CLASSNAME; }

template<typename TVisitor,typename ...T>
inline int traverseWithHelper( std::string memberNames, T &...v ) {
	//split the names
	std::vector<std::string> names = split( memberNames, ',' );

	int i{};

	//for c++17 try just
	//((void)VisitWith(std::forward<T>(v)), ...);
	int x[] = { 0, ( (void)visit<TVisitor>( names[i++], v ), i )... };

	return x[names.size() - 1]; //return last one
}

template<typename TVisitor, typename ...T> inline int serializeWithHelper( TVisitor* visitor, std::string memberNames, T &...v ) {
	//split the names
	std::vector<std::string> names = split( memberNames, ',' );

	int i{};

	//for c++17 try just
	//((void)VisitWith(std::forward<T>(v)), ...);
	int x[] = { 0, ( (void)sseserialize( visitor, names[i++], v ), i )... };

	return x[names.size() - 1]; //return last one
}

template<typename TVisitor, typename ...T> inline int deserializeWithHelper( const TVisitor& visitor, std::string memberNames, T &...v ) {
	//split the names
	std::vector<std::string> names = split( memberNames, ',' );

	int i{};

	//for c++17 try just
	//((void)VisitWith(std::forward<T>(v)), ...);
	int x[] = { 0, ( (void)sdeeserialize( visitor, names[i++], v ), i )... };

	return x[names.size() - 1]; //return last one
}