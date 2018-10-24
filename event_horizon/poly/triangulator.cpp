//
//  triangulator.cpp
//  sixthmaker
//
//  Created by Dado on 11/03/2017.
//
//

#include "triangulator.hpp"

Triangulator::Triangulator( const vector2fList& _verts, float _accuracy ) {
	vector2fList lverts = _verts;
	removeCollinear( lverts, _accuracy );
	init2d( lverts.data(), static_cast<int>( lverts.size() ) );
}

Triangulator::Triangulator( const Vector3f* _verts, const size_t vsize, const Vector3f& normal, float _accuracy ) {
	dominantPair = normal.dominantPair();

	for ( size_t t = 0; t < vsize; t++ ) {
		m2dCollinearCheck.push_back( _verts[t].pairMapped( dominantPair ) );
		mVHash[m2dCollinearCheck[t].hash()] = _verts[t];
	}

	removeCollinear( m2dCollinearCheck, _accuracy );

	init3d( m2dCollinearCheck.data(), static_cast<int>( m2dCollinearCheck.size() ) );
}

Triangulator::Triangulator( const vector3fList& _verts, const Vector3f& normal, float _accuracy ) {
	dominantPair = normal.dominantPair();

	for ( size_t t = 0; t < _verts.size(); t++ ) {
		m2dCollinearCheck.push_back( _verts[t].pairMapped( dominantPair ) );
		mVHash[m2dCollinearCheck[t].hash()] = _verts[t];
	}

	removeCollinear( m2dCollinearCheck, _accuracy );

	init3d( m2dCollinearCheck.data(), static_cast<int>( m2dCollinearCheck.size() ) );
}

void Triangulator::init2d( const Vector2f* _verts, const int vsize ) {
	if ( vsize >= 3 ) {
		mCDT = std::make_unique<p2t::CDT>( _verts, vsize );
		mCDT->Triangulate();

		// check if all corners are 90 degree
		//bool all90 = true;
		//for ( auto q = 0; q < vsize; q++ ) {
		//	if ( !isScalarEqual( dot( _verts[q] - _verts[getCircularArrayIndex( q - 1, vsize )], _verts[q] - _verts[getCircularArrayIndex( q + 1, vsize )] ), 0.0f ) ) {
		//		all90 = false;
		//		break;
		//	}
		//}

		//if ( all90 ) {
		//	int a = 1;
		//}

		gather2dTriangularizationResult();
	}
}

void Triangulator::init3d( const Vector2f* _verts, const int vsize ) {
	if ( vsize >= 3 ) {
		mCDT = std::make_unique<p2t::CDT>( _verts, vsize );
		mCDT->Triangulate();
		gather3dTriangularizationResult();
	}
}

void Triangulator::gather2dTriangularizationResult() {
	std::vector<p2t::Triangle*> triangles = mCDT->GetTriangles();

	for ( size_t t = 0; t < triangles.size(); t++ ) {
		std::vector<Vector2f> vts;
		for ( int32_t j = 2; j >= 0; j-- ) {
			p2t::Point* p = triangles[t]->GetPoint( j );
			vts.push_back( { p->x, p->y } );
		}
		m2dTriangulationTuples.push_back( std::make_tuple( vts[0], vts[1], vts[2] ) );
		m2dTriangulation.push_back( vts );
	}
}

void Triangulator::gather3dTriangularizationResult() {
	std::vector<p2t::Triangle*> triangles = mCDT->GetTriangles();

	for ( size_t t = 0; t < triangles.size(); t++ ) {
		std::vector<Vector3f> vts;
		for ( int32_t j = 2; j >= 0; j-- ) {
			p2t::Point* p = triangles[t]->GetPoint( j );
			if ( mVHash.find( Vector2f( p->x, p->y ).hash() ) == mVHash.end() ) {
				Vector3f steinerPoint = Vector3f::HUGE_VALUE_POS;
				steinerPoint[dominantPair.first] = static_cast<float>( p->x );
				steinerPoint[dominantPair.second] = static_cast<float>( p->y );
				steinerPoint[steinerPoint.dominantElement()] = mVHash.begin()->second[steinerPoint.dominantElement()];
				vts.push_back( steinerPoint );
			} else {
				vts.push_back( mVHash[Vector2f( p->x, p->y ).hash()] );
			}
		}
		m3dTriangulationTuples.push_back( std::make_tuple( vts[0], vts[1], vts[2] ) );
		m3dTriangulation.push_back( vts );
	}
}

void Triangulator::gather3dTriangularizationWithNeighbours(std::vector<std::vector<Vector2f>>& tri_neighbours ) {
	std::vector<p2t::Triangle*> triangles = mCDT->GetTriangles();

	for ( size_t t = 0; t < triangles.size(); t++ ) {
		std::vector<Vector2f> vts;
		for ( int32_t j = 2; j >= 0; j-- ) {
			p2t::Point* p = triangles[t]->GetPoint( j );
			vts.push_back( { p->x, p->y } );
		}

		for ( int32_t j = 2; j >= 0; j-- ) {
			p2t::Triangle* neighbour = triangles[t]->GetNeighbor( j );
			if ( neighbour ) {
				for ( int32_t m = 2; m >= 0; m-- ) {
					p2t::Point* p = neighbour->GetPoint( m );
					vts.push_back( { p->x, p->y } );
				}
			}
		}
		tri_neighbours.push_back( vts );
	}
}

std::vector<vector2fList>& Triangulator::get2dTrianglesList() {
	return m2dTriangulation;
}

std::vector<Triangle2d>& Triangulator::get2dTrianglesTuple() {
	return m2dTriangulationTuples;
}

std::vector<vector3fList>& Triangulator::get3dTrianglesList() {
	return m3dTriangulation;
}

std::vector<Triangle3d>& Triangulator::get3dTrianglesTuple() {
	return m3dTriangulationTuples;
}