#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <cmath>

#define TINYGLTF_IMPLEMENTATION
#define TINY_DNG_LOADER_IMPLEMENTATION

#include <iostream>
#include <tinygltf/include/tiny_dng_loader.h>
#include <tinygltf/include/tiny_gltf.h>
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.

#include <core/http/webclient.h>
#include <core/util.h>
#include <core/file_manager.h>
#include <core/runloop_core.h>
#include <core/tar_util.h>
#include <core/zlib_util.h>
#include <core/names.hpp>
#include <core/descriptors/archives.h>
#include <core/resources/publisher.hpp>
#include <core/resources/material.h>
#include <core/resources/resource_utils.hpp>
#include <core/descriptors/uniform_names.h>
#include <core/resources/resource_builder.hpp>
#include <core/resources/resource_pipe.hpp>
#include <core/resources/publisher.hpp>

#include <database/nosql/mongo/mongo.hpp>

#include <stb/stb_image.h>
#include <stb/stb_image_resize.h>
#include <core/image_util.h>
#include <core/profiler.h>
#include <ostream>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

// mongod --port 27017 --replSet rs0
// sudo brew services restart nginx
// /usr/loca/var/log/nginx/ tail -f aacess.log

void initDeamon() {
    /* Our process ID and Session ID */
    pid_t pid, sid;

    /* Fork off the parent process */
    pid = fork();
    if ( pid < 0 ) {
        exit( EXIT_FAILURE );
    }
    /* If we got a good PID, then
       we can exit the parent process. */
    if ( pid > 0 ) {
        exit( EXIT_SUCCESS );
    }

    /* Change the file mode mask */
    umask( 0 );

    /* Open any logs here */

    /* Create a new SID for the child process */
    sid = setsid();
    if ( sid < 0 ) {
        /* Log the failure */
        exit( EXIT_FAILURE );
    }
    /* Change the current working directory */
    if (( chdir( "/" )) < 0 ) {
        /* Log the failure */
        exit( EXIT_FAILURE );
    }

    /* Close out the standard file descriptors */
    close( STDIN_FILENO );
    close( STDOUT_FILENO );
    close( STDERR_FILENO );
}

struct DaemonFileStruct {
    MongoBucket bucket;
    std::string filename;
    strview project;
    strview uname;
    strview uemail;
    std::string thumb64{};
    std::vector<ArchiveDirectoryEntityElement> candidates{};
};

class DaemonException : public std::exception {
public:
    DaemonException( const std::string& msg ) : msg( msg ) {}

    virtual const char *what() const throw() {
        return msg.c_str();
    }

private:
    std::string msg{};
};

static std::vector<std::string> getExtForGroup( const std::string& _group ) {
    std::unordered_map<std::string, std::vector<std::string>> extmap;

    extmap[ResourceGroup::Geom] = { ".fbx", ".glb", ".gltf", ".obj" };
    extmap[ResourceGroup::Material] = { ".sbsar", ".png", ".jpg" };

    if ( auto it = extmap.find( _group ); it != extmap.end()) {
        return it->second;
    }
    return {};
}

void daemonExceptionLog( const std::exception& e ) {
    LOGRS( e.what());
    Socket::emit( "daemonLogger", serializeLogger( LoggerLevel::Error, e.what()));
}

void daemonWarningLog( const std::string& e ) {
    LOGRS( e );
    Socket::emit( "daemonLogger", serializeLogger( LoggerLevel::Warning, e ));
}

std::optional<MongoFileUpload> elaborateImage(
        int desiredWidth, int desiredHeight,
        DaemonFileStruct dfs,
        std::string& thumb ) {
    try {
        int w = 0, h = 0, n = 0;
        int thumbSize = 128;

        unsigned char *input_data = stbi_load( dfs.filename.c_str(), &w, &h, &n, 0 );

        auto output_data = make_uint8_p( desiredWidth * desiredHeight * n );
        auto output_data_thumb = make_uint8_p( thumbSize * thumbSize * n );
        stbir_resize_uint8( input_data, w, h, 0, output_data.first.get(), desiredWidth, desiredHeight, 0, n );
        stbir_resize_uint8( output_data.first.get(), desiredWidth, desiredHeight, 0, output_data_thumb.first.get(),
                            thumbSize, thumbSize, 0, n );

        auto bm = imageUtil::bufferToMemoryCompressed( desiredWidth, desiredHeight, n, output_data.first.get(),
                                                       imageUtil::extToMime( getFileNameExt( dfs.filename )));
        auto bm64 = imageUtil::bufferToPng64( thumbSize, thumbSize, n, output_data_thumb.first.get());
        if ( MPBRTextures::isBaseColorTexture( dfs.filename )) {
            thumb = bm64;
        }
        return Mongo::fileUpload( dfs.bucket, getFileName( dfs.filename ), std::move( bm ),
                                  Mongo::FSMetadata( ResourceGroup::Image, dfs.project, dfs.uname, dfs.uemail,
                                                     HttpContentType::octetStream,
                                                     MD5( bm.first.get(), bm.second ).hexdigest(), bm64 ));
    } catch ( const std::exception& e ) {
        LOGRS( e.what());
        return std::nullopt;
    }
}

