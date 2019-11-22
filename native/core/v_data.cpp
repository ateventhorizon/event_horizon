//
// Created by Dado on 2019-03-31.
//

#include "v_data.hpp"

#include <core/soa_utils.h>
#include <core/math/plane3f.h>

void VData::fill( const PolyStruct& ps ) {

    for ( int q = 0; q < ps.numIndices; q++ ) {
        auto idx = ps.indices[q];
        PUUNTBC d1{ ps.verts[idx],ps.uvs[q],ps.uv2s[q],ps.normals[q],ps.tangents[q],ps.binormals[q],ps.colors[q] };
        vSoaData.emplace_back( d1 );
        vIndices.push_back( q );
    }
}

void VData::fillIndices( const std::vector<uint32_t>& _indices ) {
    vIndices = _indices;
}

void VData::fillCoors3d( const std::vector<Vector3f>& _verts ) {
    vSoaData.resize( _verts.size() );
    for ( size_t t = 0; t < _verts.size(); t++ ) {
        vSoaData[t].pos = _verts[t];
//        vSoaData[t].a6 = Vector4f::ONE; // Just inizialise the color to one
    }
}

void VData::fillUV( const std::vector<Vector2f>& _uvs, uint32_t _index ) {
    if ( _index < 2 ) {
        vSoaData.resize( _uvs.size() );
        for ( size_t t = 0; t < _uvs.size(); t++ ) {
            if ( _index == 0 ) {
                vSoaData[t].a1 = _uvs[t];
            } else {
                vSoaData[t].a2 = _uvs[t];
            }
        }
    } else {
        LOGR("[GLTF2]: Mora than 2 sets of data, don't know what to do with it.");
    }
}

void VData::fillSetUV( size_t _size, const Vector2f& _uvs, uint32_t _index ) {
    if ( _index < 2 ) {
        vSoaData.resize( _size );
        for ( size_t t = 0; t < _size; t++ ) {
            if ( _index == 0 ) {
                vSoaData[t].a1 = _uvs;
            } else {
                vSoaData[t].a2 = _uvs;
            }
        }
    } else {
        LOGR("[GLTF2]: Mora than 2 sets of data, don't know what to do with it.");
    }
}

void VData::fillNormals( const std::vector<Vector3f>& _normals, bool _bInvert ) {
    vSoaData.resize( _normals.size() );
    float bi = _bInvert ? -1.0f : 1.0f;
    for ( size_t t = 0; t < _normals.size(); t++ ) {
        vSoaData[t].a3 = _normals[t] * bi;
    }
}

void VData::fillTangets( const std::vector<Vector3f>& _tangents, bool _bInvert ) {
    vSoaData.resize( _tangents.size() );
    float bi = _bInvert ? -1.0f : 1.0f;
    for ( size_t t = 0; t < _tangents.size(); t++ ) {
        vSoaData[t].a4 = Vector4f{_tangents[t], 1.0f} * bi;
    }
}

void VData::fillTangets( const std::vector<Vector4f>& _tangents, bool _bInvert ) {
    vSoaData.resize( _tangents.size() );
    float bi = _bInvert ? -1.0f : 1.0f;
    for ( size_t t = 0; t < _tangents.size(); t++ ) {
        vSoaData[t].a4 = _tangents[t].xyz() * bi;
    }
}

void VData::fillBinormal( const std::vector<Vector3f>& _binormals, bool _bInvert ) {
    vSoaData.resize( _binormals.size() );
    float bi = _bInvert ? -1.0f : 1.0f;
    for ( size_t t = 0; t < _binormals.size(); t++ ) {
        vSoaData[t].a5 = _binormals[t] * bi;
    }
}

void VData::fillColors( const std::vector<Vector4f>& _colors ) {
    vSoaData.resize( _colors.size() );
    for ( size_t t = 0; t < _colors.size(); t++ ) {
        vSoaData[t].a6 = _colors[t];
    }
}

void VData::changeHandness() {
    for (auto & i : vSoaData) {
        i.pos.swizzle( 1, 2 );
        i.a3.swizzle( 1, 2 );
        i.a4.swizzle( 1, 2 );
        i.a5.swizzle( 1, 2 );
    }
}

