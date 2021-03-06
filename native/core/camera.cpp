//
//  camera.cpp
//  SixthView
//
//  Created by Dado on 11/02/2015.
//  Copyright (c) 2015 JFDP Labs. All rights reserved.
//

#include "camera.h"
#include <core/math/path_util.h>
#include <core/app_globals.h>
#include <core/math/plane3f.h>
#include <core/math/quaternion.h>
#include <core/math/anim.h>
#include <core/math/vector_util.hpp>

int glhProjectf( float objx, float objy, float objz, float *modelview, float *projection, int *viewport,
                 float *windowCoordinate ) {
    //Transformation vectors
    float fTempo[8];
    //Modelview transform
    fTempo[0] = modelview[0] * objx + modelview[4] * objy + modelview[8] * objz + modelview[12];  //w is always 1
    fTempo[1] = modelview[1] * objx + modelview[5] * objy + modelview[9] * objz + modelview[13];
    fTempo[2] = modelview[2] * objx + modelview[6] * objy + modelview[10] * objz + modelview[14];
    fTempo[3] = modelview[3] * objx + modelview[7] * objy + modelview[11] * objz + modelview[15];
    //Projection transform, the final row of projection matrix is always [0 0 -1 0]
    //so we optimize for that.
    fTempo[4] = projection[0] * fTempo[0] + projection[4] * fTempo[1] + projection[8] * fTempo[2] +
                projection[12] * fTempo[3];
    fTempo[5] = projection[1] * fTempo[0] + projection[5] * fTempo[1] + projection[9] * fTempo[2] +
                projection[13] * fTempo[3];
    fTempo[6] = projection[2] * fTempo[0] + projection[6] * fTempo[1] + projection[10] * fTempo[2] +
                projection[14] * fTempo[3];
    fTempo[7] = -fTempo[2];
    //The result normalizes between -1 and 1
    if ( fTempo[7] == 0.0f )    //The w value
        return 0;
    fTempo[7] = 1.0f / fTempo[7];
    //Perspective division
    fTempo[4] *= fTempo[7];
    fTempo[5] *= fTempo[7];
    fTempo[6] *= fTempo[7];
    //Window coordinates
    //Map x, y to range 0-1
    windowCoordinate[0] = ( fTempo[4] * 0.5f + 0.5f ) * viewport[2] + viewport[0];
    windowCoordinate[1] = ( fTempo[5] * 0.5f + 0.5f ) * viewport[3] + viewport[1];
    //This is only correct when glDepthRange(0.0, 1.0f)
    windowCoordinate[2] = ( 1.0f + fTempo[6] ) * 0.5f;    //Between 0 and 1
    return 1;
}

int glhUnProjectf( float winx, float winy, float winz, const float *modelview, const float *projection, int *viewport,
                   float *objectCoordinate ) {
    //Transformation matrices
    float m[16], A[16];
    float in[4], out[4];
    //Calculation for inverting a matrix, compute projection x modelview
    //and store in A[16]
    MultiplyMatrices4by4OpenGL_FLOAT(A, projection, modelview);
    //Now compute the inverse of matrix A
    if ( glhInvertMatrixf2(A, m) == 0 )
        return 0;
    //Transformation of normalized coordinates between -1 and 1
    in[0] = ( winx - (float) viewport[0] ) / (float) viewport[2] * 2.0f - 1.0f;
    in[1] = ( winy - (float) viewport[1] ) / (float) viewport[3] * 2.0f - 1.0f;
    in[2] = 2.0f * winz - 1.0f;
    in[3] = 1.0f;
    //Objects coordinates
    MultiplyMatrixByVector4by4OpenGL_FLOAT(out, m, in);
    if ( out[3] == 0.0f )
        return 0;
    out[3] = 1.0f / out[3];
    objectCoordinate[0] = out[0] * out[3];
    objectCoordinate[1] = out[1] * out[3];
    objectCoordinate[2] = out[2] * out[3];
    return 1;
}