ArchiveDirectory generateActiveDirectoryFromSBSARTempFiles( const std::string& fn ) {
    ArchiveDirectory ret{ fn };
    auto fileRoot = getDaemonRoot();

    for ( const auto& output : MPBRTextures::SBSARTextureOutputs()) {
        std::string outFilename{};
        outFilename.append( fn ).append( "_" ).append( output ).append( ".png" );
        if ( FM::fileExist( fileRoot + outFilename )) {
            ret.insert( { outFilename, 0, output } );
        }
    }
    return ret;
}

ArchiveDirectory mapActiveDirectoryFilesToPBR( DaemonFileStruct dfs ) {
    ArchiveDirectory ad{dfs.filename};

    for ( auto&& elem : dfs.candidates ) {
        elem.metaString = MPBRTextures::findTextureInString( elem.name );
        ad.insert( std::move(elem) );
    }
    return ad;
}

ResourceEntityHelper elaborateInternalMaterial(
        const ArchiveDirectory& ad,
        int nominalSize,
        DaemonFileStruct dfs ) {
    ResourceEntityHelper mat{};
    auto fileRoot = getDaemonRoot();

    Material pbrmaterial{ S::SH, ad.Name() };
    for ( const auto& output : ad ) {
        std::string fullFileName = fileRoot + output.second.name;
        dfs.filename = fullFileName;
        auto to = elaborateImage( nominalSize, nominalSize, dfs, mat.thumb );
        if ( to && !output.second.metaString.empty()) {
            auto tid = ( *to ).getStringId();
            mat.deps[ResourceGroup::Image].emplace_back( tid );
            pbrmaterial.Values()->assign( MPBRTextures::mapToTextureUniform( output.second.metaString ), tid );
            if ( MPBRTextures::isMetallicTexture( output.second.metaString )) {
                pbrmaterial.setMetallicValue( 1.0f );
            }
            if ( MPBRTextures::isRoughnessTexture( output.second.metaString )) {
                pbrmaterial.setRoughnessValue( 1.0f );
            }
        }
    }

    // Create PBR material
    mat.sc = pbrmaterial.serialize();
    return mat;
}

ArchiveDirectory elaborateMatSBSAR(
        int size,
        DaemonFileStruct dfs ) {

    try {
        auto fileRoot = getDaemonRoot();

        std::string fn = getFileNameOnly( dfs.filename );
        std::string fext = ".png";

        std::string sizeString = std::to_string( log2( size ));
        std::string sbRender = "/opt/Allegorithmic/Substance_Automation_Toolkit/sbsrender render --inputs "
                               + fileRoot + dfs.filename +
                               " --set-value '$outputsize@" + sizeString + "," + sizeString +
                               "' --output-bit-depth \"8\" --png-format-compression best_compression "
                               "--output-name {inputName}_{outputNodeName}";
        for ( const auto& output : MPBRTextures::SBSARTextureOutputs()) {
            sbRender.append( " --input-graph-output " + output );
        }
        sbRender.append( " --output-path " + fileRoot );;

        std::system( sbRender.c_str());

        return generateActiveDirectoryFromSBSARTempFiles( fn );

    } catch ( const std::exception& e ) {
        daemonExceptionLog( e );
        return ArchiveDirectory{};
    }
}

