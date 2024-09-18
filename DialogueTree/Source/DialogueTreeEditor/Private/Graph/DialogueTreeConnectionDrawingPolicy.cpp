// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Graph/DialogueTreeConnectionDrawingPolicy.h"
//Plugin
#include "DialogueTreeStyle.h"

FDialogueTreeConnectionDrawingPolicy::FDialogueTreeConnectionDrawingPolicy(
	int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, 
	const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements)
	: FConnectionDrawingPolicy(
		InBackLayerID, 
		InFrontLayerID, 
		InZoomFactor, 
		InClippingRect, 
		InDrawElements
	)
{
	//Get the radius of a standard pin
	PinRadius = 
		FDialogueTreeStyle::GetVector(TEXT("PinSize")).Y / 2.f;

	//Flip direction of relevant spline settings for vertical flow 
	SplineSettings.FwdDeltaRangeX = 
		Settings->ForwardSplineVerticalDeltaRange;

	SplineSettings.BwdDeltaRangeX = 
		Settings->BackwardSplineVerticalDeltaRange;

	SplineSettings.FwdDeltaRangeY = 
		Settings->ForwardSplineHorizontalDeltaRange;

	SplineSettings.BwdDeltaRangeY = 
		Settings->BackwardSplineHorizontalDeltaRange;

	SplineSettings.FwdTanScaleX = 
		Settings->ForwardSplineTangentFromVerticalDelta.Y;

	SplineSettings.FwdTanScaleY = 
		Settings->ForwardSplineTangentFromHorizontalDelta.X;

	SplineSettings.BwdTanScaleX = 
		Settings->BackwardSplineTangentFromVerticalDelta.Y;

	SplineSettings.BwdTanScaleY = 
		Settings->BackwardSplineTangentFromHorizontalDelta.X;
}

void FDialogueTreeConnectionDrawingPolicy::DrawPreviewConnector(
	const FGeometry& PinGeometry, const FVector2D& StartPoint,
	const FVector2D& EndPoint, UEdGraphPin* Pin)
{
	FConnectionParams ConnectionParams;
	DetermineWiringStyle(Pin, nullptr, ConnectionParams);

	if (Pin->Direction == EGPD_Input)
	{
		DrawSplineWithArrow(
			StartPoint, 
			GetPinConnectionPoint(PinGeometry, EGPD_Input), 
			ConnectionParams
		);
	}
	else //Output pin
	{
		DrawSplineWithArrow(
			GetPinConnectionPoint(PinGeometry, EGPD_Output), 
			EndPoint, 
			ConnectionParams
		);
	}
}

void FDialogueTreeConnectionDrawingPolicy::DrawSplineWithArrow(
	const FVector2D& StartPoint, const FVector2D& EndPoint, 
	const FConnectionParams& Params)
{
	// Draw the spline
	DrawConnection(
		WireLayerID,
		StartPoint,
		EndPoint,
		Params
	);

	// Draw the arrow
	if (ArrowImage != nullptr)
	{
		FVector2D ArrowPoint = EndPoint - ArrowRadius;

		/**
		* Note: this is marked as deprecated, but Epic is still 
		* using it in their connection drawing policies 
		* and there doesn't appear to be a good alternative that 
		* I can find. Leaving it in for now. 
		*/
		FPaintGeometry ArrowGeometry =
			FPaintGeometry(
				ArrowPoint,
				ArrowImage->ImageSize * ZoomFactor,
				ZoomFactor
			);

		FSlateDrawElement::MakeRotatedBox(
			DrawElementsList,
			ArrowLayerID,
			ArrowGeometry,
			ArrowImage,
			ESlateDrawEffect::None,
			ARROW_ANGLE,
			TOptional<FVector2D>(),
			FSlateDrawElement::RelativeToElement,
			Params.WireColor
		);
	}
}

void FDialogueTreeConnectionDrawingPolicy::DrawSplineWithArrow(
	const FGeometry& StartGeom, const FGeometry& EndGeom, 
	const FConnectionParams& Params)
{
	const FVector2D StartPoint = GetPinConnectionPoint(
		StartGeom, 
		EGPD_Output
	);
	const FVector2D EndPoint = GetPinConnectionPoint(
		EndGeom, 
		EGPD_Input
	);

	DrawSplineWithArrow(
		StartPoint,
		EndPoint,
		Params
	);
}

FVector2D FDialogueTreeConnectionDrawingPolicy::ComputeSplineTangent(
	const FVector2D& Start, const FVector2D& End) const
{
	//Calculate delta 
	const FVector2D DeltaPos = End - Start;

	//Determine directionality
	const FSplineShape Shape = GetSplineShape(DeltaPos);

	//Calculate base clamping 
	const float ClampedTensionX = FMath::Min<float>(
		FMath::Abs<float>(DeltaPos.X),
		Shape.DeltaX
	);
	const float ClampedTensionY = FMath::Min<float>(
		FMath::Abs<float>(DeltaPos.Y), 
		Shape.DeltaY
	);

	//Return final tangent 
	return FVector2D(
		ClampedTensionX * Shape.ScaleX, 
		ClampedTensionY * Shape.ScaleY
	);
}

FVector2D FDialogueTreeConnectionDrawingPolicy::GetPinConnectionPoint(
	const FGeometry& InPinGeom, const EEdGraphPinDirection InDirection) const
{
	FVector2D PinCenter = FGeometryHelper::CenterOf(InPinGeom);

	if (InDirection == EGPD_Input)
	{
		FVector2D InputPinOffset = FVector2D(
			0.f, 
			(PinRadius + ArrowRadius.X)
		);

		return PinCenter - InputPinOffset;
	}
	else //Output pin 
	{
		FVector2D OutputPinOffset = FVector2D(0.f, PinRadius);

		return PinCenter + OutputPinOffset;
	}
}

FDialogueTreeConnectionDrawingPolicy::FSplineShape
	FDialogueTreeConnectionDrawingPolicy::GetSplineShape(
		FVector2D DeltaPos) const
{
	//Determine directionality
	const bool bGoingForward = DeltaPos.Y <= 0.0f;
	FSplineShape Shape = FSplineShape();

	if (bGoingForward)
	{
		Shape.DeltaX = SplineSettings.FwdDeltaRangeX;
		Shape.DeltaY = SplineSettings.FwdDeltaRangeY;
		Shape.ScaleX = SplineSettings.FwdTanScaleX;
		Shape.ScaleY = SplineSettings.FwdTanScaleY;
	}
	else //Going backward
	{
		Shape.DeltaX = SplineSettings.BwdDeltaRangeX;
		Shape.DeltaY = SplineSettings.BwdDeltaRangeY;
		Shape.ScaleX = SplineSettings.BwdTanScaleX;
		Shape.ScaleY = SplineSettings.BwdTanScaleY;
	}

	return Shape;
}