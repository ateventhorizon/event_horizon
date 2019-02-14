//
// Created by Dado on 2018-12-28.
//

#pragma once

#include <memory>
#include <variant>

template <typename> class Node;
class GeomData;

using GeomAsset = Node<GeomData>;
using GeomAssetSP = std::shared_ptr<GeomAsset>;

class UIElement;

using UIAsset = Node<UIElement>;
using UIAssetSP = std::shared_ptr<UIAsset>;

using NodeVariants = std::variant<GeomAssetSP, UIAssetSP>;

enum class UIShapeType {
    CameraFrustom2d,
    CameraFrustom3d,
    Line2d,
    Line3d,
    Arrow2d,
    Arrow3d,
    Polygon2d,
    Polygon3d,
    Rect2d,
    Rect3d,
    Text2d,
    Text3d,
    Separator2d,
    Separator3d,
};

enum class RectCreateAnchor {
    None,
    Top,
    Bottom,
    Center,
    Left,
    Right,
    LeftCenter,
    RightCenter
};

enum class RectFillMode {
    Scale,
    AspectFit,
    AspectFill,
    AspectFitLeft,
    AspectFitRight,
    AspectFitTop,
    AspectFitBottom,
};

enum UIRenderFlags {
    NoEffects = 1 << 0,
    DropShaodws = 1 << 1,
    RoundedCorners = 1 << 2
};