int resaveGLB( const std::string& filename ) {
    using namespace tinygltf;

    Model model;
    TinyGLTF loader;
    std::string err;
    std::string warn;

    bool ret = loader.LoadBinaryFromFile( &model, &err, &warn, filename.c_str());

    if ( !warn.empty() || !err.empty() || !ret ) {
        if ( !warn.empty()) {
            printf( "Warn: %s\n", warn.c_str());
        }
        if ( !err.empty()) {
            printf( "Err: %s\n", err.c_str());
        }
        if ( !ret ) {
            printf( "Failed to parse glTF\n" );
            return -1;
        }
    }

    for ( auto& it : model.accessors ) {
        if ( string_ends_with( it.name, "_positions" ) && it.componentType == 5126 && it.type == 3 ) {
            auto buffView = model.bufferViews[it.bufferView];
            if ( buffView.target == 34962 ) {
                auto *buffer = &model.buffers[buffView.buffer];
                auto *buffArray = buffer->data.data() + buffView.byteOffset + it.byteOffset;
                V3f *buffV3f = reinterpret_cast<V3f *>(buffArray);
                for ( int t = 0; t < it.count; t++ ) {
                    buffV3f[t] *= 0.01f;
                }
            }
            for ( auto& v : it.maxValues ) v *= 0.01f;
            for ( auto& v : it.minValues ) v *= 0.01f;
        }
    }

    loader.WriteGltfSceneToFile( &model, filename, true, true, true, true );

    return 0;
}

void elaborateGeomFBX( DaemonFileStruct dfs ) {
    try {
        auto dRoot = getDaemonRoot();
        auto fn = getFileNameOnly( dfs.filename );
        std::string filenameglb = fn + ".glb";

        std::string cmd =
                "cd " + dRoot + " && FBX2glTF -b --pbr-metallic-roughness -o '" + getFileNameOnly( filenameglb ) +
                "' '" +
                getFileName( dfs.filename ) + "'";
        auto ret = std::system( cmd.c_str());
        if ( ret != 0 ) throw DaemonException{ std::string{ "FBX elaboration return code: " + std::to_string( ret ) }};

        auto fileData = FM::readLocalFile( dRoot + filenameglb );
//        resaveGLB(dRoot + filenameglb);
        auto fileHash = Hashable<>::hashOf( fileData );
        Mongo::fileUpload( dfs.bucket, filenameglb, std::move( fileData ),
                           Mongo::FSMetadata( ResourceGroup::Geom, dfs.project, dfs.uname, dfs.uemail,
                                              HttpContentType::json, fileHash, dfs.thumb64, ResourceDependencyDict{} ));
    } catch ( const std::exception& e ) {
        daemonExceptionLog( e );
    }
}

void elaborateGeomObj( DaemonFileStruct dfs ) {
    try {
        auto dRoot = getDaemonRoot();
        auto fn = getFileNameOnly( dfs.filename );
        std::string filenameglb = fn + ".glb";

        std::string cmd =
                "cd " + dRoot + " && obj2glTF -i '" + getFileName( dfs.filename ) + "'  -o '" + filenameglb + "'";
        auto ret = std::system( cmd.c_str());
        if ( ret != 0 ) throw DaemonException{ std::string{ "OBJ elaboration return code: " + std::to_string( ret ) }};

        auto fileData = FM::readLocalFile( dRoot + filenameglb );
        resaveGLB( dRoot + filenameglb );
        fileData = FM::readLocalFile( dRoot + filenameglb );
        auto fileHash = Hashable<>::hashOf( fileData );
        Mongo::fileUpload( dfs.bucket, filenameglb, std::move( fileData ),
                           Mongo::FSMetadata( ResourceGroup::Geom, dfs.project, dfs.uname, dfs.uemail,
                                              HttpContentType::json, fileHash, "", ResourceDependencyDict{} ));
    } catch ( const std::exception& e ) {
        daemonExceptionLog( e );
    }
}

void elaborateGeomGLB( DaemonFileStruct dfs ) {
    try {
        auto dRoot = getDaemonRoot();
        auto fileData = FM::readLocalFile( dRoot + dfs.filename );
        auto fileHash = Hashable<>::hashOf( fileData );
        Mongo::fileUpload( dfs.bucket, dfs.filename, std::move( fileData ),
                           Mongo::FSMetadata( ResourceGroup::Geom, dfs.project, dfs.uname, dfs.uemail,
                                              HttpContentType::json, fileHash, "", ResourceDependencyDict{} ));
    } catch ( const std::exception& e ) {
        daemonExceptionLog( e );
    }
}

