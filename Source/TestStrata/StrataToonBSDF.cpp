#include "StrataToonBSDF.h"
#include "MaterialExpressionsInclude.h"


FStrataRegisteredSharedLocalBasis StrataCompilationInfoCreateSharedLocalBasis(FMaterialCompiler* Compiler, int32 NormalCodeChunk, int32 TangentCodeChunk)
{
	if (TangentCodeChunk == INDEX_NONE)
	{
		return Compiler->StrataCompilationInfoRegisterSharedLocalBasis(NormalCodeChunk);
	}
	return Compiler->StrataCompilationInfoRegisterSharedLocalBasis(NormalCodeChunk, TangentCodeChunk);
}


UMaterialExpressionStrataToonBSDF::UMaterialExpressionStrataToonBSDF(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	struct FConstructorStatics
	{
		FText NAME_Strata;
		FConstructorStatics() : NAME_Strata(LOCTEXT("Strata BSDFs", "Strata BSDFs")) { }
	};
	static FConstructorStatics ConstructorStatics;
#if WITH_EDITORONLY_DATA
	MenuCategories.Add(ConstructorStatics.NAME_Strata);
#endif
}

#if WITH_EDITOR
int32 UMaterialExpressionStrataToonBSDF::Compile(class FMaterialCompiler* Compiler, int32 OutputIndex)
{
	int32 NormalCodeChunk = CompileWithDefaultNormalWS(Compiler, Normal);
	const FStrataRegisteredSharedLocalBasis NewRegisteredSharedLocalBasis = StrataCompilationInfoCreateSharedLocalBasis(Compiler, NormalCodeChunk);

	FStrataOperator& StrataOperator = Compiler->StrataCompilationGetOperator(this);
	StrataOperator.BSDFRegisteredSharedLocalBasis = NewRegisteredSharedLocalBasis;

	if (StrataOperator.bUseParameterBlending)
	{
		return Compiler->Errorf(TEXT("Strata Toon BSDF node cannot be used with parameter blending."));
	}
	else if (StrataOperator.bRootOfParameterBlendingSubTree)
	{
		return Compiler->Errorf(TEXT("Strata Toon BSDF node cannot be the root of a parameter blending sub tree."));
	}

	int32 BaseColorIndex = CompileWithDefaultFloat3(Compiler, BaseColor, 0.0f, 0.0f, 0.0f);
	int32 MetallicIndex = CompileWithDefaultFloat1(Compiler, Metallic, 0.0f);
	int32 SpecularIndex = CompileWithDefaultFloat1(Compiler, Specular, 0.5f);
	int32 RoughnessIndex = CompileWithDefaultFloat1(Compiler, Roughness, 0.5f);
	int32 EmissiveColorIndex = CompileWithDefaultFloat3(Compiler, EmissiveColor, 0.0f, 0.0f, 0.0f);
	int32 TopMaterialOpacityIndex = CompileWithDefaultFloat1(Compiler, TopMaterialOpacity, 0.0f);
	int32 WaterAlbedoIndex = CompileWithDefaultFloat3(Compiler, WaterAlbedo, 0.0f, 0.0f, 0.0f);
	int32 WaterExtinctionIndex = CompileWithDefaultFloat3(Compiler, WaterExtinction, 0.0f, 0.0f, 0.0f);
	int32 WaterPhaseGIndex = CompileWithDefaultFloat1(Compiler, WaterPhaseG, 0.0f);
	int32 ColorScaleBehindWaterIndex = CompileWithDefaultFloat3(Compiler, ColorScaleBehindWater, 1.0f, 1.0f, 1.0f);
	
	int32 OutputCodeChunk = Expose_HLSLMaterialTranslator::callAddCodeChunkInner(Compiler, MCT_Strata, EDerivativeStatus::NotAware, false, TEXT("PromoteParameterBlendedBSDFToOperator(GetStrataSingleLayerWaterBSDF(%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s), Parameters.StrataTree, %u, %u, %u, %u) /* Normal:%s */"),
	*Expose_HLSLMaterialTranslator::callStrataGetCastParameterCode(Compiler, BaseColorIndex,				MCT_Float3),
	*Expose_HLSLMaterialTranslator::callStrataGetCastParameterCode(Compiler, MetallicIndex,				MCT_Float),
	*Expose_HLSLMaterialTranslator::callStrataGetCastParameterCode(Compiler,SpecularIndex,				MCT_Float),
	*Expose_HLSLMaterialTranslator::callStrataGetCastParameterCode(Compiler,RoughnessIndex,				MCT_Float),
	*Expose_HLSLMaterialTranslator::callStrataGetCastParameterCode(Compiler,EmissiveColorIndex,			MCT_Float3),
	*Expose_HLSLMaterialTranslator::callStrataGetCastParameterCode(Compiler,TopMaterialOpacityIndex,		MCT_Float),
	*Expose_HLSLMaterialTranslator::callStrataGetCastParameterCode(Compiler,WaterAlbedoIndex,			MCT_Float3),
	*Expose_HLSLMaterialTranslator::callStrataGetCastParameterCode(Compiler,WaterExtinctionIndex,		MCT_Float3),
	*Expose_HLSLMaterialTranslator::callStrataGetCastParameterCode(Compiler,WaterPhaseGIndex,			MCT_Float),
	*Expose_HLSLMaterialTranslator::callStrataGetCastParameterCode(Compiler,ColorScaleBehindWaterIndex,	MCT_Float3),
	*Compiler->GetStrataSharedLocalBasisIndexMacro(NewRegisteredSharedLocalBasis),
	StrataOperator.Index,
	StrataOperator.BSDFIndex,
	StrataOperator.LayerDepth,
	StrataOperator.bIsBottom ? 1 : 0,
	*Expose_HLSLMaterialTranslator::callGetParameterCode(Compiler, NormalCodeChunk)
	);

	return OutputCodeChunk;
}



