//
// Created by Dado on 2018-10-29.
//

#include "login_layout.h"
#include <graphics/imgui/imgui.h>
#include <render_scene_graph/scene_orchestrator.hpp>
#include <render_scene_graph/layouts/layout_helper.hpp>

void ImGuiLogin::renderImpl( SceneOrchestrator* p, Rect2f& _r ) {
    static char projectInputChar[256];
    static char emailInputChar[256];
    static char passwordInputChar[256];
    static std::string projectBuf;
    static std::string emailBuf;
    static std::string passwordBuf;

    projectBuf.reserve(256);
    emailBuf.reserve(256);
    passwordBuf.reserve(256);

    if ( Http::cachedLoginFields().project.size() < 256 && projectBuf.empty() ) {
        projectBuf = Http::cachedLoginFields().project;
        strcpy( projectInputChar, projectBuf.data() );
    }

    if ( emailBuf.empty() ) {
        emailBuf = Http::cachedLoginFields().email;
        strcpy( emailInputChar, emailBuf.data() );
    }

    if ( passwordBuf.empty() ) {
        passwordBuf = Http::cachedLoginFields().password;
        strcpy( passwordInputChar, passwordBuf.data() );
    }

    static LoginFields lf;
    ImGui::PushID( static_cast<int>(std::hash<std::string>{}("Email")));
    ImGui::InputText( "email", emailInputChar, 256 );
    ImGui::PopID();
    ImGui::PushID( static_cast<int>(std::hash<std::string>{}("Password")));
    ImGui::InputText( "password", passwordInputChar, 256, ImGuiInputTextFlags_Password );
    ImGui::PopID();
    ImGui::PushID( static_cast<int>(std::hash<std::string>{}("Project")));
    ImGui::InputText( "Project", projectInputChar, 256 );
    ImGui::PopID();

    lf.email = emailInputChar;
    lf.password = passwordInputChar;
    lf.project = projectInputChar;

    if ( ImGui::Button( "Login" ) ) {
        Http::login( lf );
    }
}