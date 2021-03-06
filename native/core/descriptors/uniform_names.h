#pragma once

#include <string>
#include <array>
#include <core/string_util.h>

namespace UniformNames {

    const std::string mvpMatrix = "u_mvpMatrix";
    const std::string mvpMatrixDepth = "u_mvpMatrixDepth";
    const std::string mvpMatrixDepthBias = "u_depthBiasMVP";
    const std::string mvpShadowMap = "u_mvpShadowMap";
    const std::string modelMatrix = "u_modelMatrix";
    const std::string viewMatrix = "u_viewMatrix";
    const std::string screenSpaceMatrix = "u_screenSpaceMatrix";
    const std::string projMatrix = "u_projMatrix";
    const std::string sunDirection = "u_sunDirection";
    const std::string sunPosition = "u_sunPosition";
    const std::string sunRadiance = "u_sunRadiance";

    const std::string pointLightPos = "u_pointLightPos";
    const std::string pointLightDir = "u_pointLightDir";
    const std::string pointLightIntensity = "u_pointLightIntensity";
    const std::string pointLightAttenuation = "u_pointLightAttenuation";
    const std::string spotLightBeamDir = "u_spotLightBeamDir";
    const std::string outerCutOff = "u_outerCutOff";
    const std::string lightType = "u_lightType";
    const std::string numPointLights = "u_numPointLights";
    const std::string hdrExposures = "u_hdrExposures";
    const std::string shadowParameters = "u_shadowParameters";

    const std::string deltaAnimTime = "u_deltaAnimTime";

    const std::string eyeDir = "u_eyeDir";
    const std::string eyePos = "u_eyePos";
    const std::string nearFar = "u_nearFar";
    const std::string inverseMvMatrix = "u_inverseMvMatrix";
    const std::string prevMvpMatrix = "u_prevMvpMatrix";

    const std::string timeOfTheDay = "u_timeOfTheDay";
    const std::string sunHRDMult = "u_sunHDRMult";

    const std::string interpolateDayNightDelta = "u_interpolateDayNightDelta";
    const std::string colorTexture = "colorTexture";
    const std::string yTexture = "yTexture";
    const std::string uTexture = "uTexture";
    const std::string vTexture = "vTexture";
    const std::string shadowMapTexture = "shadowMapTexture";
    const std::string depthMapTexture = "depthMapTexture";
    const std::string normalMapTexture = "normalMapTexture";
    const std::string ssaoMapTexture = "ssaoMapTexture";
    const std::string lut3dTexture = "lut3dTexture";
    const std::string uiTexture = "uiTexture";
    const std::string colorFBTexture = "colorFBTexture";
    const std::string bloomTexture = "bloomTexture";
    const std::string aoTexture = "aoTexture";
    const std::string diffuseTexture = "diffuseTexture";
    const std::string normalTexture = "normalTexture";
    const std::string roughnessTexture = "roughnessTexture";
    const std::string metallicTexture = "metallicTexture";
    const std::string heightTexture = "heightTexture";
    const std::string opacityTexture = "opacityTexture";
    const std::string translucencyTexture = "translucencyTexture";
    const std::string emissionTexture = "emissionTexture";
    const std::string lightmapTexture = "lightmapTexture";
    const std::string noise4x4Texture = "noise4x4Texture";
    const std::string cubeMapTexture = "cubeMapTexture";
    const std::string ibl_irradianceMap = "ibl_irradianceMap";
    const std::string ibl_specularMap = "ibl_specularMap";
    const std::string ibl_brdfLUTMap = "ibl_brdfLUTMap";
    const std::string reflectionTexture = "reflectionTexture";
    const std::string alpha = "alpha";
    const std::string ui_z = "ui_z";
    const std::string clip_plane = "clip_plane";
    const std::string screenSizeInv = "screenSizeInv";
    const std::string reflectionScreenSizeInv = "reflectionScreenSizeInv";
    const std::string ssaoBlendFactor = "ssaoBlendFactor";

    const std::string diffuseColor = "diffuseColor";
    const std::string ambient = "ambient";
    const std::string incandescenceColor = "incandescenceColor";
    const std::string incandescenceFactor = "incandescenceFactor";
    const std::string transparency = "transparency";
    const std::string opacity = "opacity";
    const std::string diffuse = "diffuse";

    const std::string hemisphereKernelSize = "u_hemisphereSampleKernelSize";
    const std::string hemisphereKernel = "u_hemisphereSampleKernel";
    const std::string ssaoParameters = "u_ssaoParameters";
    const std::string shLightCoeffs = "shLightCoeffs";

    const std::string specularColor = "specularColor";
    const std::string cosinePower = "cosinePower";
    const std::string specular = "specular";
    const std::string reflectivity = "reflectivity";
    const std::string reflectedColor = "reflectedColor";
    const std::string metallic = "metallicV";
    const std::string roughness = "roughnessV";
    const std::string ao = "aoV";
};

namespace MQSettings {
    const static std::string Low = "_lowqDD256";
    const static std::string Medium = "";
    const static std::string Hi = "_hiqDD2048";
    const static std::string UltraHi = "_ultrahiDD4k";
};

namespace MPBRTextures {

    const static std::string convolution = "convolution";
    const static std::string specular_prefilter = "specular_prefilter";
    const static std::string ibl_brdf = "ibl_brdf";

