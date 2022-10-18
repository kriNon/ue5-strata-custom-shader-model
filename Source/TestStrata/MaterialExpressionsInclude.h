// Copyright Epic Games, Inc. All Rights Reserved.
//This file contains some functions from "MaterialExpressions.cpp"
#pragma once

#include "MaterialCompiler.h"
#include "Materials/MaterialExpressionStrata.h"
#define LOCTEXT_NAMESPACE "MaterialExpression"

#if WITH_EDITOR
int32 UMaterialExpressionStrataBSDF::CompilePreview(class FMaterialCompiler* Compiler, int32 OutputIndex)
{
	// Compile the StrataData output.
	int32 StrataDataCodeChunk = Compile(Compiler, OutputIndex);
	// Convert the StrataData to a preview color.
	int32 PreviewCodeChunk = Compiler->StrataCompilePreview(StrataDataCodeChunk);
	return PreviewCodeChunk;
}
#endif

static int32 CastToNonLWCType(class FMaterialCompiler* Compiler, int32 CodeChunk)
{
	EMaterialValueType Type = Compiler->GetType(CodeChunk);
	if (IsLWCType(Type))
	{
		Type = MakeNonLWCType(Type);
		CodeChunk = Compiler->ValidCast(CodeChunk, Type);
	}
	return CodeChunk;
}
static int32 CompileWithDefaultFloat1(class FMaterialCompiler* Compiler, FExpressionInput& Input, float X, bool* bDefaultIsUsed = nullptr)
{
	if (bDefaultIsUsed)
	{
		*bDefaultIsUsed = Input.GetTracedInput().Expression == nullptr;
	}
	int32 CodeChunk = Input.GetTracedInput().Expression ? Input.Compile(Compiler) : Compiler->Constant(X);
	if (bDefaultIsUsed)
	{
		*bDefaultIsUsed |= CodeChunk == INDEX_NONE;
	}
	else
	{
		CodeChunk = CastToNonLWCType(Compiler, CodeChunk);
	}
	return CodeChunk == INDEX_NONE ? Compiler->Constant(X) : CodeChunk;
}
static int32 CompileWithDefaultFloat3(class FMaterialCompiler* Compiler, FExpressionInput& Input, float X, float Y, float Z, bool* bDefaultIsUsed = nullptr)
{
	if (bDefaultIsUsed)
	{
		*bDefaultIsUsed = Input.GetTracedInput().Expression == nullptr;
	}
	int32 CodeChunk = Input.GetTracedInput().Expression ? Input.Compile(Compiler) : Compiler->Constant3(X, Y, Z);
	if (bDefaultIsUsed)
	{
		*bDefaultIsUsed |= CodeChunk == INDEX_NONE;
	}
	else
	{
		CodeChunk = CastToNonLWCType(Compiler, CodeChunk);
	}
	return CodeChunk == INDEX_NONE ? Compiler->Constant3(X, Y, Z) : CodeChunk;
}
static int32 CompileWithDefaultNormalWS(class FMaterialCompiler* Compiler, FExpressionInput& Input, bool* bDefaultIsUsed = nullptr)
{
	if (Input.GetTracedInput().Expression != nullptr)
	{
		int32 NormalCodeChunk = Input.Compile(Compiler);

		if (NormalCodeChunk == INDEX_NONE)
		{
			if (bDefaultIsUsed)
			{
				*bDefaultIsUsed = true;
			}
			return Compiler->VertexNormal();
		}
		return Compiler->TransformNormalFromRequestedBasisToWorld(NormalCodeChunk);
	}
	if (bDefaultIsUsed)
	{
		*bDefaultIsUsed = true;
	}
	return Compiler->VertexNormal();
}