void MultiplyMatrices4by4OpenGL_FLOAT( float *result, const float *matrix1, const float *matrix2 ) {
    result[0] = matrix1[0] * matrix2[0] +
                matrix1[4] * matrix2[1] +
                matrix1[8] * matrix2[2] +
                matrix1[12] * matrix2[3];
    result[4] = matrix1[0] * matrix2[4] +
                matrix1[4] * matrix2[5] +
                matrix1[8] * matrix2[6] +
                matrix1[12] * matrix2[7];
    result[8] = matrix1[0] * matrix2[8] +
                matrix1[4] * matrix2[9] +
                matrix1[8] * matrix2[10] +
                matrix1[12] * matrix2[11];
    result[12] = matrix1[0] * matrix2[12] +
                 matrix1[4] * matrix2[13] +
                 matrix1[8] * matrix2[14] +
                 matrix1[12] * matrix2[15];
    result[1] = matrix1[1] * matrix2[0] +
                matrix1[5] * matrix2[1] +
                matrix1[9] * matrix2[2] +
                matrix1[13] * matrix2[3];
    result[5] = matrix1[1] * matrix2[4] +
                matrix1[5] * matrix2[5] +
                matrix1[9] * matrix2[6] +
                matrix1[13] * matrix2[7];
    result[9] = matrix1[1] * matrix2[8] +
                matrix1[5] * matrix2[9] +
                matrix1[9] * matrix2[10] +
                matrix1[13] * matrix2[11];
    result[13] = matrix1[1] * matrix2[12] +
                 matrix1[5] * matrix2[13] +
                 matrix1[9] * matrix2[14] +
                 matrix1[13] * matrix2[15];
    result[2] = matrix1[2] * matrix2[0] +
                matrix1[6] * matrix2[1] +
                matrix1[10] * matrix2[2] +
                matrix1[14] * matrix2[3];
    result[6] = matrix1[2] * matrix2[4] +
                matrix1[6] * matrix2[5] +
                matrix1[10] * matrix2[6] +
                matrix1[14] * matrix2[7];
    result[10] = matrix1[2] * matrix2[8] +
                 matrix1[6] * matrix2[9] +
                 matrix1[10] * matrix2[10] +
                 matrix1[14] * matrix2[11];
    result[14] = matrix1[2] * matrix2[12] +
                 matrix1[6] * matrix2[13] +
                 matrix1[10] * matrix2[14] +
                 matrix1[14] * matrix2[15];
    result[3] = matrix1[3] * matrix2[0] +
                matrix1[7] * matrix2[1] +
                matrix1[11] * matrix2[2] +
                matrix1[15] * matrix2[3];
    result[7] = matrix1[3] * matrix2[4] +
                matrix1[7] * matrix2[5] +
                matrix1[11] * matrix2[6] +
                matrix1[15] * matrix2[7];
    result[11] = matrix1[3] * matrix2[8] +
                 matrix1[7] * matrix2[9] +
                 matrix1[11] * matrix2[10] +
                 matrix1[15] * matrix2[11];
    result[15] = matrix1[3] * matrix2[12] +
                 matrix1[7] * matrix2[13] +
                 matrix1[11] * matrix2[14] +
                 matrix1[15] * matrix2[15];
}

void MultiplyMatrixByVector4by4OpenGL_FLOAT( float *resultvector, const float *matrix, const float *pvector ) {
    resultvector[0] =
            matrix[0] * pvector[0] + matrix[4] * pvector[1] + matrix[8] * pvector[2] + matrix[12] * pvector[3];
    resultvector[1] =
            matrix[1] * pvector[0] + matrix[5] * pvector[1] + matrix[9] * pvector[2] + matrix[13] * pvector[3];
    resultvector[2] =
            matrix[2] * pvector[0] + matrix[6] * pvector[1] + matrix[10] * pvector[2] + matrix[14] * pvector[3];
    resultvector[3] =
            matrix[3] * pvector[0] + matrix[7] * pvector[1] + matrix[11] * pvector[2] + matrix[15] * pvector[3];
}

#define SWAP_ROWS_DOUBLE( a, b ) { double *_tmp = a; (a) = (b); (b) = _tmp; }
#define SWAP_ROWS_FLOAT( a, b ) { float *_tmp = a; (a) = (b); (b) = _tmp; }
#define MAT( m, r, c ) (m)[(c) * 4 + (r)]

