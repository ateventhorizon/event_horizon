#define TINYGLTF_IMPLEMENTATION

#include "gltf2.h"
#include <iostream>
#include <core/node.hpp>
#include <core/raw_image.h>
#include <core/math/quaternion.h>
#include <core/raw_image.h>
#include <core/image_util.h>
#include <core/names.hpp>
#include <core/heterogeneous_map.hpp>
#include <core/resources/material.h>
#include <core/geom.hpp>
#include <core/file_manager.h>
#include <core/v_data.hpp>
#include <core/resources/entity_factory.hpp>
#include <core/resources/resource_builder.hpp>
#include <core/recursive_transformation.hpp>
#include <poly/scene_graph.h>

namespace GLTF2Service {

    GLTF2Service::IntermediateMaterial::IntermediateMaterial() {
        values = std::make_shared<HeterogeneousMap>(S::SH);
    }

    unsigned int accessorTypeToNumberOfComponent( int ty ) {
        if ( ty == TINYGLTF_TYPE_SCALAR) {
            return 1;
        } else if ( ty == TINYGLTF_TYPE_VECTOR) {
            return 1;
        } else if ( ty == TINYGLTF_TYPE_VEC2) {
            return 2;
        } else if ( ty == TINYGLTF_TYPE_VEC3) {
            return 3;
        } else if ( ty == TINYGLTF_TYPE_VEC4) {
            return 4;
        } else if ( ty == TINYGLTF_TYPE_MATRIX) {
            return 1;
        } else if ( ty == TINYGLTF_TYPE_MAT2) {
            return 4;
        } else if ( ty == TINYGLTF_TYPE_MAT3) {
            return 9;
        } else if ( ty == TINYGLTF_TYPE_MAT4) {
            return 16;
        }
        ASSERTV(0, "Accessor type unknown");
        return 0;
    }

    unsigned int componentTypeToByteSize( int ct, int type ) {
        auto bsize = 1;
        switch ( ct ) {
            case 5120:
                bsize = 1;
                break;
            case 5121:
                bsize = 1;
                break;
            case 5122:
                bsize = 2;
                break;
            case 5123:
                bsize = 2;
                break;
            case 5124:
                bsize = 4;
                break;
            case 5125:
                bsize = 4;
                break;
            case 5126:
                bsize = 4;
                break;
            default:
                ASSERTV(0, "Invalid component type");
        }
        return bsize * accessorTypeToNumberOfComponent(type);
    }

    struct UCharFiller {
        const unsigned char *data;
        size_t size;
        unsigned int stride;
    };

    UCharFiller accessorFiller( const tinygltf::Model& model, int _index, GLTF2Service::ExtraAccessorData* _ead = nullptr ) {
        UCharFiller ucf{};

        auto acc = model.accessors[_index];
        if ( !acc.minValues.empty() && _ead ) _ead->min = acc.minValues;
        if ( !acc.maxValues.empty() && _ead ) _ead->max = acc.maxValues;

        auto bvi = model.bufferViews[acc.bufferView];
        auto& bufferContent = model.buffers[bvi.buffer];
        auto bs = componentTypeToByteSize( acc.componentType, acc.type );

        ucf.data = bufferContent.data.data() + acc.byteOffset + bvi.byteOffset;
        ucf.size = acc.count;
        ucf.stride = bs;

        return ucf;
    }

    template <typename RT>
    std::vector<RT> fillData( const tinygltf::Model& model, int _index, GLTF2Service::ExtraAccessorData* _ead = nullptr ) {
        auto ucf = accessorFiller( model, _index, _ead );
        std::vector<RT> ret;
        for ( size_t q = 0; q < ucf.size; q++ ) {
            RT elem{0};
            memcpy( &elem, ucf.data + q*ucf.stride, ucf.stride );
            ret.push_back( elem );
        }
        return ret;
    }

    Primitive gltfToPrimitive( int mode ) {
        ASSERT( mode >= 4 );

        if ( mode == 4 ) return PRIMITIVE_TRIANGLES;
        if ( mode == 5 ) return PRIMITIVE_TRIANGLE_STRIP;
        if ( mode == 6 ) return PRIMITIVE_TRIANGLE_FAN;

        return PRIMITIVE_TRIANGLES;
    }

