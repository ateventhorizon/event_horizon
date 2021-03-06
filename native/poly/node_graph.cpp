//
// Created by Dado on 2019-04-15.
//

#include "node_graph.hpp"

void NodeGraph::preloadCompleteConnect( const std::function<ConnectVoidFuncSig>& _slot ) {
    preloadCompleteSignal.connect( _slot );
}

void NodeGraph::preloadProgressConnect( const std::function<ConnectVoidFloatFuncSig>& _slot ) {
    preloadProgressSignal.connect( _slot );
}

void NodeGraph::propagateDirtyFlagConnect( const std::function<ConnectPairStringBoolFuncSig>& _slot ) {
    propagateDirtyFlagSignal.connect( _slot );
}

void NodeGraph::nodeAddConnect( const std::function<NodeGraphConnectFuncSig>& _slot ) {
    nodeAddSignal.connect( _slot );
}

void NodeGraph::nodeUpdateConnect( const std::function<NodeGraphConnectFuncSig>& _slot ) {
    nodeUpdateSignal.connect( _slot );
}

void NodeGraph::runLUAScript( const std::function<ConnectLUAScriptSig>& _slot ) {
    runLUAScriptSignal.connect( _slot );
}

void NodeGraph::nodeRemoveConnect( const std::function<NodeGraphConnectFuncSig>& _slot ) {
    nodeRemoveSignal.connect( _slot );
}

void NodeGraph::gmNodeRemoveConnect( const std::function<NodeGraphConnectFuncSig>& _slot ) {
    gmNodeRemoveSignal.connect( _slot );
}

void NodeGraph::replaceMaterialConnect( const std::function<NodeGraphConnectReplaceFuncSig>& _slot ) {
    replaceMaterialSignal.connect( _slot );
}

void NodeGraph::nodeFullScreenImageConnect( const std::function<NodeGraphConnectCResourceRefFuncSig>& _slot ) {
    nodeFullScreenImageSignal.connect( _slot );
}

void NodeGraph::nodeFullScreenMaterialConnect( const std::function<NodeGraphConnectCResourceRefFuncSig>& _slot ) {
    nodeFullScreenMaterialSignal.connect( _slot );
}

void NodeGraph::nodeSetSkyboxConnect( const std::function<NodeGraphConnectCResourceRefFuncSig>& _slot ) {
    nodeSetSkyboxSignal.connect( _slot );
}

void NodeGraph::nodeFullScreenFontSonnetConnect( const std::function<NodeGraphConnectCResourceRefFuncSig>& _slot ) {
    nodeFullScreenFontSonnetSignal.connect( _slot );
}

void NodeGraph::nodeFullScreenProfileConnect( const std::function<NodeGraphConnectCResourceRefFuncSig>& _slot ) {
    nodeFullScreenProfileSignal.connect( _slot );
}

void NodeGraph::nodeFullScreenUIContainerConnect( const std::function<NodeGraphConnectCResourceRefFuncSig>& _slot ) {
    nodeFullScreenUIContainerSignal.connect( _slot );
}

void NodeGraph::changeMaterialPropertyConnectString( const std::function<NodeGraphConnectChangeMaterialPropertySFuncSig>& _slot ) {
    changeMaterialPropertyStringSignal.connect(_slot);
}

void NodeGraph::changeMaterialPropertyConnectFloat( const std::function<NodeGraphConnectChangeMaterialPropertyFFuncSig>& _slot ) {
    changeMaterialPropertyFloatSignal.connect(_slot);
}

void NodeGraph::changeMaterialPropertyConnectV3f( const std::function<NodeGraphConnectChangeMaterialPropertyV3fFuncSig>& _slot ) {
    changeMaterialPropertyV3fSignal.connect(_slot);
}

void NodeGraph::changeMaterialPropertyConnectV4f( const std::function<NodeGraphConnectChangeMaterialPropertyV4fFuncSig>& _slot ) {
    changeMaterialPropertyV4fSignal.connect(_slot);
}

void NodeGraph::nodeTakeScreenShotOnImportConnect( const std::function<NodeGraphConnectSerializableContainerSig>& _slot ) {
    takeScreenShotOnImportSignal.connect(_slot);
}