//This code comes directly from GLU except that it is for float
int glhInvertMatrixf2( float *m, float *out ) {
    float wtmp[4][8];
    float m0, m1, m2, m3, s;
    float *r0, *r1, *r2, *r3;
    r0 = wtmp[0];
    r1 = wtmp[1];
    r2 = wtmp[2];
    r3 = wtmp[3];
    r0[0] = MAT(m, 0, 0);
    r0[1] = MAT(m, 0, 1);
    r0[2] = MAT(m, 0, 2);
    r0[3] = MAT(m, 0, 3);
    r0[4] = 1.0f;
    r0[5] = r0[6] = r0[7] = 0.0f;
    r1[0] = MAT(m, 1, 0);
    r1[1] = MAT(m, 1, 1);
    r1[2] = MAT(m, 1, 2);
    r1[3] = MAT(m, 1, 3);
    r1[5] = 1.0f;
    r1[4] = r1[6] = r1[7] = 0.0f;
    r2[0] = MAT(m, 2, 0);
    r2[1] = MAT(m, 2, 1);
    r2[2] = MAT(m, 2, 2);
    r2[3] = MAT(m, 2, 3);
    r2[6] = 1.0f;
    r2[4] = r2[5] = r2[7] = 0.0f;
    r3[0] = MAT(m, 3, 0);
    r3[1] = MAT(m, 3, 1);
    r3[2] = MAT(m, 3, 2);
    r3[3] = MAT(m, 3, 3);
    r3[7] = 1.0f;
    r3[4] = r3[5] = r3[6] = 0.0f;
    /* choose pivot - or die */
    if ( fabsf(r3[0]) > fabsf(r2[0]) ) SWAP_ROWS_FLOAT(r3, r2);
    if ( fabsf(r2[0]) > fabsf(r1[0]) ) SWAP_ROWS_FLOAT(r2, r1);
    if ( fabsf(r1[0]) > fabsf(r0[0]) ) SWAP_ROWS_FLOAT(r1, r0);
    if ( 0.0f == r0[0] )
        return 0;
    /* eliminate first variable     */
    m1 = r1[0] / r0[0];
    m2 = r2[0] / r0[0];
    m3 = r3[0] / r0[0];
    s = r0[1];
    r1[1] -= m1 * s;
    r2[1] -= m2 * s;
    r3[1] -= m3 * s;
    s = r0[2];
    r1[2] -= m1 * s;
    r2[2] -= m2 * s;
    r3[2] -= m3 * s;
    s = r0[3];
    r1[3] -= m1 * s;
    r2[3] -= m2 * s;
    r3[3] -= m3 * s;
    s = r0[4];
    if ( s != 0.0f ) {
        r1[4] -= m1 * s;
        r2[4] -= m2 * s;
        r3[4] -= m3 * s;
    }
    s = r0[5];
    if ( s != 0.0f ) {
        r1[5] -= m1 * s;
        r2[5] -= m2 * s;
        r3[5] -= m3 * s;
    }
    s = r0[6];
    if ( s != 0.0f ) {
        r1[6] -= m1 * s;
        r2[6] -= m2 * s;
        r3[6] -= m3 * s;
    }
    s = r0[7];
    if ( s != 0.0f ) {
        r1[7] -= m1 * s;
        r2[7] -= m2 * s;
        r3[7] -= m3 * s;
    }
    /* choose pivot - or die */
    if ( fabsf(r3[1]) > fabsf(r2[1]) ) SWAP_ROWS_FLOAT(r3, r2);
    if ( fabsf(r2[1]) > fabsf(r1[1]) ) SWAP_ROWS_FLOAT(r2, r1);
    if ( 0.0f == r1[1] )
        return 0;
    /* eliminate second variable */
    m2 = r2[1] / r1[1];
    m3 = r3[1] / r1[1];
    r2[2] -= m2 * r1[2];
    r3[2] -= m3 * r1[2];
    r2[3] -= m2 * r1[3];
    r3[3] -= m3 * r1[3];
    s = r1[4];
    if ( 0.0f != s ) {
        r2[4] -= m2 * s;
        r3[4] -= m3 * s;
    }
    s = r1[5];
    if ( 0.0f != s ) {
        r2[5] -= m2 * s;
        r3[5] -= m3 * s;
    }
    s = r1[6];
    if ( 0.0f != s ) {
        r2[6] -= m2 * s;
        r3[6] -= m3 * s;
    }
    s = r1[7];
    if ( 0.0f != s ) {
        r2[7] -= m2 * s;
        r3[7] -= m3 * s;
    }
    /* choose pivot - or die */
    if ( fabsf(r3[2]) > fabsf(r2[2]) ) SWAP_ROWS_FLOAT(r3, r2);
    if ( 0.0f == r2[2] )
        return 0;
    /* eliminate third variable */
    m3 = r3[2] / r2[2];
    r3[3] -= m3 * r2[3];
    r3[4] -= m3 * r2[4];
    r3[5] -= m3 * r2[5];
    r3[6] -= m3 * r2[6];
    r3[7] -= m3 * r2[7];
    /* last check */
    if ( 0.0f == r3[3] )
        return 0;
    s = 1.0f / r3[3];        /* now back substitute row 3 */
    r3[4] *= s;
    r3[5] *= s;
    r3[6] *= s;
    r3[7] *= s;
    m2 = r2[3];            /* now back substitute row 2 */
    s = 1.0f / r2[2];
    r2[4] = s * ( r2[4] - r3[4] * m2 );
    r2[5] = s * ( r2[5] - r3[5] * m2 );
    r2[6] = s * ( r2[6] - r3[6] * m2 );
    r2[7] = s * ( r2[7] - r3[7] * m2 );
    m1 = r1[3];
    r1[4] -= r3[4] * m1;
    r1[5] -= r3[5] * m1;
    r1[6] -= r3[6] * m1;
    r1[7] -= r3[7] * m1;
    m0 = r0[3];
    r0[4] -= r3[4] * m0;
    r0[5] -= r3[5] * m0;
    r0[6] -= r3[6] * m0;
    r0[7] -= r3[7] * m0;
    m1 = r1[2];            /* now back substitute row 1 */
    s = 1.0f / r1[1];
    r1[4] = s * ( r1[4] - r2[4] * m1 );
    r1[5] = s * ( r1[5] - r2[5] * m1 );
    r1[6] = s * ( r1[6] - r2[6] * m1 );
    r1[7] = s * ( r1[7] - r2[7] * m1 );
    m0 = r0[2];
    r0[4] -= r2[4] * m0;
    r0[5] -= r2[5] * m0;
    r0[6] -= r2[6] * m0;
    r0[7] -= r2[7] * m0;
    m0 = r0[1];            /* now back substitute row 0 */
    s = 1.0f / r0[0];
    r0[4] = s * ( r0[4] - r1[4] * m0 );
    r0[5] = s * ( r0[5] - r1[5] * m0 );
    r0[6] = s * ( r0[6] - r1[6] * m0 );
    r0[7] = s * ( r0[7] - r1[7] * m0 );
    MAT(out, 0, 0) = r0[4];
    MAT(out, 0, 1) = r0[5];
    MAT(out, 0, 2) = r0[6];
    MAT(out, 0, 3) = r0[7];
    MAT(out, 1, 0) = r1[4];
    MAT(out, 1, 1) = r1[5];
    MAT(out, 1, 2) = r1[6];
    MAT(out, 1, 3) = r1[7];
    MAT(out, 2, 0) = r2[4];
    MAT(out, 2, 1) = r2[5];
    MAT(out, 2, 2) = r2[6];
    MAT(out, 2, 3) = r2[7];
    MAT(out, 3, 0) = r3[4];
    MAT(out, 3, 1) = r3[5];
    MAT(out, 3, 2) = r3[6];
    MAT(out, 3, 3) = r3[7];
    return 1;
}

