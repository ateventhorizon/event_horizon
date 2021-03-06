#include "material.h"
#include <core/descriptors/uniform_names.h>

Material::Material( const std::string& _type ) {
    makeValues(_type);
}

Material::Material( const std::string& _type, const std::string& _key ) {
    Key(_key);
    makeValues(_type);
}

Material::Material( std::shared_ptr<HeterogeneousMap> _values ) {
    values = std::move( _values );
}

Material::Material( const Color4f& _color, const std::string& _type ) {
    makeValues(_type);
    Values()->assign( UniformNames::diffuseColor, _color );
}

void Material::makeValues( const std::string& _type ) {
    values = std::make_shared<HeterogeneousMap>(_type);
}

//static inline bool isShaderStreammable( const std::string& _sn ) {
//    return ( _sn == S::YUV_GREENSCREEN || _sn == S::YUV );
//}

float Material::getMetallicValue() const {
    auto ret = values->get<float>( UniformNames::metallic );
    return ret ? *ret : 1.0f;
}

void Material::setMetallicValue( float _metallicValue ) {
    values->assign( UniformNames::metallic, _metallicValue );
}

float Material::getRoughnessValue() const {
    auto ret = values->get<float>( UniformNames::roughness );
    return ret ? *ret : 1.0f;
}

void Material::setRoughnessValue( float _roughnessValue ) {
    values->assign( UniformNames::roughness, _roughnessValue );
}

float Material::getAoValue() const {
    auto ret = values->get<float>( UniformNames::ao );
    return ret ? *ret : 1.0f;
}

void Material::setAoValue( float _aoValue ) {
    values->assign( UniformNames::ao, _aoValue );
}

float Material::getOpacity() const {
    auto ret = values->get<float>( UniformNames::opacity );
    return ret ? *ret : 1.0f;
}

void Material::setOpacity( float _opacityValue ) {
    values->assign( UniformNames::opacity, _opacityValue );
}

float Material::translucency() const {
//    if ( shaderName == S::YUV_GREENSCREEN ) return 0.5f;
    return getOpacity();
}

const std::shared_ptr<HeterogeneousMap> Material::Values() const {
    return values;
}

std::shared_ptr<HeterogeneousMap> Material::Values() {
    return values;
}

void Material::Values( std::shared_ptr<HeterogeneousMap> _values ) {
    Material::values = std::move( _values );
}

V3f Material::getDiffuseColor() const {
    auto ret = values->get<V3f>( UniformNames::diffuseColor );
    return ret ? *ret : V3fc::ONE;
}

void Material::setDiffuseColor( const V3f& _value ) {
    values->assign( UniformNames::diffuseColor, _value );
}

ResourceRef Material::getTexture( const std::string& _tt ) const {
    auto ret = values->get<std::string>( _tt );
    return ret ? *ret : "";
}

ResourceRef Material::getDiffuseTexture() const {
    auto ret = values->get<std::string>( UniformNames::diffuseTexture );
    return ret ? *ret : "";
}

ResourceRef Material::getNormalTexture() const {
    auto ret = values->get<std::string>( UniformNames::normalTexture );
    return ret ? *ret : "";
}

ResourceRef Material::getMetallicTexture() const {
    auto ret = values->get<std::string>( UniformNames::metallicTexture );
    return ret ? *ret : "";
}

ResourceRef Material::getRoughnessTexture() const {
    auto ret = values->get<std::string>( UniformNames::roughnessTexture );
    return ret ? *ret : "";
}

ResourceRef Material::getAOTexture() const {
    auto ret = values->get<std::string>( UniformNames::aoTexture );
    return ret ? *ret : "";
}

ResourceRef Material::getHeightTexture() const {
    auto ret = values->get<std::string>( UniformNames::heightTexture );
    return ret ? *ret : "";
}

ResourceRef Material::getOpacityTexture() const {
    auto ret = values->get<std::string>( UniformNames::opacityTexture );
    return ret ? *ret : "";
}

ResourceRef Material::getTranslucencyTexture() const {
    auto ret = values->get<std::string>( UniformNames::translucencyTexture );
    return ret ? *ret : "";
}

float Material::getTexelRatio() const {
    return texelRatio;
}

void Material::setTexelRatio( float _value ) {
    texelRatio = _value;
}
