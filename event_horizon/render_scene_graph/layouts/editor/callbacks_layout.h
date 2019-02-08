//
// Created by Dado on 2018-11-05.
//

#pragma once

#include <string>
#include <vector>
class SceneOrchestrator;

using entityDaemonCallbackFunction = void( const std::string&, const std::vector<char>& );

void allCallbacksEntitySetup();
void allConversionsDragAndDropCallback( SceneOrchestrator* p, const std::string& _path );