void getViewporti( int *viewport, const JMATH::Rect2f& viewportRect ) {
    viewport[0] = static_cast<int>( viewportRect.topLeft()[0] );
    viewport[1] = static_cast<int>( viewportRect.topLeft()[1] );
    viewport[2] = static_cast<int>( viewportRect.width());
    viewport[3] = static_cast<int>( viewportRect.height());
}

void Frustum::calculateFromMVP( const V3f& cameraPos, const Matrix4f& viewMat, const Matrix4f& projMat,
                                const Rect2f& viewportRect ) {
    Matrix4f mat = viewMat * projMat;
    int viewport[4];
    getViewporti(viewport, viewportRect);

    l.n.setX(mat[3] + mat[0]);
    l.n.setY(mat[7] + mat[4]);
    l.n.setZ(mat[11] + mat[8]);
    l.d = mat[15] + mat[12];

    // Right Plane
    // col4 - col1
    r.n.setX(mat[3] - mat[0]);
    r.n.setY(mat[7] - mat[4]);
    r.n.setZ(mat[11] - mat[8]);
    r.d = mat[15] - mat[12];

    // Bottom Plane
    // col4 + col2
    b.n.setX(mat[3] + mat[1]);
    b.n.setY(mat[7] + mat[5]);
    b.n.setZ(mat[11] + mat[9]);
    b.d = mat[15] + mat[13];

    // Top Plane
    // col4 - col2
    t.n.setX(mat[3] - mat[1]);
    t.n.setY(mat[7] - mat[5]);
    t.n.setZ(mat[11] - mat[9]);
    t.d = mat[15] - mat[13];

    // Near Plane
    // col4 + col3
    n.n.setX(mat[3] + mat[2]);
    n.n.setY(mat[7] + mat[6]);
    n.n.setZ(mat[11] + mat[10]);
    n.d = mat[15] + mat[14];

    // Far Plane
    // col4 - mProjection
    f.n.setX(mat[3] - mat[2]);
    f.n.setY(mat[7] - mat[6]);
    f.n.setZ(mat[11] - mat[10]);
    f.d = mat[15] - mat[14];

    Vector2f p1 = viewportRect.topRight();
    Vector2f p2 = viewportRect.bottomRight();
    Vector2f p3 = viewportRect.topLeft();;
    Vector2f p4 = viewportRect.bottomLeft();

    float farPj[3] = { 0.0f, 0.0f, 0.0f };
    glhUnProjectf(p1.x(), p1.y(), 1.0f, viewMat.rawPtr(), projMat.rawPtr(), viewport, farPj);
    cornersFar[0] = V3f{ farPj };
    glhUnProjectf(p2.x(), p2.y(), 1.0f, viewMat.rawPtr(), projMat.rawPtr(), viewport, farPj);
    cornersFar[1] = V3f{ farPj };
    glhUnProjectf(p3.x(), p3.y(), 1.0f, viewMat.rawPtr(), projMat.rawPtr(), viewport, farPj);
    cornersFar[2] = V3f{ farPj };
    glhUnProjectf(p4.x(), p4.y(), 1.0f, viewMat.rawPtr(), projMat.rawPtr(), viewport, farPj);
    cornersFar[3] = V3f{ farPj };

    for ( int i = 0; i < 4; i++ ) {
        cornersFar[i] = viewMat * cornersFar[i];
    }
}

Camera::Camera( const std::string& cameraName, const Rect2f& _viewport ) : NamePolicy(cameraName) {

    ViewPort(_viewport);
    mTarget = V3fc::Z_AXIS;
    mNearClipPlaneZ = 0.05f;

    spatials.qangle = std::make_shared<AnimType<Quaternion>>(Quaternion{ V3fc::ZERO }, Name() + "_Angle");
    spatials.mPos = std::make_shared<AnimType<Vector3f>>(V3fc::ZERO, Name() + "_Pos");
    spatials.mFov = std::make_shared<AnimType<float>>(72.0f, Name() + "_Fov");

    mProjection.setPerspective(spatials.mFov->value, 1.0f, mNearClipPlaneZ, mFarClipPlaneZ);
    mOrthogonal.setOrthogonalProjection();
    mAspectRatio.setAspectRatioMatrix(mViewPort.ratio());
}

void Camera::setFoV( float fieldOfView ) {
    if ( mbLocked ) return;
    spatials.mFov->value = fieldOfView;
}

void Camera::setPosition( const Vector3f& pos ) {
    if ( mbLocked ) return;
    spatials.mPos->value = pos;
}