    void fillGeom( std::shared_ptr<VData> geom, tinygltf::Model* model, int meshIndex, int primitiveIndex ) {

        auto mesh = model->meshes[meshIndex];
        tinygltf::Primitive primitive = mesh.primitives[primitiveIndex];

        geom->fillIndices( fillData<int32_t>( *model, primitive.indices ));

        int64_t NTBFill = 0;
        constexpr uint64_t NORMAL_MASK = 2;
        constexpr uint64_t TANGENT_MASK = 4;
        constexpr uint64_t TEXCOORD_MASK = 1;
        for ( const auto&[k, v] : primitive.attributes ) {
            if ( k == "POSITION" ) {
                GLTF2Service::ExtraAccessorData ead;
                geom->fillCoors3d( fillData<Vector3f>( *model, v, &ead ));
                geom->setMax( ead.max );
                geom->setMin( ead.min );
            } else if ( k == "NORMAL" ) {
                NTBFill |= NORMAL_MASK;
                geom->fillNormals( fillData<Vector3f>( *model, v ));
            } else if ( k == "TANGENT" ) {
                NTBFill |= TANGENT_MASK;
                geom->fillTangets( fillData<Vector4f>( *model, v ));
            } else if ( k == "TEXCOORD_0" ) {
                NTBFill |= TEXCOORD_MASK;
                geom->fillUV( fillData<Vector2f>( *model, v ), 0 );
                geom->fillUV( fillData<Vector2f>( *model, v ), 1 ); // By default fill second uvs anyway
            } else if ( k == "TEXCOORD_1" ) {
                geom->fillUV( fillData<Vector2f>( *model, v ), 1 );
            }
        }

        if ( !checkBitWiseFlag( NTBFill, TEXCOORD_MASK ) ) {
            geom->forcePlanarMapping();
        }
        if ( NTBFill < 2 ) {
            geom->calcBinormal();
        } else {
            geom->calcBinormalFromNormAndTang();
        }
    }

    void addGeom( SceneGraph& _sg, IntermediateGLTF& _gltf, int meshIndex, int primitiveIndex,
                  GeomSP father ) {

        auto mesh = _gltf.model->meshes[meshIndex];
        tinygltf::Primitive primitive = mesh.primitives[primitiveIndex];
        if ( primitive.indices < 0 ) return;

        auto material = _gltf.model->materials[primitive.material];
        _sg.GB<GT::GLTF2>( _gltf.model.get(),
                           GT::GLTF2MeshIndex( meshIndex ), GT::GLTF2PrimitiveIndex( primitiveIndex ),
                           GT::M( _gltf.matMap.at( material.name )), father );
    }

    template<typename T1, typename T2>
    void readParameterJsonDoubleValue( const tinygltf::Parameter v,
                                       const std::string& s1, const std::string& s2,
                                       std::pair<T1, T2>& baseColorTexture ) {
        if ( auto index = v.json_double_value.find( s1 ); index != v.json_double_value.end()) {
            if ( auto tcoord = v.json_double_value.find( s2 ); tcoord != v.json_double_value.end()) {
                baseColorTexture = { static_cast<int>(index->second), static_cast<int>(tcoord->second) };
            }
        }
    }

    void sigmoidMap( const InternalPBRComponent& ic, const IntermediateMaterial& _im, SigmoidSlope _sg ) {

        std::shared_ptr<RawImage> greyValue = EF::clone( *_im.grayScaleBaseColor.get());

        float lFloor = ( _sg == SigmoidSlope::Positive ? 1.0f - ic.value.x() : ic.value.x()) * 0.5f;

        greyValue->transform<uint8_t>( [&]( uint8_t& _value ) {
            float v = ( static_cast<float>(_value) - 127.0f ) / 127.0f;
            float vn = std::clamp( ic.value.x() + ( v * lFloor ), 0.0f, 1.0f );
            _value = static_cast<uint8_t >(vn * 255.0f);
        } );

//    _im.mb->buffer( ic.baseName, imageUtil::rawToPngMemory( greyValue ) );
    }

    void valueToColorMap( const InternalPBRComponent& ic, const IntermediateMaterial& _im ) {
        auto vc = RawImage{ 1, 1, 3, ic.value.RGBATOI() };
        vc.grayScale();
//    _im.mb->buffer( ic.baseName, imageUtil::rawToPngMemory(vc) );
    }

    void grayscaleToNormalMap( const InternalPBRComponent& ic, const IntermediateMaterial& _im ) {
        RawImage normalMap = _im.grayScaleBaseColor->toNormalMap();
//    _im.mb->buffer( ic.baseName, imageUtil::rawToPngMemory(normalMap) );
    }

