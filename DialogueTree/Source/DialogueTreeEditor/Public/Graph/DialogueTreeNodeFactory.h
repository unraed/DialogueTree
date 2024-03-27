// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "EdGraph/EdGraphNode.h"
#include "EdGraphUtilities.h"

/**
* Factory for creating the visual representation of a node 
* (SGraphNode).  
*/
class FDialogueTreeNodeFactory : public FGraphPanelNodeFactory
{
public:
	/** FGraphPanelNodeFactory Impl. */
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node)
		const override;
	/** End FGraphPanelNodeFactory */
};