//
// Created by Dado on 2019-04-15.
//

#include "node_graph.hpp"

void NodeGraph::nodeAddConnect( const std::function<NodeGraphConnectFuncSig>& _slot ) {
    nodeAddSignal.connect( _slot );
}