void elaborateGeom( DaemonFileStruct dfs ) {
    if ( getFileNameExt( dfs.filename ) == ".fbx" ) {
        elaborateGeomFBX( dfs );
    }
    if ( getFileNameExt( dfs.filename ) == ".obj" ) {
        elaborateGeomObj( dfs );
    }
    if ( getFileNameExt( dfs.filename ) == ".glb" || getFileNameExt( dfs.filename ) == ".gltf" ) {
        elaborateGeomGLB( dfs );
    }
}

void elaborateMaterial( DaemonFileStruct dfs ) {
    ArchiveDirectory ad{ dfs.filename };
    int nominalSize = 2048;

    if ( getFileNameExt( std::string( dfs.filename )) == ".zip" ) {
        ad = mapActiveDirectoryFilesToPBR( dfs );
    }
    if ( getFileNameExt( std::string( dfs.filename )) == ".sbsar" ) {
        ad = elaborateMatSBSAR( nominalSize, dfs );
    }

    // Gather texture outputs
    ResourceEntityHelper mat = elaborateInternalMaterial( ad, 512, dfs );
    Mongo::fileUpload( dfs.bucket, dfs.filename, mat.sc,
                       Mongo::FSMetadata( ResourceGroup::Material, dfs.project, dfs.uname, dfs.uemail,
                                          HttpContentType::json, Hashable<>::hashOf( mat.sc ), mat.thumb,
                                          mat.deps ));

    // Clean up
//    std::string cleanup = "cd " + fileRoot + " && rm " + fn + "*";
//    std::system( cleanup.c_str());
}

void findCandidatesScreenshotForThumbnail( DaemonFileStruct& dfs, const ArchiveDirectory& ad, strview group ) {
    if ( group == ResourceGroup::Geom ) {
        auto candidateScreenshot = ad.findFilesWithExtension( { ".jpg" } );
        for ( const auto& elem : candidateScreenshot ) {
            if ( elem.name.find( "preview_" ) != std::string::npos ||
                 elem.name.find( "_preview" ) != std::string::npos ) {
                dfs.thumb64 = imageUtil::makeThumbnail64( getDaemonRoot() + elem.name );
            }
        }
    }
}

void geomFilterOutSameAssetDifferentFormatFromBasePriority(
        const std::string& filename,
        const std::string& formatPriority,
        const std::vector<ArchiveDirectoryEntityElement>& sourceCandidates,
        std::vector<ArchiveDirectoryEntityElement>& destCandidates ) {
    if ( filename.find( formatPriority ) != std::string::npos ) {
        for ( const auto& elem2 : sourceCandidates ) {
            if ( getFileNameNoExt( elem2.name ) == filename && filename != elem2.name ) {
                erase_if( destCandidates, [elem2]( const auto& us ) { return us.name == elem2.name; } );
            }
        }
    }
}

void geomFilterDesingConnectCrazyRedundancy(
        const std::string& filename,
        std::vector<ArchiveDirectoryEntityElement>& destCandidates ) {

    auto removeDCCrazyDoubles = [&]( const std::string& _source, const std::string& _d1, const std::string& _d2 ) {
        if ( auto pos = filename.find( _source ); pos != std::string::npos ) {
            auto fn2 = filename.substr( 0, pos );
            erase_if( destCandidates, [fn2, _d1, _d2]( const auto& us ) {
                bool sfn = us.name.find( fn2 ) == 0;
                return sfn && ( us.name.find( _d1 ) != std::string::npos ||
                                us.name.find( _d2 ) != std::string::npos );
            } );
        }
    };

    removeDCCrazyDoubles( "_fbx_upY.fbx", "_fbx_upZ.fbx", "_obj.obj" );
    removeDCCrazyDoubles( "_fbx_upZ.fbx", "_fbx_upY.fbx", "_obj.obj" );
}

void materialFilterNonImageAssets(
        const std::string& filename,
        std::vector<ArchiveDirectoryEntityElement>& destCandidates ) {

    if ( !nameHasImageExtension( filename )) {
        erase_if( destCandidates, [filename]( const auto& us ) {
            return us.name == filename;
        } );
    }
}

std::vector<ArchiveDirectoryEntityElement>
filterCandidates( const std::vector<ArchiveDirectoryEntityElement>& candidates, strview group ) {
    auto filteredCandidates = candidates;
    for ( const auto& elem : candidates ) {
        if ( group == ResourceGroup::Geom ) {
            auto fn = getFileNameNoExt( elem.name );
            geomFilterOutSameAssetDifferentFormatFromBasePriority( fn, ".fbx", candidates, filteredCandidates );
            geomFilterDesingConnectCrazyRedundancy( elem.name, filteredCandidates );
        } else if ( group == ResourceGroup::Material ) {
            materialFilterNonImageAssets( elem.name, filteredCandidates );
        }
    }

    return filteredCandidates;
}