void Camera::setProjectionMatrix( float fovyInDegrees, float aspectRatio, float znear, float zfar ) {
    mProjection.setPerspective(fovyInDegrees, aspectRatio, znear, zfar);
    mFrustum.calculateFromMVP(spatials.mPos->value, mView, mProjection, mViewPort);
}

void Camera::setProjectionMatrix( const Matrix4f& val ) {
    mProjection = val;
    mFrustum.calculateFromMVP(spatials.mPos->value, mView, mProjection, mViewPort);
}

void Camera::translate( const Vector3f& pos ) {
    if ( mbLocked ) return;
    Vector3f mask = LockAtWalkingHeight() ? V3fc::MASK_UP_OUT : V3fc::ONE;
    if ( Mode() == CameraControlType::Orbit ) {
        mOrbitStrafe += pos;
    } else {
        setPosition(spatials.mPos->value + ( pos * mask ));
    }
}

void Camera::zoom2d( float amount ) {
    constexpr float UI_ZOOM_SCALER = 10.0f;

    if ( mbLocked ) return;
    amount /= -UI_ZOOM_SCALER;
    if ( Mode() == CameraControlType::Edit2d ) {
        spatials.mPos->value.setY(
                clamp(spatials.mPos->value.y() + amount, mNearClipPlaneZClampEdit2d, mFarClipPlaneZClampEdit2d));
    }
}

void Camera::moveUp( float amount ) {
    if ( amount == 0.0f || mbLocked ) return;
    Matrix4f invView;
    mView.invert(invView);
    Vector3f dir = invView.getRow(1).xyz();
    dir = normalize(dir);
    bool lSaveLock = LockAtWalkingHeight();
    LockAtWalkingHeight(false);
    translate(dir * -amount);
    LockAtWalkingHeight(lSaveLock);
}

void Camera::moveForward( float amount ) {
    if ( amount == 0.0f || mbLocked ) return;
    Matrix4f invView;
    mView.invert(invView);
    Vector3f dir = invView.getRow(2).xyz();
    dir = normalize(dir);
    translate(dir * -amount);
}

void Camera::strafe( float amount ) {
    if ( amount == 0.0f || mbLocked ) return;
    Matrix4f invView;
    mView.invert(invView);
    Vector3f dir = invView.getRow(0).xyz();
    dir = normalize(dir);
    translate(dir * -amount);
}

void Camera::setViewMatrix( const Vector3f& pos, const Quaternion& q ) {
    if ( mbLocked ) return;
    quatMatrix = q.rotationMatrix();// * Matrix4f( V3fc::ZERO, M_PI_2, V3fc::X_AXIS );

    quatMatrix.invert(quatMatrix);
    mView = Matrix4f(pos) * quatMatrix;
}

void Camera::setViewMatrixVR( const Vector3f& pos, const Quaternion& q, const Matrix4f& origRotMatrix ) {
    if ( mbLocked ) return;
    quatMatrix = q.rotationMatrix() * origRotMatrix;

    //	quatMatrix.invert(quatMatrix);
    mView = Matrix4f(pos) * quatMatrix;
}

void Camera::lookAt( const Vector3f& posAt ) {
    if ( mbLocked ) return;
    mTarget = posAt;
}

void Camera::lookAtCalc() {
    if ( mbLocked ) return;
    Vector3f z = normalize(spatials.mPos->value - mTarget);  // Forward
    Vector3f x = normalize(cross(V3fc::UP_AXIS, z)); // Right
    Vector3f y = cross(z, x);

    auto finalPos = spatials.mPos->value + mOrbitStrafe;
    mView = Matrix4f(Vector4f(x.x(), y.x(), z.x(), 0.0f),
                     Vector4f(x.y(), y.y(), z.y(), 0.0f),
                     Vector4f(x.z(), y.z(), z.z(), 0.0f),
                     Vector4f(-( dot(x, finalPos) ), -( dot(y, finalPos) ), -( dot(z, finalPos) ), 1.0f));
}

