// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

// UE
#include "ConnectionDrawingPolicy.h"

/**
 * Class responsible for drawing connections between dialouge nodes
 * in the graph.
 */
class FDialogueTreeConnectionDrawingPolicy : public FConnectionDrawingPolicy
{
public:
    /** Constructor */
    FDialogueTreeConnectionDrawingPolicy(int32 InBackLayerID,
                                         int32 InFrontLayerID, float InZoomFactor,
                                         const FSlateRect &InClippingRect,
                                         FSlateWindowElementList &InDrawElements);

public:
    /** FConnectionDrawingPolicy Implementation */
    virtual void DrawPreviewConnector(const FGeometry &PinGeometry,
                                      const FVector2f &StartPoint, const FVector2f &EndPoint,
                                      UEdGraphPin *Pin) override;
    virtual void DrawSplineWithArrow(const FVector2f &StartPoint,
                                     const FVector2f &EndPoint, const FConnectionParams &Params) override;
    virtual void DrawSplineWithArrow(const FGeometry &StartGeom,
                                     const FGeometry &EndGeom, const FConnectionParams &Params) override;
    virtual FVector2f ComputeSplineTangent(const FVector2f &Start,
                                           const FVector2f &End) const override;
    /** End FConnectionDrawingPolicy */

private:
    /** Helper struct used to define the shape of a spline */
    struct FSplineShape
    {
        float DeltaX;
        float DeltaY;
        float ScaleX;
        float ScaleY;
    };

    /**
     * Determines the location to draw the pin's connection to.
     *
     * @param InPinGeom - const FGeometry&, the pin's geometry.
     * @param InDirection - const EEdGraphPinDirection, whether an
     * input or output pin.
     * @return FVector2f - Location of the connection point.
     */
    FVector2f GetPinConnectionPoint(const FGeometry &InPinGeom,
                                    const EEdGraphPinDirection InDirection) const;

    /**
     * Retrieves the desired spline shape for the connection.
     *
     * @param DeltaPos - FVector2D, the offset between the pins to
     * be connected.
     * @return FSplineShape - the shape struct for the spline.
     */
    FSplineShape GetSplineShape(FVector2f DeltaPos) const;

private:
    /** Radius of a standard pin */
    float PinRadius;

    /**
     * Helper struct storing spline settings for drawing connections
     * in a vertical rather than horizontal direction.
     */
    struct FVerticalSplineSettings
    {
        float FwdDeltaRangeX;
        float BwdDeltaRangeX;
        float FwdDeltaRangeY;
        float BwdDeltaRangeY;
        double FwdTanScaleX;
        double FwdTanScaleY;
        double BwdTanScaleX;
        double BwdTanScaleY;
    };

    /** Cached settings for the vertically flowing spines */
    FVerticalSplineSettings SplineSettings;

    /** Constants */
    const float ARROW_ANGLE = 1.5708f; // 90 degrees in radians
};