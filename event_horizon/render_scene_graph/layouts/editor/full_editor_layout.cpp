//
// Created by Dado on 2018-10-16.
//

#include "full_editor_layout.h"

#include <render_scene_graph/scene.hpp>
#include <render_scene_graph/scene_layout.h>
#include <poly/ui_shape_builder.h>
#include <poly/geom_builder.h>
                            #include <media/audio_video_stream.hpp>
#include "callbacks_layout.h"

void initLayout( SceneLayout* _layout, [[maybe_unused]] SceneOrchestrator* p ) {

    struct UIViewLayout {
        float consoleHeight = 0.0f;
        float rightPanelWidth = 0.0f;
        float leftPanelHeight = 0.0f;
        float leftPanelHeight2 = 0.0f;
        float rightPanelHeight = 0.0f;
        float loginPanelHeight = 0.0f;
        float taskbarHeight = 0.05f;
        Vector2f main3dWindowSize = Vector2f::ZERO;
        Vector2f timeLinePanelSize = Vector2f::ZERO;
        Rect2f foxLayout = Rect2f::ZERO;
    };

    UIViewLayout uivl;

    uivl.consoleHeight = 0.15f;
    uivl.rightPanelWidth = 0.25f;
    uivl.loginPanelHeight = 0.18f;
    uivl.rightPanelHeight = ((1.0f - uivl.consoleHeight)/2.0f) - uivl.loginPanelHeight*0.5f;
    uivl.leftPanelHeight = (1.0f - uivl.consoleHeight)/3.0f;
    uivl.leftPanelHeight2 = (1.0f - uivl.consoleHeight)/5.f;
    uivl.timeLinePanelSize = { 1.0f - (uivl.rightPanelWidth*2), 0.35f };
    float topX = uivl.rightPanelWidth;
    float cameraWidth = (1.0f-uivl.rightPanelWidth*2.0f);
    float cameraAspectRatio = (720.0f / 1280.0f);
    float cameraHeight = cameraWidth*(cameraAspectRatio*(1280.0f/720.0f));
    float timeLineY = 1.0f-(uivl.consoleHeight+uivl.timeLinePanelSize.y());

#define CENTER(xc,yc) 0.5f-xc*0.5f, 0.5f+xc*0.5f, 0.5f-yc*0.5f, 0.5f+yc*0.5f

    _layout->addBox( SceneLayoutDefaultNames::Taskbar, 0.0f, 1.0f, 0.0f, uivl.taskbarHeight );

    _layout->addBox( SceneLayoutDefaultNames::Login, CENTER(uivl.rightPanelWidth, uivl.loginPanelHeight), false);

    _layout->addBox( SceneLayoutDefaultNames::Console, 0.0f, 1.0f, 1.0f-uivl.consoleHeight, 1.0f );
    _layout->addBox( SceneLayoutDefaultNames::Geom, 0.0f, uivl.rightPanelWidth, uivl.taskbarHeight, timeLineY + uivl.taskbarHeight );
    _layout->addBox( SceneLayoutDefaultNames::Material, 0.0f, uivl.rightPanelWidth, uivl.leftPanelHeight, uivl.leftPanelHeight*2.0f - uivl.taskbarHeight, false );

    _layout->addBox( SceneLayoutDefaultNames::Image, CENTER(uivl.rightPanelWidth, uivl.leftPanelHeight2*5.0f), false );
    _layout->addBox( SceneLayoutDefaultNames::Camera, 1.0f-uivl.rightPanelWidth, 1.0f,
                     timeLineY - uivl.leftPanelHeight2 + uivl.taskbarHeight,
                     timeLineY+ uivl.taskbarHeight);

    _layout->addBox( SceneLayoutDefaultNames::Timeline,
                     0.0f, 1.0f,
                     timeLineY +uivl.taskbarHeight, timeLineY + uivl.timeLinePanelSize.y() );

    _layout->addBox( SceneLayoutDefaultNames::CloudMaterial,
                     1.0f-uivl.rightPanelWidth, 1.0f, uivl.loginPanelHeight, uivl.loginPanelHeight + uivl.rightPanelHeight, false );

    _layout->addBox( SceneLayoutDefaultNames::CloudGeom,
                     1.0f-uivl.rightPanelWidth, 1.0f, uivl.loginPanelHeight + uivl.rightPanelHeight, uivl.loginPanelHeight + uivl.rightPanelHeight*2, false );

    _layout->addBox( Name::Foxtrot,
                     topX, topX + cameraWidth,
                     uivl.taskbarHeight, cameraHeight + uivl.taskbarHeight, CameraControls::Fly );

    allCallbacksEntitySetup();
}

void render( SceneOrchestrator* p ) {
//    static bool bS = true;
//    if ( bS ) {
//        auto streamName = "http://192.168.1.123:8080/video";
//        p->addHttpStream<AudioVideoStreamFFmpeg>(streamName);
//        GB{ShapeType::Cube}.m(S::YUV_GREENSCREEN,streamName).build(p->RSG());
//        bS = false;
//    }
}

std::shared_ptr<SceneLayout> fullEditor() {
    return std::make_shared<SceneLayout>(initLayout, render, allConversionsDragAndDropCallback);
}