void Camera::center( const AABB& _bbox, CameraCenterAngle cca ) {
    if ( mbLocked ) return;
//    float aperture = ( tanf(degToRad(90.0f - ( spatials.mFov->value ))) ) / mViewPort.ratio();
    float aperture = ( tanf(degToRad(FoV() * 0.5f)) );

    float bdiameter = _bbox.calcDiameter();
//    mOrbitDistance = bdiameter * (aperture + (cos(degToRad(FoV())) * 0.5f) + mNearClipPlaneZ );
    mOrbitDistance = ( ( bdiameter * 0.5f ) / aperture ) + ( cos(degToRad(FoV())) * 0.5f ) + mNearClipPlaneZ;
    mOrbitStrafe = V3fc::ZERO;
    Vector3f cp = { 0.0f, 0.0f, mOrbitDistance };
    mTarget = _bbox.centre();

    if ( cca == CameraCenterAngle::Front ) {
        spatials.mPos->value = cp + _bbox.centre();
        spatials.qangle->value = Quaternion{ V3fc::ZERO };
        sphericalAcc = V2f{ 0.0f, M_PI_2 };
    } else if ( cca == CameraCenterAngle::Back ) {
        spatials.mPos->value = _bbox.centre() - cp;
        spatials.qangle->value = Quaternion{ M_PI, V3fc::UP_AXIS };
        sphericalAcc = V2f{ M_PI, M_PI_2 };
    } else if ( cca == CameraCenterAngle::Halfway || cca == CameraCenterAngle::HalfwayOpposite ) {
        float rangle = cca == CameraCenterAngle::Halfway ? M_PI_4 : M_PI_4 + M_PI * 2.0f;
        spatials.qangle->value = Quaternion{ rangle, V3fc::UP_AXIS } * Quaternion{ M_PI_4, V3fc::Z_AXIS };
        sphericalAcc = V2f{ TWO_PI - (float) rangle * 0.5f, (float) M_PI_4 + (float) M_PI_4 * 0.5f };
        if ( Mode() == CameraControlType::Orbit ) {
            computeOrbitPosition();
        } else {
            spatials.mPos->value = cp + _bbox.centre();
        }
    }
    if ( Mode() != CameraControlType::Orbit ) {
        spatials.qangle->value = Quaternion{ V3fc::ZERO };
    }

}

void Camera::pan( const Vector3f& posDiff ) {
    if ( mbLocked ) return;

    spatials.mPos->value += ( V3fc::X_AXIS * posDiff.x() );
    spatials.mPos->value += ( V3fc::Z_AXIS * posDiff.y() );
}

Vector3f Camera::center( const Rect2f& area, float slack ) {
    float mainSize = area.height() + slack;
    Vector2f center = area.centre();
    float aperture = ( tanf(degToRad(FoV() * 0.5f)) );
    float z = ( mainSize * 0.5f ) / aperture;
    V3f ret = XZY::C({ center, z });
//    LOGRS("Area: " << area << " Fov: " << FoV() << " slack: " << slack );
//    LOGRS("CameraCenter: " << ret );
    return ret;
}

Vector3f
Camera::centerScreenOnWithinArea( Vector2f area, const Rect2f& targetArea, const float padding, const float slack ) {
    area *= 1.0f + slack;
    float mainSize = ( area.ratio() > targetArea.aspectRatio() ) ? ( area.x() * 0.5f ) / targetArea.aspectRatio() : (
            area.y() * 0.5f );
    float aperture = ( tanf(degToRad(FoV())) );
    float xratio = getScreenAspectRatio / targetArea.width();
    float totalWidth = xratio * area.x();
    float origin = ( ( targetArea.origin().x() / getScreenAspectRatio ) * totalWidth ) + ( padding * area.x() );
    return { -totalWidth * 0.5f + origin + area.x() * slack * 0.5f / getScreenAspectRatio,
             targetArea.origin().y() * totalWidth + -area.y() * padding + area.y() * slack * 0.5f,
             -mainSize / aperture };
}

Matrix4f& Camera::MVP( const Matrix4f& model, CameraProjectionType cpType ) {
    mPrevMVP = mMVP;
    mMVP = model;
    switch ( cpType ) {
        case CameraProjectionType::Perspective:
            mMVP.mult(mView);
            mMVP.mult(mProjection);
            break;
        case CameraProjectionType::Orthogonal:
            mMVP.mult(mAspectRatio);
            mMVP.mult(mOrthogonal);
            break;

        default:
            break;
    }
    mView.invert(mInverseMV);

    return mMVP;
}

bool Camera::frustumClipping( const AABB& bbox ) const {
    // Indexed for the 'index trick' later
    Vector3f box[] = { bbox.minPoint(), bbox.maxPoint() };

    // We have 6 planes defining the frustum
    static const int NUM_PLANES = 6;
    const Plane3f *planes[NUM_PLANES] = { &mFrustum.n, &mFrustum.l, &mFrustum.r, &mFrustum.b, &mFrustum.t,
                                          &mFrustum.f };

    // We only need to do 6 point-plane tests
    for ( int i = 0; i < NUM_PLANES; ++i ) {
        // This is the current plane
        const Plane3f& p = *planes[i];

        // p-vertex selection (with the index trick)
        // According to the plane normal we can know the
        // indices of the positive vertex
        const int px = static_cast<int>( p.n.x() > 0.0f );
        const int py = static_cast<int>( p.n.y() > 0.0f );
        const int pz = static_cast<int>( p.n.z() > 0.0f );

        // Dot product
        // project p-vertex on plane normal
        // (How far is p-vertex from the origin)
        const float dp = dot(p.n, { box[px].x(), box[py].y(), box[pz].z() });
        //( p.n.x() *  ) +
        //( p.n.y() * box[py].y ) +
        //( p.n.z() *  box[pz].z );

        // Doesn't intersect if it is behind the plane
        if ( dp < -p.d ) {
            return false;
        }
    }
    return true;
}

