#pragma once

#include <mutex>
#include <atomic>
#include <memory>

class HierGeom;

class  GeometryOffset {
public:
	GeometryOffset( std::shared_ptr<HierGeom> g, size_t os, size_t l ) :geometry{ g }, offset{ os }, length{ l }, totalDone{ 0 } {}

	bool incrementDone( size_t howMany );
	bool ensureScheduledForRelighting();

public:
	std::shared_ptr<HierGeom> geometry;
	size_t offset;
	size_t length;

private:

	bool scheduledToBeRelighted = false;
	std::atomic_size_t totalDone;
};