    static const std::string basecolorString = "basecolor";
    static const std::string base_colorString = "base_color";
    static const std::string diffString = "diff";
    static const std::string colString = "_col";
    static const std::string albedoString = "_albedo";
    static const std::string heightString = "height";
    static const std::string dispString = "_disp";
    static const std::string metallicString = "metallic";
    static const std::string rglossString = "rgloss";
    static const std::string roughnessString = "roughness";
    static const std::string rghString = "_rgh";
    static const std::string normalString = "normal";
    static const std::string normalDxString = "normal_dx";
    static const std::string nrmString = "_nrm";
    static const std::string ambientOcclusionString = "ambient_occlusion";
    static const std::string aoString = "_ao";
    static const std::string opacityString = "opacity";
    static const std::string translucencyString = "translucency";
    static const std::string emissionString = "emission";

    const static std::vector<std::string> g_pbrNames{ "_basecolor", "_base_color", "_normal", "_ambient_occlusion",
                                                      "_roughness", "_emission",
                                                      "_metallic", "_height", "_opacity", "_translucency" };

    static inline auto SBSARTextureOutputs() {
        return std::array<std::string, 6>{
                MPBRTextures::basecolorString,
                MPBRTextures::metallicString,
                MPBRTextures::ambientOcclusionString,
                MPBRTextures::roughnessString,
                MPBRTextures::heightString,
                MPBRTextures::normalString
        };
    }

    static inline const std::vector<std::string>& Names() {
        return g_pbrNames;
    }

    static inline std::string sanitizeTextureOutput( const std::string& input ) {
        auto value = toLower( input );
        replaceAllStrings( value, "-", "_" );
        return value;
    }

    static inline bool isBaseColorTexture( const std::string& _value ) {
        auto value = sanitizeTextureOutput( _value );
        if ( value.find( basecolorString ) != std::string::npos ) return true;
        if ( value.find( base_colorString ) != std::string::npos ) return true;
        if ( value.find( diffString ) != std::string::npos ) return true;
        if ( value.find( colString ) != std::string::npos ) return true;
        return value.find( albedoString ) != std::string::npos;
    }

    static inline bool isRoughnessTexture( const std::string& _value ) {
        auto value = sanitizeTextureOutput( _value );
        if ( value.find( roughnessString ) != std::string::npos ) return true;
        return value.find( rghString ) != std::string::npos;
    }

    static inline bool isMetallicTexture( const std::string& _value ) {
        auto value = sanitizeTextureOutput( _value );
        if ( value.find( metallicString ) != std::string::npos ) return true;
        return value.find( rglossString ) != std::string::npos;
    }

    static inline bool isHeightTexture( const std::string& _value ) {
        auto value = sanitizeTextureOutput( _value );
        if ( value.find( heightString ) != std::string::npos ) return true;
        return value.find( dispString ) != std::string::npos;
    }

    static inline bool isNormalTexture( const std::string& _value ) {
        auto value = sanitizeTextureOutput( _value );
        if ( value.find( normalString ) != std::string::npos ) return true;
        if ( value.find( nrmString ) != std::string::npos ) return true;
        return value.find( normalDxString ) != std::string::npos;
    }

    static inline bool isAOTexture( const std::string& _value ) {
        auto value = sanitizeTextureOutput( _value );
        if ( value.find( ambientOcclusionString ) != std::string::npos ) return true;
        return value.find( aoString ) != std::string::npos;
    }

    static inline bool isOpacityTexture( const std::string& _value ) {
        auto value = sanitizeTextureOutput( _value );
        return value.find( opacityString ) != std::string::npos;
    }

    static inline bool isTranslucencyTexture( const std::string& _value ) {
        auto value = sanitizeTextureOutput( _value );
        return value.find( translucencyString ) != std::string::npos;
    }

    static inline bool isEmissiveTexture( const std::string& _value ) {
        auto value = sanitizeTextureOutput( _value );
        return value.find( emissionString ) != std::string::npos;
    }

    static inline const std::string findTextureInString( const std::string& _value ) {
        auto value = sanitizeTextureOutput( _value );
        if ( isBaseColorTexture( value )) return basecolorString;
        if ( isNormalTexture( value )) return normalString;
        if ( isRoughnessTexture( value )) return roughnessString;
        if ( isMetallicTexture( value )) return metallicString;
        if ( isHeightTexture( value )) return heightString;
        if ( isAOTexture( value )) return ambientOcclusionString;
        if ( isOpacityTexture( value )) return opacityString;
        if ( isTranslucencyTexture( value )) return translucencyString;
        if ( isEmissiveTexture( value )) return emissionString;

        return "";
    }

    static inline const std::string mapToTextureUniform( const std::string& _value ) {
        auto value = sanitizeTextureOutput( _value );
        if ( isBaseColorTexture( value )) return UniformNames::diffuseTexture;
        if ( isNormalTexture( value )) return UniformNames::normalTexture;
        if ( isRoughnessTexture( value )) return UniformNames::roughnessTexture;
        if ( isMetallicTexture( _value )) return UniformNames::metallicTexture;
        if ( isHeightTexture( value )) return UniformNames::heightTexture;
        if ( isAOTexture( value )) return UniformNames::aoTexture;
        if ( isOpacityTexture( value )) return UniformNames::opacityTexture;
        if ( isTranslucencyTexture( value )) return UniformNames::translucencyTexture;
        if ( isEmissiveTexture( value )) return UniformNames::emissionTexture;

        ASSERTV( 0, "Couldn't map PBR texture %s", _value.c_str());
        return "";
    }

}