PickRayData Camera::rayViewportPickIntersection( const Vector2f& p1 ) const {

    const float *matModelView = mView.rawPtr();
    const float *matProjection = mProjection.rawPtr();
    int viewport[4];
    getViewporti(viewport);

    float nearPj[3] = { 0.0f, 0.0f, 0.0f };
    float farPj[3] = { 0.0f, 0.0f, 0.0f };

    // get point on the 'near' plane (third param is set to 0.0f)
    glhUnProjectf(p1.x(), p1.y(), 0.0f, matModelView, matProjection, viewport, nearPj);

    // get point on the 'far' plane (third param is set to 1.0f)
    glhUnProjectf(p1.x(), p1.y(), 1.0f, matModelView, matProjection, viewport, farPj);

    return PickRayData{ V3f(nearPj), V3f(farPj) };
}

void Camera::calcProjectionMatrix() {
    mProjection.setPerspective(spatials.mFov->value, mViewPort.ratio() * mAspectRatioMultiplier, mNearClipPlaneZ,
                               mFarClipPlaneZ);
    mFrustum.calculateFromMVP(spatials.mPos->value, mView, mProjection, mViewPort);
}

void Camera::update() {

    Matrix4f oldViewMatrix = mView;
    Matrix4f oldProjectonMatrix = mProjection;

    if ( Mode() == CameraControlType::Edit2d ) {
        Quaternion qy(M_PI_2, V3fc::X_AXIS);
        quatMatrix = qy.rotationMatrix();
    }

    if ( Mode() == CameraControlType::Walk || Mode() == CameraControlType::Fly ) {
        quatMatrix = spatials.qangle->value.rotationMatrix();
    }

    if ( Mode() == CameraControlType::Orbit ) {
        lookAtCalc();
    }

    if ( Mode() != CameraControlType::Orbit ) {
        quatMatrix.setTranslation(spatials.mPos->value);
        quatMatrix.invert(mView);
    }

    if ( Mode() == CameraControlType::Edit2d ) {
        float vs = spatials.mPos->value.y();
        mProjection.setOrthogonalProjection(-0.5f * mViewPort.ratio() * vs, 0.5f * mViewPort.ratio() * vs,
                                            -0.5f * vs, 0.5f * vs);
    } else {
        calcProjectionMatrix();
    }

    mFrustum.calculateFromMVP(spatials.mPos->value, mView, mProjection, mViewPort);

    setDirty(oldViewMatrix != mView || oldProjectonMatrix != mProjection);
}

Vector2f Camera::mousePickRayOrtho( const Vector2f& _pos ) {
    auto vc = mViewPort.normalizeWithinRect(_pos);
    auto p = getPosition();
    float z = p.z();
    auto v2p = Vector2f{ z * mViewPort.ratio(), z };

    auto vf = ( vc * v2p ) - ( v2p * 0.5f ) + p.xy();
    return vf;
}

std::ostream& operator<<( std::ostream& os, const Camera& camera ) {
    os << std::endl
       << "mMode: " << static_cast<uint64_t>(camera.mMode) << std::endl
       << "spatials.mPos: " << camera.spatials.mPos->value << std::endl
       << "spatials.qangle: " << camera.spatials.qangle->value << std::endl
       << "quatMatrix: " << camera.quatMatrix << std::endl
       << "mView: " << camera.mView << std::endl
       << "mProjection: " << camera.mProjection << std::endl
       << "mTarget: " << camera.mTarget << std::endl
       << "mVP: " << camera.mVP << std::endl
       << "mMVP: " << camera.mMVP << std::endl
       << "mInverseMV: " << camera.mInverseMV << std::endl
       << "mPrevMVP: " << camera.mPrevMVP << std::endl
       << "spatials.mFov: " << camera.spatials.mFov->value << std::endl
       << "mOrthogonal: " << camera.mOrthogonal << std::endl
       << "mAspectRatio: " << camera.mAspectRatio << std::endl
       << "mScreenAspectRatio: " << camera.mScreenAspectRatio << std::endl
       << "mAspectRatioMultiplier: " << camera.mAspectRatioMultiplier << std::endl
       << "mNearClipPlaneZ: " << camera.mNearClipPlaneZ << std::endl
       << "mFarClipPlaneZ: " << camera.mFarClipPlaneZ << std::endl
       << "mNearClipPlaneZClampEdit2d: " << camera.mNearClipPlaneZClampEdit2d << std::endl
       << "mFarClipPlaneZClampEdit2d: " << camera.mFarClipPlaneZClampEdit2d << std::endl
       << "mLockAtWalkingHeight: " << camera.mLockAtWalkingHeight << std::endl
       << "mbLocked: " << camera.mbLocked << std::endl
       << "sphericalAcc: " << camera.sphericalAcc << std::endl
       << "mOrbitStrafe: " << camera.mOrbitStrafe << std::endl
       << "mOrbitDistance: " << camera.mOrbitDistance << std::endl;// << " mViewPort: " << camera.mViewPort;
    return os;
}

float Camera::FoV() const {
    return spatials.mFov->value;
}

floata& Camera::FoVAnim() {
    return spatials.mFov;
}

Vector3f Camera::getPosition() const {
    return spatials.mPos->value;
}

Vector3f Camera::getPositionRH() const {
    Vector3f lPos = spatials.mPos->value.xzy();
    lPos.invZ();
    return lPos;
}

void Camera::setQuat( const Quaternion& a ) {
    if ( mbLocked ) return;
    spatials.qangle->value = a;
}