void VData::sanitizeUVMap() {
    for ( auto& v : vSoaData ) {
        v.a1 = v.pos.dominantVector2();
        v.a2 = v.a1;
    }
}

void VData::calcBinormalFromNormAndTang() {
    for ( size_t i = 0; i < vSoaData.size(); i+=3 ) {
        vSoaData[i].a5 = cross( vSoaData[i].a3, vSoaData[i].a4.xyz() ) * vSoaData[i].a4.w();
    }
}

void VData::calcBinormal() {
    ASSERT( primitive == PRIMITIVE_TRIANGLES );
    for ( size_t i = 0; i < vIndices.size(); i+=3 ) {
        auto i0 = vIndices[i+0];
        auto i1 = vIndices[i+1];
        auto i2 = vIndices[i+2];
        tbCalc( vSoaData[i0].pos, vSoaData[i1].pos, vSoaData[i2].pos,
                vSoaData[i0].a1, vSoaData[i1].a1, vSoaData[i2].a1,
                vSoaData[i0].a4, vSoaData[i1].a4, vSoaData[i2].a4,
                vSoaData[i0].a5, vSoaData[i1].a5, vSoaData[i2].a5 );
    }
}

void VData::changeWindingOrder() {
    ASSERT( vIndices.size() % 3 == 0 );

    for ( unsigned int t = 0; t < vIndices.size(); t += 3 ) {
        std::swap( vIndices[t + 0], vIndices[t + 2] );
        std::swap( vSoaData[t + 0].pos, vSoaData[t + 2].pos );
        std::swap( vSoaData[t + 0].a1, vSoaData[t + 2].a1 );
        std::swap( vSoaData[t + 0].a2, vSoaData[t + 2].a2 );
        std::swap( vSoaData[t + 0].a3, vSoaData[t + 2].a3 );
        std::swap( vSoaData[t + 0].a4, vSoaData[t + 2].a4 );
        std::swap( vSoaData[t + 0].a5, vSoaData[t + 2].a5 );
        std::swap( vSoaData[t + 0].a6, vSoaData[t + 2].a6 );
    }
}

void VData::mirrorFlip( WindingOrderT wow, WindingOrderT woh, const Rect2f& bbox ) {
    Vector3f flipXNormal = Vector3f( -1.0f, 1.0f, 1.0f );
    Vector3f flipYNormal = Vector3f( 1.0f, -1.0f, 1.0f );
    for (auto & index : vSoaData) {
        if ( wow == WindingOrder::CW ) {
            index.pos.setX( bbox.width() - index.pos[0] );
            index.a3 *= flipXNormal;
            index.a4 *= V4f{flipXNormal, 1.0f};
            index.a5 *= flipXNormal;
        }
        if ( woh == WindingOrder::CW ) {
            index.pos.setY( bbox.height() - index.pos[1] );
            index.a3 *= flipYNormal;
            index.a4 *= V4f{flipYNormal, 1.0f};
            index.a5 *= flipYNormal;
        }
    }
    if ( ( wow == WindingOrder::CW || woh == WindingOrder::CW ) && ( wow == WindingOrder::CCW || woh == WindingOrder::CCW ) ) {
        changeWindingOrder();
    }
}

