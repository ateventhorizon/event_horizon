#include <utility>

//
// Created by Dado on 2019-02-16.
//

#include "material.h"
#include <core/megawriter.hpp>

//static inline bool isShaderStreammable( const std::string& _sn ) {
//    return ( _sn == S::YUV_GREENSCREEN || _sn == S::YUV );
//}

Material::Material() {
    values = std::make_shared<HeterogeneousMap>();
}

Material::Material( std::string type, std::vector<ResourceRef> _images,
                    std::shared_ptr<HeterogeneousMap> values ) :
                    type( std::move( type )), images( std::move( _images )), values( std::move( values )) {}

void Material::bufferDecode( const unsigned char *_buffer, size_t _length ) {
    std::string strBuff{ _buffer, _buffer + _length};
    rapidjson::Document document;
    document.Parse<rapidjson::kParseStopWhenDoneFlag>( strBuff.c_str() );
    MegaReader reader( document );
    values = std::make_shared<HeterogeneousMap>();
    deserialize( reader );
//    reader.deserialize( "Type", type );
//    reader.deserialize( "Images", images );
//    values->deserialize( reader );

//    auto reader = std::make_shared<DeserializeBin>(_buffer, _length);
//
//    reader->read(type);
//    reader->read(images);
//    values = std::make_shared<HeterogeneousMap>();
//    values->deserializeImpl( reader );
}

//Material::Material( const Material& _source ) {
//    values = std::make_shared<HeterogeneousMap>(_source.Values());
//}

//Material& Material::t( const std::string& _tn ) {
//    values->assign( UniformNames::colorTexture, _tn );
//    return *this;
//}
//
//Material& Material::c( const Color4f& _col ) {
//    values->assign( UniformNames::opacity, _col.w() );
//    values->assign( UniformNames::diffuseColor, _col.xyz() );
//    return *this;
//}

void Material::resolveDynamicConstants() {
//    ### MAT Fix up this
//    values->visitTexturesWithKey( [&]( std::string& u, const std::string& _key ) {
//        if ( _key == UniformNames::yTexture) {
//            u = Name() + "_y";
//        } else if ( _key == UniformNames::uTexture) {
//            u = Name() + "_u";
//        } else if ( _key == UniformNames::vTexture) {
//            u = Name() + "_v";
//        }
//    });
}

//std::string Material::PBRName( const std::string& _type ) const {
//    return Name() + "_" + _type;
//}
//
//const std::string Material::getBaseColor() const {
//    return PBRName(MPBRTextures::basecolorString);
//}
//
//const std::string Material::getNormal() const {
//    return PBRName(MPBRTextures::normalString);
//}
//
//const std::string Material::getAmbientOcclusion() const {
//    return PBRName(MPBRTextures::ambientOcclusionString);
//}
//
//const std::string Material::getRoughness() const {
//    return PBRName(MPBRTextures::roughnessString);
//}
//
//const std::string Material::getMetallic() const {
//    return PBRName(MPBRTextures::metallicString);
//}
//
//const std::string Material::getHeight() const {
//    return PBRName(MPBRTextures::heightString);
//}

float Material::getMetallicValue() const {
    float ret;
    values->get( UniformNames::metallic, ret );
    return ret;
}

void Material::setMetallicValue( float _metallicValue ) {
    values->assign( UniformNames::metallic, _metallicValue );
}

float Material::getRoughnessValue() const {
    float ret;
    values->get( UniformNames::roughness, ret );
    return ret;
}

void Material::setRoughnessValue( float _roughnessValue ) {
    values->assign( UniformNames::roughness, _roughnessValue );
}

float Material::getAoValue() const {
    float ret;
    values->get( UniformNames::ao, ret );
    return ret;
}

void Material::setAoValue( float _aoValue ) {
    values->assign( UniformNames::ao, _aoValue );
}

float Material::getOpacity() const {
    float ret;
    values->get( UniformNames::opacity, ret );
    return ret;
}

void Material::setOpacity( float _opacityValue ) {
    values->assign( UniformNames::opacity, _opacityValue );
}

//const MaterialProperties& Material::getProperties() const {
//    return properties;
//}
//
//void Material::setProperties( const MaterialProperties& properties ) {
//    Material::properties = properties;
//}