void Camera::incrementOrbitDistance( float _d ) {
    if ( _d == 0.0f ) return;

    mOrbitDistance += _d;
    if ( mOrbitDistance < mNearClipPlaneZ * 2.0f ) {
        mOrbitDistance = mNearClipPlaneZ * 2.0f;
    }
    computeOrbitPosition();
}

void Camera::incrementSphericalAngles( const V2f& _sph ) {
    sphericalAcc -= _sph * 5.0f;
    float sphericalSafeAntiGLockClamp = 0.02f;

    if ( sphericalAcc.y() <= sphericalSafeAntiGLockClamp ) sphericalAcc.setY(sphericalSafeAntiGLockClamp);
    if ( sphericalAcc.y() >= M_PI - sphericalSafeAntiGLockClamp )
        sphericalAcc.setY(M_PI - sphericalSafeAntiGLockClamp);

    if ( sphericalAcc.x() <= 0.0f ) sphericalAcc.setX(TWO_PI - 0.0001f);
    if ( sphericalAcc.x() >= TWO_PI ) sphericalAcc.setX(0.0f);

    computeOrbitPosition();
}

void Camera::computeOrbitPosition() {
    auto stc = sphericalToCartesian(V3f{ sphericalAcc, mOrbitDistance });
    setPosition(mTarget + XZY::C(stc));
}

void Camera::resetQuat() {
    spatials.qangle->value = Quaternion{ V3fc::ZERO };
}

Quaternion Camera::quatAngle() const { return spatials.qangle->value; }

V3fa& Camera::PosAnim() { return spatials.mPos; }

Quaterniona& Camera::QAngleAnim() { return spatials.qangle; }

TimelineSet Camera::addKeyFrame( const std::string& _name, float _time ) {
    TimelineSet ret{};

//    Timeline::add( _name, PosAnim(),    KeyFramePair{_time, getPosition() } );
//    Timeline::add( _name, QAngleAnim(), KeyFramePair{_time, quatAngle()   } );

    return ret;
}

void Camera::getViewporti( int *viewport ) const {
    viewport[0] = static_cast<int>( ViewPort().topLeft()[0] );
    viewport[1] = static_cast<int>( ViewPort().topLeft()[1] );
    viewport[2] = static_cast<int>( ViewPort().width());
    viewport[3] = static_cast<int>( ViewPort().height());
}

void Camera::ViewPort( JMATH::Rect2f val ) {
    mViewPort = val;
    mScreenAspectRatio.setAspectRatioMatrixScreenSpace(mViewPort.ratio());
}

Vector3f Camera::getYawVector() const {
    return mView.getCol(0).xyz();
}

Vector3f Camera::getUpVector() const {
    return mView.getCol(1).xyz();
}

Vector3f Camera::getDirection() const {
    return mView.getCol(2).xyz();
}

Vector3f Camera::getDirectionRH() const {
    Vector3f ret = mView.getCol(2).xzy();
    ret.setX(-ret.x());
    return ret;
}

void Camera::AspectRatioMultiplier( float val ) {
    mAspectRatioMultiplier = val;
}

void Camera::Mode( const CameraControlType& val ) {
    mMode = val;
}

CameraControlType Camera::Mode() const {
    return mMode;
}

Vector4f Camera::getNearFar() const {
    return V4f{ mNearClipPlaneZ, mFarClipPlaneZ, spatials.mFov->value, mFarClipPlaneZ*0.95f };
}

std::vector<V3f> Camera::frustumFarViewPort() const {
    std::vector<V3f> ret;
    for ( int t = 0; t < 4; t++ ) ret.emplace_back(mFrustum.cornersFar[t]);
    return ret;
}

Matrix4f Camera::getViewMatrix() const {
    return mView;
}

Matrix4f Camera::getProjectionMatrix() const {
    return mProjection;
}

Matrix4f Camera::getInverseMV() const {
    return mInverseMV;
}

Matrix4f Camera::getPrevMVP() const {
    return mPrevMVP;
}

bool Camera::isDoom() const {
    return Mode() == CameraControlType::Walk;
}

const CameraSpatials& Camera::getSpatials() const {
    return spatials;
}

void Camera::enableInputs( bool _enableInputs ) {
    mbLocked = !_enableInputs;
}

bool Camera::areInputsEnabled() const {
    return !mbLocked;
}

bool Camera::LockAtWalkingHeight() const { return mLockAtWalkingHeight; }
void Camera::LockAtWalkingHeight( bool val ) { mLockAtWalkingHeight = val; }
void Camera::LockScrollWheelMovements( bool _val ) { mbLockScrollWheelMovements = _val; }
bool Camera::areScrollWheelMovementsLocked() const { return mbLockScrollWheelMovements; }

void Camera::setNearClipPlane( float _value ) {
    mNearClipPlaneZ = _value;
    calcProjectionMatrix();
}

void Camera::setFarClipPlane( float _value ) {
    mFarClipPlaneZ = _value;
    calcProjectionMatrix();
}

void Camera::setNearFarClipPlane( float _near, float _far ) {
    mNearClipPlaneZ = _near;
    mFarClipPlaneZ = _far;
    calcProjectionMatrix();
}


