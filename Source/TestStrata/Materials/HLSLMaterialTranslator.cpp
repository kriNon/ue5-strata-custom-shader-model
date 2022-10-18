// Copyright Epic Games, Inc. All Rights Reserved.
/*=============================================================================
	HLSLMaterialTranslator.cpp: Translates material expressions into HLSL code.
=============================================================================*/

#include "TestStrata/Materials/HLSLMaterialTranslator.h"

/** Used to get a user friendly type from EMaterialValueType */
const TCHAR* FHLSLMaterialTranslator::DescribeType(EMaterialValueType Type) const
{
	switch(Type)
	{
	case MCT_Float1:				return TEXT("float");
	case MCT_Float2:				return TEXT("float2");
	case MCT_Float3:				return TEXT("float3");
	case MCT_Float4:				return TEXT("float4");
	case MCT_Float:					return TEXT("float");
	case MCT_Texture2D:				return TEXT("texture2D");
	case MCT_TextureCube:			return TEXT("textureCube");
	case MCT_Texture2DArray:		return TEXT("texture2DArray");
	case MCT_TextureCubeArray:		return TEXT("textureCubeArray");
	case MCT_VolumeTexture:			return TEXT("volumeTexture");
	case MCT_StaticBool:			return TEXT("static bool");
	case MCT_Bool:					return TEXT("bool");
	case MCT_MaterialAttributes:	return TEXT("MaterialAttributes");
	case MCT_TextureExternal:		return TEXT("TextureExternal");
	case MCT_TextureVirtual:		return TEXT("TextureVirtual");
	case MCT_VTPageTableResult:		return TEXT("VTPageTableResult");
	case MCT_ShadingModel:			return TEXT("ShadingModel");
	case MCT_Strata:				return TEXT("Strata");
	case MCT_LWCScalar:				return TEXT("LWCScalar");
	case MCT_LWCVector2:			return TEXT("LWCVector2");
	case MCT_LWCVector3:			return TEXT("LWCVector3");
	case MCT_LWCVector4:			return TEXT("LWCVector4");
	default:						return TEXT("unknown");
	};
}

/** Used to get an HLSL type from EMaterialValueType */
const TCHAR* FHLSLMaterialTranslator::HLSLTypeString(EMaterialValueType Type) const
{
	switch(Type)
	{
	case MCT_Float1:				return TEXT("MaterialFloat");
	case MCT_Float2:				return TEXT("MaterialFloat2");
	case MCT_Float3:				return TEXT("MaterialFloat3");
	case MCT_Float4:				return TEXT("MaterialFloat4");
	case MCT_Float:					return TEXT("MaterialFloat");
	case MCT_Texture2D:				return TEXT("texture2D");
	case MCT_TextureCube:			return TEXT("textureCube");
	case MCT_Texture2DArray:		return TEXT("texture2DArray");
	case MCT_TextureCubeArray:		return TEXT("textureCubeArray");
	case MCT_VolumeTexture:			return TEXT("volumeTexture");
	case MCT_StaticBool:			return TEXT("static bool");
	case MCT_Bool:					return TEXT("bool");
	case MCT_MaterialAttributes:	return TEXT("FMaterialAttributes");
	case MCT_TextureExternal:		return TEXT("TextureExternal");
	case MCT_TextureVirtual:		return TEXT("TextureVirtual");
	case MCT_VTPageTableResult:		return TEXT("VTPageTableResult");
	case MCT_ShadingModel:			return TEXT("uint");
	case MCT_Strata:				return TEXT("FStrataData");
	case MCT_LWCScalar:				return TEXT("FLWCScalar");
	case MCT_LWCVector2:			return TEXT("FLWCVector2");
	case MCT_LWCVector3:			return TEXT("FLWCVector3");
	case MCT_LWCVector4:			return TEXT("FLWCVector4");
	default:						return TEXT("unknown");
	};
}

/** Creates a unique symbol name and adds it to the symbol list. */
FString FHLSLMaterialTranslator::CreateSymbolName(const TCHAR* SymbolNameHint)
{
	NextSymbolIndex++;
	return FString(SymbolNameHint) + FString::FromInt(NextSymbolIndex);
}

