#include "Main.h"

void ScreenConstants::Resize(const Vec2i &WindowDimensions)
{
    if(WindowDimensions.x == 1024 && WindowDimensions.y == 768)
    {
        Load1024x768Constants();
        _ValidSize = true;
    }
    else if(WindowDimensions.x == 1920 && WindowDimensions.y == 1080)
    {
        Load1024x768Constants();
        _ValidSize = true;
    }
    else
    {
        g_Context->Files.Assert << "Unexpected screen resolution: " << WindowDimensions.CommaSeparatedString() << endl;
        Load1024x768Constants();
        _ValidSize = false;
    }
}

Vec2i ScreenConstants::RandomClickablePoint(float BorderRegion) const
{
    bool Valid = false;
    Vec2i Result;
    while(!Valid)
    {
        Rectangle2i Rect = GetRectangle2iConstant(ScreenConstantRectangle2iClickableRegion);
        Vec2f Point(rnd(), rnd());
        Point.x = Math::LinearMap(0.0f, 1.0f, BorderRegion, 1.0f - BorderRegion, Point.x);
        Point.y = Math::LinearMap(0.0f, 1.0f, BorderRegion, 1.0f - BorderRegion, Point.y);
        Result = Rect.Min + Vec2i(Math::Round(Rect.Width() * Point.x), Math::Round(Rect.Height() * Point.y));
        Valid = Clickable(Result);
    }
    return Result;
}

void ScreenConstants::Load1024x768Constants()
{
    _ResolutionVec2fConstants[ScreenConstantVec2fBubblePortraitTopLeft] = Vec2f(300.0f, 630.0f);
    _ResolutionVec2fConstants[ScreenConstantVec2fBubblePortraitDimensions] = Vec2f(41.0f, 41.0f);
    _ResolutionVec2fConstants[ScreenConstantVec2fActionButtonTopLeft] = Vec2f(754.5f, 601.5f);
    _ResolutionVec2fConstants[ScreenConstantVec2fActionButtonDimensions] = Vec2f(49.0f, 49.0f);
    _ResolutionVec2fConstants[ScreenConstantVec2fMinimapTopLeft] = Vec2f(20.5f, 574.5f);
    _ResolutionVec2fConstants[ScreenConstantVec2fMinimapDimensions] = Vec2f(183.0f, 183.0f);
    _ResolutionVec2fConstants[ScreenConstantVec2fControlGroupLevelTopLeft] = Vec2f(280.0f, 642.0f);
    _ResolutionVec2fConstants[ScreenConstantVec2fControlGroupLevelDisplacement] = Vec2f(0.0f, 23.5f);
    _ResolutionVec2fConstants[ScreenConstantVec2fScreenCenter] = Vec2f(508.0f, 311.0f);

    _ResolutionRectangle2iConstants[ScreenConstantRectangle2iClickableRegion] = Rectangle2i(Vec2i(5, 5), Vec2i(1019, 535));
    _ResolutionRectangle2iConstants[ScreenConstantRectangle2iIdleWorkerRegion] = Rectangle2i(Vec2i(0, 466), Vec2i(43, 511));
    
	//
	// Menubar clicking should be be disabled
	//
	_ResolutionRectangle2iConstants[ScreenConstantRectangle2iMenuRegion] = Rectangle2i(Vec2i(0, 0), Vec2i(693, 22));
	_ResolutionRectangle2iConstants[ScreenConstantRectangle2iResourceRegion] = Rectangle2i(Vec2i(703, 0), Vec2i(1024, 22));
	
    _ResolutionRectangle2iConstants[ScreenConstantRectangle2iMinimapRegion] =
        Rectangle2i::ConstructFromMinDimensions(_ResolutionVec2fConstants[ScreenConstantVec2fMinimapTopLeft].RoundToVec2i(), _ResolutionVec2fConstants[ScreenConstantVec2fMinimapDimensions].RoundToVec2i());
    _ResolutionRectangle2iConstants[ScreenConstantRectangle2iUnitInfoRegion] = Rectangle2i(Vec2i(256, 623), Vec2i(637, 768));
    _ResolutionRectangle2iConstants[ScreenConstantRectangle2iBaseSearchRegion] = Rectangle2i(Vec2i(507, 257), Vec2i(527, 318));
    _ResolutionRectangle2iConstants[ScreenConstantRectangle2iWarpGateRegion] = Rectangle2i(Vec2i(974, 476), Vec2i(1020, 523));
}
