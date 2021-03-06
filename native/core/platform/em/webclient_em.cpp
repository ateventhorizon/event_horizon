#include <unordered_map>
#include "../../http/webclient.h"
#include "../../util.h"

#include <iomanip>
#include <emscripten.h>
#include <emscripten/fetch.h>
#include <emscripten/val.h>

namespace Http {

    using callbackRespondeMap = std::unordered_map<std::string, ResponseCallbackFunc>;
    using callbackRespondeCCF = std::unordered_map<std::string, HttpResourceCB>;
    callbackRespondeMap argCallbackMapOk;
    callbackRespondeMap argCallbackMapFail;
    callbackRespondeCCF argCallbackMapCCF;

    void responseCallback( callbackRespondeMap& argCallbackMap,
                           const char* ckey, int code, const char* etag, const char* contentType, void* data, unsigned numBytes ) {
        auto skey = std::string( ckey );
        if ( argCallbackMap[skey] ) {
            argCallbackMap[skey]( { skey, std::string(contentType), reinterpret_cast<const char*>(data), numBytes, code,
                                    std::string(etag), argCallbackMapCCF[skey] } );
        }
        delete [] ckey;
    }

    void onSuccessWget( unsigned boh, void* arg, int code, const char* etag, const char* contentType, void* data, unsigned numBytes ) {
        LOGR( "[HTTP-RESPONSE] code: %d, etag: %s, contentType: %s, handle %d, numBytes: %d", code, etag, contentType, boh, numBytes );
        responseCallback( argCallbackMapOk, reinterpret_cast<char*>(arg), code, etag, contentType, data, numBytes );
    }

    void onFailWget( [[maybe_unused]] unsigned boh, void *arg, int code, const char* why ) {
        LOGR("[HTTP-RESPONSE][ERROR] handle: %d code: %d URI: %s -- %s", boh, code, reinterpret_cast<char*>(arg), why );
        responseCallback( argCallbackMapFail,  reinterpret_cast<char*>(arg), code, nullptr, nullptr, nullptr, 0 );
    }

    void onProgressWget( [[maybe_unused]] unsigned boh, [[maybe_unused]] void* arg,
                         [[maybe_unused]] int bytesRead, [[maybe_unused]] int totalBytes ) {

    }

    // We inject space separated header key/value, please notice the space
    std::string makeHeaders() {
        std::stringstream ss;
        ss << "eh_aft " << Http::AFT();
        return ss.str();
    }

    char* urlKeyPassing( const Url& uri, const ResponseCallbackFunc callbackOk,
                                         const ResponseCallbackFunc callbackFail,
                                         HttpResourceCB ccf ) {
        auto key = uri.toString();
        argCallbackMapOk[key] = callbackOk;
        argCallbackMapFail[key] = callbackFail;
        argCallbackMapCCF[key] = ccf;
        char* keyToCharCPassing = new char[key.size()+1];
        strcpy( keyToCharCPassing, key.c_str() );
        keyToCharCPassing[key.size()] = '\0';
        return keyToCharCPassing;
    }

    void getInternal( const Url& uri,
                      ResponseCallbackFunc callback,
                      ResponseCallbackFunc callbackFailed,
                      [[maybe_unused]] ResponseFlags rf,
                      HttpResourceCB ccf ) {

        // NDDado:
        // http req specification doesn't allow body on GET requests, so we have to ignore them

        emscripten_async_http_request(uri.toString().c_str(),
                                    "GET",
                                    makeHeaders().c_str(),
                                      nullptr,
                                    0,
                                    nullptr,
                                    reinterpret_cast<void*>(urlKeyPassing(uri, callback, callbackFailed, ccf)),
                                    false,
                                    onSuccessWget,
                                    onFailWget,
                                    onProgressWget,
                                    true );
    }

    void postInternal( const Url& uri, const std::string& method, const char *buff, uint64_t length, HttpQuery qt,
                       ResponseCallbackFunc callback, ResponseCallbackFunc callbackFailed,
                       HttpResourceCB ccf ) {

        LOGRS( "[HTTP-" << method << "] " << uri.toString() );
        LOGRS( "[HTTP-" << method << "-DATA-LENGTH] " << length );

        std::string contenType = qt==HttpQuery::Binary ? "application/octet-stream" : "application/json; charset=utf-8";
        emscripten_async_http_request( uri.toString().c_str(),
                                       method.c_str(),
                                       makeHeaders().c_str(),
                                       buff,
                                       static_cast<int>(length),
                                       contenType.c_str(),
                                       reinterpret_cast<void*>(urlKeyPassing(uri, callback, callbackFailed, ccf)),
                                       false,
                                       onSuccessWget,
                                       onFailWget,
                                       onProgressWget,
                                       true );
    }

    bool Result::isSuccessStatusCode() const {
        return ::isSuccessStatusCode( statusCode );
    }

}