// /** Adds an already formatted inline or referenced code chunk */
int32 FHLSLMaterialTranslator::AddCodeChunkInner(uint64 Hash, const TCHAR* FormattedCode, EMaterialValueType Type, EDerivativeStatus DerivativeStatus, bool bInlined)
{
	check(bAllowCodeChunkGeneration);

	if (Type == MCT_Unknown)
	{
		return INDEX_NONE;
	}

	int32 CodeIndex = INDEX_NONE;
	if (Type == MCT_VoidStatement)
	{
		CodeIndex = CurrentScopeChunks->Num();
		const FString Statement = FString("") + FormattedCode + LINE_TERMINATOR;
		new(*CurrentScopeChunks) FShaderCodeChunk(Hash, *Statement, *Statement, TEXT(""), Type, DerivativeStatus, true);
	}
	else if (bInlined)
	{
		CodeIndex = CurrentScopeChunks->Num();
		// Adding an inline code chunk, the definition will be the code to inline
		new(*CurrentScopeChunks) FShaderCodeChunk(Hash, FormattedCode, FormattedCode, TEXT(""), Type, DerivativeStatus, true);
	}
	// Can only create temporaries for certain types
	else if ((Type & (MCT_Float | MCT_LWCType | MCT_VTPageTableResult)) || Type == MCT_ShadingModel || Type == MCT_MaterialAttributes || Type == MCT_Strata)
	{
		// Check for existing
		for (int32 i = 0; i < CurrentScopeChunks->Num(); ++i)
		{
			if ((*CurrentScopeChunks)[i].Hash == Hash)
			{
				CodeIndex = i;
				break;
			}
		}

		if (CodeIndex == INDEX_NONE)
		{
			CodeIndex = CurrentScopeChunks->Num();
		// Allocate a local variable name
		const FString SymbolName = CreateSymbolName(TEXT("Local"));
		// Construct the definition string which stores the result in a temporary and adds a newline for readability
			const FString LocalVariableDefinitionFinite = FString("	") + HLSLTypeString(Type) + TEXT(" ") + SymbolName + TEXT(" = ") + FormattedCode + TEXT(";") + LINE_TERMINATOR;
			// Construct the definition string which stores the result in a temporary and adds a newline for readability
			const FString LocalVariableDefinitionAnalytic = FString("	") + HLSLTypeString(Type) + TEXT(" ") + SymbolName + TEXT(" = ") + FormattedCode + TEXT(";") + LINE_TERMINATOR;
		// Adding a code chunk that creates a local variable
			new(*CurrentScopeChunks) FShaderCodeChunk(Hash, *LocalVariableDefinitionFinite, *LocalVariableDefinitionAnalytic, SymbolName, Type, DerivativeStatus, false);
		}
	}
	else
	{
		if (Type & MCT_Texture)
		{
			return Errorf(TEXT("Operation not supported on a Texture"));
		}
		else if (Type == MCT_StaticBool)
		{
			return Errorf(TEXT("Operation not supported on a Static Bool"));
		}
		else
		{
			return Errorf(TEXT("Operation not supported for type %s"), DescribeType(Type));
		}
	}
	
	AddCodeChunkToCurrentScope(CodeIndex);
	return CodeIndex;
}

int32 FHLSLMaterialTranslator::AddCodeChunkInner(EMaterialValueType Type, EDerivativeStatus DerivativeStatus, bool bInlined, const TCHAR* Format, ...)
{
	int32	BufferSize		= 256;
	TCHAR*	FormattedCode	= NULL;
	int32	Result			= -1;

	while(Result == -1)
	{
		FormattedCode = (TCHAR*) FMemory::Realloc( FormattedCode, BufferSize * sizeof(TCHAR) );
		GET_VARARGS_RESULT( FormattedCode, BufferSize, BufferSize-1, Format, Format, Result );
		BufferSize *= 2;
	};
	FormattedCode[Result] = 0;

	const uint64 Hash = CityHash64((char*)FormattedCode, Result * sizeof(TCHAR));
	const int32 CodeIndex = AddCodeChunkInner(Hash, FormattedCode, Type, DerivativeStatus, bInlined);
	FMemory::Free(FormattedCode);

	return CodeIndex;
}

void FHLSLMaterialTranslator::AddCodeChunkToCurrentScope(int32 ChunkIndex)
{
	if (ChunkIndex != INDEX_NONE && ScopeStack.Num() > 0)
	{
		const int32 CurrentScopeIndex = ScopeStack.Last();
		AddCodeChunkToScope(ChunkIndex, CurrentScopeIndex);
	}
}

void FHLSLMaterialTranslator::AddCodeChunkToScope(int32 ChunkIndex, int32 ScopeIndex)
{
	if (ChunkIndex != INDEX_NONE)
	{
		FShaderCodeChunk& CurrentScope = (*CurrentScopeChunks)[ScopeIndex];

		FShaderCodeChunk& Chunk = (*CurrentScopeChunks)[ChunkIndex];
		if (Chunk.DeclaredScopeIndex == INDEX_NONE)
		{
			check(Chunk.UsedScopeIndex == INDEX_NONE);
			Chunk.DeclaredScopeIndex = ScopeIndex;
			Chunk.UsedScopeIndex = ScopeIndex;
			Chunk.ScopeLevel = CurrentScope.ScopeLevel + 1;
		}
		else if (Chunk.UsedScopeIndex != ScopeIndex)
		{
			// Find the most derived scope that's shared by the current scope, and the scope this code was previously referenced from
			int32 ScopeIndex0 = ScopeIndex;
			int32 ScopeIndex1 = Chunk.UsedScopeIndex;
			while (ScopeIndex0 != ScopeIndex1)
			{
				const FShaderCodeChunk& Scope0 = (*CurrentScopeChunks)[ScopeIndex0];
				const FShaderCodeChunk& Scope1 = (*CurrentScopeChunks)[ScopeIndex1];
				if (Scope0.ScopeLevel > Scope1.ScopeLevel)
				{
					check(Scope0.UsedScopeIndex != INDEX_NONE);
					ScopeIndex0 = Scope0.UsedScopeIndex;
				}
				else
				{
					check(Scope1.UsedScopeIndex != INDEX_NONE);
					ScopeIndex1 = Scope1.UsedScopeIndex;
				}
			}

			const FShaderCodeChunk& Scope = (*CurrentScopeChunks)[ScopeIndex0];
			Chunk.UsedScopeIndex = ScopeIndex0;
			Chunk.ScopeLevel = Scope.ScopeLevel + 1;
		}
	}
}

FString FHLSLMaterialTranslator::StrataGetCastParameterCode(int32 Index, EMaterialValueType DestType)
{
	int32 CastParameter = ForceCast(Index, DestType);
	return GetParameterCode(CastParameter);
}