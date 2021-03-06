#pragma once

#include <zlib.h>
#include <core/htypes_shared.hpp>

namespace zlibUtil {

    [[nodiscard]] std::string rawb64gzip( const SerializableContainer& _raw );

    // Decompress
    SerializableContainer inflateFromMemory( uint8_p&& fin );
    // Compress
    SerializableContainer deflateMemory( const uint8_p&& source, int level = Z_DEFAULT_COMPRESSION );
    SerializableContainer deflateMemory( const std::string& source, int level = Z_DEFAULT_COMPRESSION );
}