void elaborateAsset( DaemonFileStruct& dfs, const std::string& assetName, strview group ) {
    dfs.filename = assetName;
    if ( group == ResourceGroup::Geom ) {
        elaborateGeom( dfs );
    }
    if ( group == ResourceGroup::Material ) {
        elaborateMaterial( dfs );
    }
}

bool groupIsAchievable( const std::string& sourceName, strview group ) {
    return group == ResourceGroup::Material && isFileExtCompressedArchive( sourceName );
}

void elaborateCandidates( DaemonFileStruct& dfs,
                          strview group ) {
    if ( groupIsAchievable( dfs.filename, group )) {
        elaborateAsset( dfs, dfs.filename, group );
    } else {
        for ( const auto& elem : dfs.candidates ) {
            elaborateAsset( dfs, elem.name, group );
        }
    }
}

uint64_t chooseMainArchiveFilename( const ArchiveDirectory& ad, DaemonFileStruct& dfs, strview group ) {

    findCandidatesScreenshotForThumbnail( dfs, ad, group );

    auto candidates = ad.findFilesWithExtension( getExtForGroup( std::string{ group } ));
    dfs.candidates = filterCandidates( candidates, group );
    elaborateCandidates( dfs, group );

    return dfs.candidates.size();
}

void parseElaborateStream( mongocxx::change_stream& stream, MongoBucket sourceAssetBucket, MongoBucket entityBucket ) {

    try {
        for ( auto change : stream ) {
            StreamChangeMetadata meta{ change };
            Profiler p1{ "elaborate time:" };
            auto filename = std::string( meta.filename );
            auto fileDownloaded = Mongo::fileDownload( sourceAssetBucket,
                                                       meta.id,
                                                       getDaemonRoot() + std::string{ filename } );

            DaemonFileStruct dfs{ entityBucket, getFileName( fileDownloaded ), meta.project, meta.username,
                                  meta.useremail };

            ArchiveDirectory ad{ filename };
            // First unzip all the content if package arrives in a zip file
            if ( isFileExtCompressedArchive( std::string( filename ))) {
                unzipFilesToTempFolder( fileDownloaded, ad );
                auto numElaborated = chooseMainArchiveFilename( ad, dfs, meta.group );
                if (( dfs.filename.empty() || numElaborated == 0 )) {
                    daemonWarningLog( std::string( meta.filename ) + " does not contain any appropriate asset file" );
                    continue;
                }
            } else {
                elaborateAsset( dfs, filename, meta.group );
            }
        }
    } catch ( const std::exception& e ) {
        daemonExceptionLog( e );
    }
}

void parseAssetStream( Mongo& mdb, mongocxx::change_stream& stream ) {
    for ( auto change : stream ) {
        StreamChangeMetadata meta{ change };
        mdb.insertEntityFromAsset( meta );
    }
}

int main( [[maybe_unused]] int argc, [[maybe_unused]] char **argv ) {

    LOGRS( "Daemon version 3.0.2" );

    if ( !Http::useClientCertificate( true,
                                      "EH_DEAMON_CERT_KEY_PATH", "EH_DEAMON_CERT_CRT_PATH" )) {
        LOGRS( "Daemon certificate and key environment variables needs to be present as"
               "\n$EH_DEAMON_CERT_KEY_PATH\n$EH_DEAMON_CERT_CRT_PATH" );
        return 1;
    }
    Socket::createConnection();

//    initDeamon();

    Mongo mdb{ "event_horizon" };
    auto sourceAssetBucket = mdb.useBucket( "fs_assets_to_elaborate" );
    auto entityBucket = mdb.useBucket( "fs_entity_assets" );

    sourceAssetBucket.deleteAll();
    entityBucket.deleteAll();
    mdb["entities"]().delete_many( {} );
    mdb["remaps"]().delete_many( {} );

    auto streamToElaborate = mdb["fs_assets_to_elaborate.files"].watch();
    auto streamAsset = mdb["fs_entity_assets.files"].watch();

    while ( true ) {
        parseElaborateStream( streamToElaborate, sourceAssetBucket, entityBucket );
        parseAssetStream( mdb, streamAsset );
    }

    return 0;
}

#pragma clang diagnostic pop