void UMaterialExpressionStrataToonBSDF::GetCaption(TArray<FString>& OutCaptions) const
{
	OutCaptions.Add(TEXT("Strata Toon Water BSDF"));
}

uint32 UMaterialExpressionStrataToonBSDF::GetOutputType(int32 OutputIndex)
{
	return MCT_Strata;
}

uint32 UMaterialExpressionStrataToonBSDF::GetInputType(int32 InputIndex)
{
	switch (InputIndex)
	{
	case 0:
		return MCT_Float3; // BaseColor
		break;
	case 1:
		return MCT_Float1; // Metallic
		break;
	case 2:
		return MCT_Float1; // Specular
		break;
	case 3:
		return MCT_Float1; // Roughness
		break;
	case 4:
		return MCT_Float3; // Normal
		break;
	case 5:
		return MCT_Float3; // Emissive Color
		break;
	case 6:
		return MCT_Float1; // TopMaterialOpacity
		break;
	case 7:
		return MCT_Float3; // WaterAlbedo
		break;
	case 8:
		return MCT_Float3; // WaterExtinction
		break;
	case 9:
		return MCT_Float1; // WaterPhaseG
		break;
	case 10:
		return MCT_Float3; // ColorScaleBehindWater
		break;
	}
	
	check(false);
	return MCT_Float1;
}

bool UMaterialExpressionStrataToonBSDF::IsResultStrataMaterial(int32 OutputIndex)
{
	return true;
}

void UMaterialExpressionStrataToonBSDF::GatherStrataMaterialInfo(FStrataMaterialInfo& StrataMaterialInfo, int32 OutputIndex)
{
	// Track connected inputs
	if (BaseColor.IsConnected())		{ StrataMaterialInfo.AddPropertyConnected(MP_BaseColor); }
	if (Metallic.IsConnected())			{ StrataMaterialInfo.AddPropertyConnected(MP_Metallic); }
	if (Specular.IsConnected())			{ StrataMaterialInfo.AddPropertyConnected(MP_Specular); }
	if (Roughness.IsConnected())		{ StrataMaterialInfo.AddPropertyConnected(MP_Roughness); }
	if (EmissiveColor.IsConnected())	{ StrataMaterialInfo.AddPropertyConnected(MP_EmissiveColor); }
	if (Normal.IsConnected())			{ StrataMaterialInfo.AddPropertyConnected(MP_Normal); }
	
	StrataMaterialInfo.AddShadingModel(SSM_SingleLayerWater);
}

FStrataOperator* UMaterialExpressionStrataToonBSDF::StrataGenerateMaterialTopologyTree(class FMaterialCompiler* Compiler, class UMaterialExpression* Parent, int32 OutputIndex)
{
	FStrataOperator& StrataOperator = Compiler->StrataCompilationRegisterOperator(STRATA_OPERATOR_BSDF, this, Parent);
	StrataOperator.BSDFType = STRATA_BSDF_TYPE_SINGLELAYERWATER;
	return &StrataOperator;
}
#endif // WITH_EDITOR