void VData::checkBaricentricCoordsOn( const Vector3f& i, uint32_t pIndexStart, uint32_t pIndexEnd, uint32_t& pIndex,
                                      float& u, float& v ) {
    Vector3f a = Vector3f::ZERO;
    Vector3f b = Vector3f::ZERO;
    Vector3f c = Vector3f::ZERO;
    uint32_t vi = pIndexStart;
    uint32_t possibleIndices[256];
    uint32_t pii = 0;
    pIndex = pIndexStart;
    u = 0.0f;
    v = 0.0f;
    int passes = 3;
    if ( pIndexEnd - pIndexStart > 3 ) {
        // Shortest distance from i
        float minD = std::numeric_limits<float>::max();
        float d = 0.0f;
        for ( auto q = pIndexStart; q < pIndexEnd; q += 3 ) {
            d = distance( i, vSoaData[q].pos );
            if ( d < minD ) {
                pii = 0;
                minD = d;
            } else if ( isScalarEqual( d - minD, 0.0f ) ) {
                possibleIndices[pii++] = q;
            }

            d = distance( i, vSoaData[q + 1].pos );
            if ( d < minD ) {
                pii = 0;
                minD = d;
            } else if ( isScalarEqual( d - minD, 0.0f ) ) {
                possibleIndices[pii++] = q;
            }

            d = distance( i, vSoaData[q + 2].pos );
            if ( d < minD ) {
                pii = 0;
                minD = d;
            } else if ( isScalarEqual( d - minD, 0.0f ) ) {
                possibleIndices[pii++] = q;
            }
        }

        for ( uint32_t h = 0; h < pii; h++ ) {
            vi = possibleIndices[h];
            passes = 0;
            for ( int q = 0; q < 3; q++ ) {
                a = vSoaData[vi + q].pos;
                b = vSoaData[q == 2 ? vi : vi + q + 1].pos;
                c = a + vSoaData[vi + q].a3;
                Plane3f pplane = { a,b,c };
                if ( pplane.checkSide( i ) > 0.0f ) continue;
                ++passes;
            }
            if ( passes == 3 ) break;
        }
    }
    if ( passes == 3 ) {
        a = vSoaData[vi].pos;
        b = vSoaData[vi + 1].pos;
        c = vSoaData[vi + 2].pos;

        Vector3f crossCB = cross( c - b, i - b );
        u = length( crossCB );
        Vector3f crossAC = cross( a - c, i - c );
        v = length( crossAC );
        pIndex = vi;
        return;
    }
}

void VData::addTriangleVertex( const Vector3f& _vc, const Vector2f& _uv, const Vector2f& _uv2, const Vector3f& _vn,
                               const Vector4f& _vt, const Vector3f& _vb, const Vector3f& _v8 ) {

    BBox3d().expand(_vc);
    vSoaData.emplace_back( _vc, _uv, _uv2, _vn, _vt, _vb, _v8 );
    vIndices.push_back( vIndices.size() );
}

void VData::add( uint32_t _i, const Vector3f& _v, const Vector3f& _n, const Vector2f& _uv, const Vector2f& _uv2,
                 const Vector4f& _t, const Vector3f& _b, const Vector4f& _c ) {
    BBox3d().expand(_v);
    vIndices.push_back( _i );
    vSoaData.emplace_back( _v, _uv, _uv2, _n, _t, _b, _c );
}

void VData::swapIndicesWinding( Primitive _pr ) {
    switch ( _pr ) {
        case PRIMITIVE_TRIANGLES:
            for ( size_t i = 0; i < vIndices.size(); i+=3 ) {
                swapVectorPair( vIndices, i + 1, i + 2 );
            }
            break;
        case PRIMITIVE_TRIANGLE_STRIP:
            break;
        case PRIMITIVE_TRIANGLE_FAN:
            break;
        default:
            ASSERT(0);
    }
}

void VData::flipNormals() {
    for ( auto& v : vSoaData ) {
        v.a3 *= -1.0f;
        v.a4 *= -1.0f;
        v.a5 *= -1.0f;
    }
}

size_t VData::numIndices() const { return vIndices.size(); }

size_t VData::numVerts() const { return vSoaData.size(); }

const std::vector<uint32_t>& VData::getVIndices() const {
    return vIndices;
}

//const std::vector<Vector3f>& VData::getVcoords3d() const {
//    return vcoords3d;
//}
//
//const std::vector<Vector3f>& VData::getVnormals3d() const {
//    return vnormals3d;
//}
//
//const std::vector<Vector3f>& VData::getVtangents3d() const {
//    return vtangents3d;
//}
//
//const std::vector<Vector3f>& VData::getVbinormals3d() const {
//    return vbinormals3d;
//}
//
//const std::vector<Vector2f>& VData::getVUVs() const {
//    return vUVs;
//}
//
//const std::vector<Vector2f>& VData::getVUV2s() const {
//    return vUV2s;
//}
//
//const std::vector<Vector4f>& VData::getVColor() const {
//    return vColor;
//}