    void colorToBasecolorMap( const InternalPBRComponent& ic, const IntermediateMaterial& _im ) {
        _im.grayScaleBaseColor = std::make_shared<RawImage>( 1, 1, 3, ic.value.RGBATOI());

//    _im.mb->buffer( ic.baseName, imageUtil::rawToPngMemory(_im.grayScaleBaseColor) );

        _im.grayScaleBaseColor->grayScale();
    }

    void saveInternalPBRComponent( IntermediateGLTF& _gltf, SceneGraph& _sg, const IntermediateMaterial& _im,
                                   const InternalPBRComponent& ic, const std::string& _uniformName ) {
        auto baseFileName = std::string{ _im.name + "_" + ic.baseName };
        if ( ic.texture.first == -1 ) {
//        switch ( ic.textureReconstructionMode ) {
//            case InternalPBRTextureReconstructionMode::GrayScaleCreate:
//                colorToBasecolorMap( ic, _im );
//                break;
//            case InternalPBRTextureReconstructionMode::SigmoidFloor:
//                sigmoidMap( ic, _im, SigmoidSlope::Negative );
////                valueToColorMap( ic, _im );
//                break;
//            case InternalPBRTextureReconstructionMode::SigmoidCeiling:
//                sigmoidMap( ic, _im, SigmoidSlope::Positive );
////                valueToColorMap( ic, _im );
//                break;
//            case InternalPBRTextureReconstructionMode::NormalMap:
//                grayscaleToNormalMap( ic, _im );
//                break;
//            default:
//                break;
//        }
        } else {
            auto texture = _gltf.model->textures[ic.texture.first];
            auto image = _gltf.model->images[texture.source];
            uint8_p imgBuffer;
            if ( !image.uri.empty()) {
                auto ext = getFileNameExt( image.name );
                imgBuffer = FM::readLocalFile( _gltf.basePath + image.name );
            } else {
                imgBuffer = imageUtil::bufferToPngMemory( image.width, image.height, image.component,
                                                          image.image.data());
            }
//        if ( ic.textureReconstructionMode == InternalPBRTextureReconstructionMode::GrayScaleCreate ) {
//            _im.grayScaleBaseColor = rawImageDecodeFromMemory(imgBuffer);
//            _im.grayScaleBaseColor.grayScale();
//            _im.grayScaleBaseColor.brightnessContrast( 2.2f, 100 );
//        }
            auto imRef = _sg.B<IB>( baseFileName ).addIM( RawImage{ std::move( imgBuffer ) } );
            _im.values->assign( _uniformName, imRef );
//            _im.mb.emplace( baseFileName, RawImage{ std::move( imgBuffer ) } ); //, _uniformName
        }
    }

    void saveMaterial( IntermediateGLTF& _gltf, SceneGraph& _sg, const IntermediateMaterial& im ) {
        saveInternalPBRComponent( _gltf, _sg, im, im.baseColor, UniformNames::diffuseTexture );
        saveInternalPBRComponent( _gltf, _sg, im, im.metallic, UniformNames::metallicTexture );
        saveInternalPBRComponent( _gltf, _sg, im, im.roughness, UniformNames::roughnessTexture );
        saveInternalPBRComponent( _gltf, _sg, im, im.normal, UniformNames::normalTexture );
    }

    IntermediateMaterial elaborateMaterial( SceneGraph& _sg, IntermediateGLTF& _gltf, const tinygltf::Material& mat ) {
        IntermediateMaterial im;
        im.name = toLower( mat.name );
        removeNonAlphaCharFromString( im.name );

        for ( const auto&[k, v] : mat.values ) {
            if ( k == "baseColorFactor" ) {
                im.baseColor.value = v.number_array;
                im.values->assign( UniformNames::diffuseColor, im.baseColor.value.xyz());
            } else if ( k == "baseColorTexture" ) {
                readParameterJsonDoubleValue( v, "index", "texCoord", im.baseColor.texture );
            } else if ( k == "metallicFactor" ) {
                auto lv = static_cast<float>(v.number_value);
                im.metallic.value = Vector4f{ lv, lv, lv, 1.0f };
                im.values->assign( UniformNames::metallic, lv );
            } else if ( k == "metallicTexture" ) {
                readParameterJsonDoubleValue( v, "index", "texCoord", im.metallic.texture );
            } else if ( k == "roughnessFactor" ) {
                auto lv = static_cast<float>(v.number_value);
                im.values->assign( UniformNames::roughness, lv );
                im.roughness.value = Vector4f{ lv, lv, lv, 1.0f };
            } else if ( k == "roughnessTexture" ) {
                readParameterJsonDoubleValue( v, "index", "texCoord", im.roughness.texture );
            }
        }

        for ( const auto&[k, v] : mat.additionalValues ) {
            if ( k == "normalTexture" ) {
                readParameterJsonDoubleValue( v, "index", "texCoord", im.normal.texture );
            }
        }

        saveMaterial( _gltf, _sg, im );

        auto materialSP = std::make_shared<Material>( im.values );
        auto matRef = _sg.B<MB>( mat.name ).addIM( materialSP );

        _gltf.matMap[mat.name] = matRef;

        return im;
    }

