const UINT BubblePortraitCount = 24;
const UINT ActionButtonCount = 15;
const UINT BuildQueueCount = 5;

class ScreenConstants
{
public:
    ScreenConstants()
    {
        _ValidSize = false;
    }

    void Resize(const Vec2i &WindowDimensions);

    __forceinline const Vec2f& GetVec2fConstant(ScreenConstantVec2fType Constant) const
    {
        return _ResolutionVec2fConstants[Constant];
    }
    __forceinline const Rectangle2i& GetRectangle2iConstant(ScreenConstantRectangle2iType Constant) const
    {
        return _ResolutionRectangle2iConstants[Constant];
    }
    __forceinline bool ValidSize()
    {
        return _ValidSize;
    }
    __forceinline Vec2i MinimapCoordToScreenCoord(const Vec2f &MinimapCoord) const
    {
        const Vec2f MinimapTopLeft = _ResolutionVec2fConstants[ScreenConstantVec2fMinimapTopLeft];
        const Vec2f MinimapDimensions = _ResolutionVec2fConstants[ScreenConstantVec2fMinimapDimensions];
        Vec2f Result = MinimapTopLeft + Vec2f(MinimapDimensions.x * MinimapCoord.x, MinimapDimensions.y * MinimapCoord.y);
        return Result.RoundToVec2i();
    }
    __forceinline Vec2f ScreenCoordToMinimapCoord(const Vec2f &ScreenCoord) const
    {
        const Vec2f MinimapTopLeft = _ResolutionVec2fConstants[ScreenConstantVec2fMinimapTopLeft];
        const Vec2f MinimapDimensions = _ResolutionVec2fConstants[ScreenConstantVec2fMinimapDimensions];
        Vec2f MinimapCoord = ScreenCoord - MinimapTopLeft;
        MinimapCoord.x = Utility::Bound(MinimapCoord.x / MinimapDimensions.x, 0.0f, 1.0f);
        MinimapCoord.y = Utility::Bound(MinimapCoord.y / MinimapDimensions.y, 0.0f, 1.0f);
        return MinimapCoord;
    }
    __forceinline bool Clickable(const Vec2i &ScreenCoord) const
    {
        if(_ResolutionRectangle2iConstants[ScreenConstantRectangle2iClickableRegion].ContainsPt(ScreenCoord))
        {
            return (!_ResolutionRectangle2iConstants[ScreenConstantRectangle2iIdleWorkerRegion].ContainsPt(ScreenCoord));
			//&& !_ResolutionRectangle2iConstants[ScreenConstantRectangle2iMenuRegion].ContainsPt(ScreenCoord));
        }
        else
        {
            return false;
        }
    }
    Vec2i RandomClickablePoint(float BorderRegion) const;

private:
    void Load1024x768Constants();

    bool _ValidSize;
    Vec2f _ResolutionVec2fConstants[ScreenConstantVec2fCount];
    Rectangle2i _ResolutionRectangle2iConstants[ScreenConstantRectangle2iCount];
};