//void Material::serializeInternal( std::shared_ptr<SerializeBin> writer ) const {
//    HeterogeneousMap::serializeImpl(writer);
//    writer->write( buffers );
//    properties.serialize(writer);
//    writer->write(shaderName);
//}
//
//void Material::deserializeInternal(std::shared_ptr<DeserializeBin> reader) {
//    HeterogeneousMap::deserializeImpl(reader);
//    reader->read( buffers );
//    properties.deserialize(reader);
//    reader->read(shaderName);
//}

// ### MAT REMOVE CLONE, it's shit
void Material::clone( const Material& _source ) {
    values->clone( *_source.values.get() );
}

//Material& Material::buffer( const std::string& _bname, uint8_p&& _data, const std::string& _uniformName ) {
//    if ( _data.second > 0 ) {
//        buffers.emplace( std::make_pair(_bname, std::move(_data)) );
//        values->assign( _uniformName, _bname );
//    }
//    return *this;
//}
//
//Material& Material::buffer( const std::string& _bname, const ucchar_p& _data, const std::string& _uniformName ) {
//    return buffer( _bname, ucchar_pTouint8_p(_data), _uniformName );
//}

//KnownBufferMap Material::knownBuffers() const {
//    KnownBufferMap ret;
//
//    ret.emplace( getBaseColor(), UniformNames::diffuseTexture );
//    ret.emplace( getNormal(), UniformNames::normalTexture );
//    ret.emplace( getAmbientOcclusion(), UniformNames::aoTexture );
//    ret.emplace( getRoughness(), UniformNames::roughnessTexture );
//    ret.emplace( getMetallic(), UniformNames::metallicTexture );
//    ret.emplace( getHeight(), UniformNames::heightTexture );
//
//    return ret;
//}

//void Material::tarBuffers( const SerializableContainer& _bufferTarFiles ) {
//    if ( !_bufferTarFiles.empty() ) {
//        auto kbs = knownBuffers();
//        auto files = tarUtil::untar( _bufferTarFiles );
//        for ( const auto& fi  : files ) {
//            auto finame = getFileNameOnly(fi.name);
//            auto lUniformName = finame;
//            if ( auto unk = kbs.find( finame ); unk != kbs.end() ) {
//                lUniformName = unk->second;
//            }
//            buffer( finame, fi.dataPtr, lUniformName );
//            assign(lUniformName, finame );
//        }
//    }
//}

//void Material::Buffers( MaterialImageCallback imageCallback ) {
//    if ( !buffers.empty() ) {
//        auto kbs = knownBuffers();
//        for ( const auto& [k,v] : buffers ) {
//            imageCallback( k, { v.first.get(), v.second } );
//        }
//    }
//}
//
//const MaterialImageBuffers& Material::Buffers() const {
//    return buffers;
//}

//bool Material::isStreammable() const {
//    return properties.isStreaming;
//}

//std::string Material::generateThumbnail() const {
//    auto thumb = std::make_unique<std::string>();
//
//    std::unique_ptr<uint8_t[]> lthumb;
//    int thumbSize = 64;
//    int oc = 3;
//    int obpp = 8;
//    if ( const auto& it = buffers.find(getBaseColor()); it != buffers.end() ) {
//        lthumb = imageUtil::resize( it->second.first.get(), it->second.second, thumbSize, thumbSize, oc, obpp );
//    } else {
//        V3f defColor = Color3f::ONE;
//        if ( hasVector3f(UniformNames::diffuseColor) ) {
//            get(UniformNames::diffuseColor, defColor);
//        }
//        lthumb = imageUtil::zeroImage3( Vector4f{defColor}.RGBATOI(), 1, 1 );
//    }
//    stbi_write_png_to_func( imageUtil::resizeCallbackb64, reinterpret_cast<void*>(thumb.get()),
//                            thumbSize, thumbSize, oc, lthumb.get(), thumbSize*oc*(obpp/8) );
//
//    return std::string{ thumb->data(), thumb->size() };
//}

float Material::translucency() const {
//    if ( shaderName == S::YUV_GREENSCREEN ) return 0.5f;
    return getOpacity();
}

Material Material::WHITE_PBR() {
    return Material{};
}