void VData::setVIndices( const size_t _index, const uint32_t& _value ) {
    vIndices[_index] = _value;
}

//void VData::setVcoords3d( const size_t _index, const Vector3f& _value ) {
//    vcoords3d[_index] = _value;
//}
//
//void VData::setVnormals3d( const size_t _index, const Vector3f& _value ) {
//    vnormals3d[_index] = _value;
//}
//
//void VData::setVtangents3d( const size_t _index, const Vector3f& _value ) {
//    vtangents3d[_index] = _value;
//}
//
//void VData::setVbinormals3d( const size_t _index, const Vector3f& _value ) {
//    vbinormals3d[_index] = _value;
//}
//
//void VData::setVUVs( const size_t _index, const Vector2f& _value ) {
//    vUVs[_index] = _value;
//}
//
void VData::setVUV2s( const size_t _index, const Vector2f& _value ) {
    vSoaData[_index].a2 = _value;
}
//
//void VData::setVColor( const size_t _index, const Vector4f& _value ) {
//    vColor[_index] = _value;
//}

const Vector3f& VData::getMin() const {
    return BBox3d().minPoint();
}

void VData::setMin( const Vector3f& min ) {
    BBox3d().setMinPoint(min);
}

const Vector3f& VData::getMax() const {
    return BBox3d().maxPoint();
}

void VData::setMax( const Vector3f& max ) {
    BBox3d().setMaxPoint(max);
}

Primitive VData::getPrimitive() const {
    return primitive;
}

void VData::setPrimitive( Primitive _primitive ) {
    VData::primitive = _primitive;
}

void VData::forcePlanarMapping() {
    for (auto & t : vSoaData) {
        t.a1 = dominantMapping( t.a3, t.pos );
        t.a2 = t.a1;
    }

}

void VData::flattenStride( void* ret, size_t _index, std::shared_ptr<Matrix4f> _mat ) {

    size_t off = 0;
    size_t stride = sizeof(float)*3;
    switch (_index ) {
//        case 0:
//        case 3:
//        case 5:
//            stride = sizeof(float)*3;
//            break;
        case 1:
        case 2:
            stride = sizeof(float)*2;
            break;
        case 4:
        case 6:
            stride = sizeof(float)*4;
            break;
        default:
            break;
    }

    for ( const auto& elem : vSoaData ) {
        switch (_index ) {
            case 0: {
                auto v1 = _mat ? _mat->transform(elem.pos) : elem.pos;
                memcpy((unsigned char*)ret + off*stride, &v1, stride );
            }
            break;
            case 1:
                memcpy((unsigned char*)ret + off*stride, &elem.a1, stride );
                break;
            case 2:
                memcpy((unsigned char*)ret + off*stride, &elem.a2, stride );
                break;
            case 3: {
                auto v1 = _mat ? _mat->transform3x3(elem.a3) : elem.a3;
                if ( _mat) v1 = normalize(v1);
                memcpy((unsigned char*)ret + off*stride, &v1, stride );
            }
                break;
            case 4:
                memcpy((unsigned char*)ret + off*stride, &elem.a4, stride );
                break;
            case 5:
                memcpy((unsigned char*)ret + off*stride, &elem.a5, stride );
                break;
            case 6:
                memcpy((unsigned char*)ret + off*stride, &elem.a6, stride );
                break;
            default:
                break;
        }
        off++;
    }
}

void VData::mapIndices( void* ret, uint32_t _startIndex, uint32_t _voffsetIndex,
                            const std::string& _oRef, std::unordered_map<uint32_t, HashIndexPairU32>& _oMap  ) {
    auto fill = (uint32_t*)ret;
    fill += _startIndex;
    for ( size_t t = 0; t < vIndices.size(); t++ ) {
        fill[t] = vIndices[t] + _voffsetIndex;
        _oMap[_startIndex+t] = { _oRef, vIndices[t] };
    }
}