    void addMeshNode( SceneGraph& _sg, IntermediateGLTF& _gltf, const tinygltf::Node& node, GeomSP hier ) {

        Vector3f pos = Vector3f::ZERO;
        Quaternion rot;
        Vector3f scale = Vector3f::ONE;

        if ( !node.translation.empty()) {
            pos = { node.translation[0], node.translation[1], node.translation[2] };
        }
        if ( !node.rotation.empty()) {
            rot = Quaternion{ -node.rotation[0], -node.rotation[1], -node.rotation[2], node.rotation[3] };
        }

        if ( !node.scale.empty()) {
            scale = { node.scale[0], node.scale[1], node.scale[2] };
        }

        hier->generateLocalTransformData( pos, rot, scale );

        if ( node.mesh >= 0 ) {
            for ( size_t k = 0; k < _gltf.model->meshes[node.mesh].primitives.size(); k++ ) {
                addGeom( _sg, _gltf, node.mesh, k, hier );
            }
        }

        for ( const auto& ci : node.children ) {
            auto nextNode = _gltf.model->nodes[ci];
            if ( nextNode.mesh >= 0 || !nextNode.children.empty()) {
                auto c = hier->addChildren( nextNode.name );
                addMeshNode( _sg, _gltf, nextNode, c );
            }
        }
    }

    void convert( SceneGraph& _sg, IntermediateGLTF& _gltf ) {

        for ( const auto& gltfMaterial : _gltf.model->materials ) {
            elaborateMaterial( _sg, _gltf, gltfMaterial );
        }
        for ( const auto& scene : _gltf.model->scenes ) {
            auto rootScene = _sg.GC();
            for ( auto nodeIndex = 0; nodeIndex < scene.nodes.size(); nodeIndex++ ) {
                auto node = _gltf.model->nodes[nodeIndex];
//                auto cscene = std::make_shared<GeomSceneArtifact>( node.name );
                addMeshNode( _sg, _gltf, node, rootScene );
//                cscene->prune();
//                cscene->generateMatrixHierarchy();
            }
            rootScene->updateTransform( V3f::ZERO );
            _sg.GC(rootScene);
        }
    }

    void load( SceneGraph& _sg, const std::string& _path, const SerializableContainer& _array ) {

        tinygltf::TinyGLTF gltf_ctx;
        std::string err;
        std::string warn;
        std::string ext = getFileNameExt(_path); //GetFilePathExtension( _path );

        IntermediateGLTF gltfScene;

        gltfScene.basePath = getFileNamePath( _path ) + "/";
        gltfScene.Name( getFileNameOnly( _path ));
        gltfScene.model = std::make_shared<tinygltf::Model>();

        bool ret = false;
        if ( ext.compare( ".glb" ) == 0 ) {
            std::cout << "Reading binary glTF" << std::endl;
            if ( _array.empty()) {
                ret = gltf_ctx.LoadBinaryFromFile( gltfScene.model.get(), &err, &warn, _path.c_str());
            } else {
                ret = gltf_ctx.LoadBinaryFromMemory( gltfScene.model.get(), &err, &warn,
                                                     reinterpret_cast<const unsigned char *>(_array.data()),
                                                     _array.size());
            }
        } else {
            std::cout << "Reading ASCII glTF" << std::endl;
            if ( _array.empty()) {
                ret = gltf_ctx.LoadASCIIFromFile( gltfScene.model.get(), &err, &warn, _path.c_str());
            } else {
                auto str = std::string( _array.begin(), _array.end());
                ret = gltf_ctx.LoadASCIIFromString( gltfScene.model.get(), &err, &warn, str.c_str(), str.size(), "" );
            }
        }

        if ( !warn.empty()) {
            LOGR( "Warn: %s", warn.c_str());
        }

        if ( !err.empty()) {
            LOGR( "Err: %s", err.c_str());
        }

        if ( !ret ) {
            LOGR( "Failed to parse glTF" );
            return;
        }

        convert( _sg, gltfScene );

//    Dump( model );
    }
}