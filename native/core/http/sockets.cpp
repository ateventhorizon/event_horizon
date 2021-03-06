//
// Created by Dado on 29/04/2018.
//

#include <iostream>
#include <unordered_map>
#include <thread>
#include <condition_variable>
#include <rapidjson/document.h>
#include "webclient.h"
#include "core/util.h"

namespace Socket {

    JSONDATA( SocketMessage, msg, data )

        SocketMessage( const std::string& msg, const std::string& data ) : msg( msg ), data( data ) {}

        std::string msg;
        std::string data;
    };

    void createConnection() {
        // First thing close if any existing connections are present
//        close();
        std::string host = Url::Host( Url::WssProtocol, Http::CLOUD_WSS_SERVER() );
        auto sessionString = std::string{Http::sessionId()};
        if ( sessionString.empty() ) sessionString = "daemon";
        host += "/?s=" + sessionString;
        startClient(host);
    }

    void on( const std::string& eventName, SocketCallbackFunc f ) {
        callbacksMap[eventName] = f;
    }

    void emitImpl( const std::string& _message );

    void emit( const std::string& _message ) {
//        LOGRS("[WSS][CLIENT] Send " << _message );
        emitImpl(_message);
    }

    void emit( const std::string& _message, const std::string& jsonString ) {
//        LOGRS("[WSS][CLIENT] Send msg: " << _message );
        SocketMessage sm{ _message, jsonString };

        emitImpl(sm.serialize());
    }

    void onMessage( const std::string& _message ) {

//        LOGR( "[WEB-SOCKET-MESSAGE] %s", _message.c_str());

        rapidjson::Document document;
        document.Parse( _message.c_str() );

        if (!document.IsObject()) {
            LOGR( "[WEB-SOCKET] [ERROR] Document not an object" );
            return;
        }
        if (!document.HasMember("msg")) {
            LOGR( "[WEB-SOCKET] [ERROR] Document msg entry missing" );
            return;
        }
        if (!document["msg"].IsString()) {
            LOGR( "[WEB-SOCKET] [ERROR] Document msg NOT a string" );
            return;
        }

        auto msg = document["msg"].GetString();

        if ( auto fi = callbacksMap.find(std::string(msg)); fi != callbacksMap.end() ) {
            fi->second( msg, std::move(document) );
        }
    }

}
