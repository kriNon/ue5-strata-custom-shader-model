#pragma once
#include "MaterialCompiler.h"
#include "Expose_HLSLMaterialTranslator.h"
#include "Materials/MaterialExpression.h"
#include "Materials/MaterialExpressionStrata.h"
#include "StrataToonBSDF.generated.h"

UCLASS(MinimalAPI, collapsecategories, hidecategories = Object, DisplayName = "Strata Toon BSDF")
class UMaterialExpressionStrataToonBSDF : public UMaterialExpressionStrataBSDF
{
	GENERATED_UCLASS_BODY()

	/**
	 * Surface base color. (type = float3, unit = unitless, defaults to black)
	 */
	UPROPERTY()
	FExpressionInput BaseColor;

	/**
	 * Whether the surface represents a dielectric (such as plastic) or a conductor (such as metal). (type = float, unit = unitless, defaults to 0 = dielectric)
	 */
	UPROPERTY()
	FExpressionInput Metallic;

	/**
	 * Specular amount (type = float, unit = unitless, defaults to 0.5)
	 */
	UPROPERTY()
	FExpressionInput Specular;

	/**
	 * Controls how rough the Material is. Roughness of 0 (smooth) is a mirror reflection and 1 (rough) is completely matte or diffuse (type = float, unit = unitless, defaults to 0.5)
	 */
	UPROPERTY()
	FExpressionInput Roughness;

	/**
	 * The normal of the surface (type = float3, unit = unitless, defaults to +Z vector)
	 */
	UPROPERTY()
	FExpressionInput Normal;

	/**
	 * Emissive color on top of the surface (type = float3, unit = luminance, defaults to 0.0)
	 */
	UPROPERTY()
	FExpressionInput EmissiveColor;

	/**
	 * Opacity of the material layered on top of the water (type = float3, unit = unitless, defaults to 0.0)
	 */
	UPROPERTY()
	FExpressionInput TopMaterialOpacity;

	/**
	* The single scattering Albedo defining the overall color of the Material (type = float3, unit = unitless, default = 0)
	 */
	UPROPERTY()
	FExpressionInput WaterAlbedo;

	/**
	 * The rate at which light is absorbed or out-scattered by the medium. Mean Free Path = 1 / Extinction. (type = float3, unit = 1/cm, default = 0)
	 */
	UPROPERTY()
	FExpressionInput WaterExtinction;

	/**
	 * Anisotropy of the volume with values lower than 0 representing back-scattering, equal 0 representing isotropic scattering and greater than 0 representing forward scattering. (type = float, unit = unitless, defaults to 0)
	 */
	UPROPERTY()
	FExpressionInput WaterPhaseG;

	/**
	 * A scale to apply on the scene color behind the water surface. It can be used to approximate caustics for instance. (type = float3, unit = unitless, defaults to 1)
	 */
	UPROPERTY()
	FExpressionInput ColorScaleBehindWater;

	//~ Begin UMaterialExpression Interface
#if WITH_EDITOR
	virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
	virtual uint32 GetOutputType(int32 OutputIndex) override;
	virtual uint32 GetInputType(int32 InputIndex) override;
	virtual bool IsResultStrataMaterial(int32 OutputIndex) override;
	virtual void GatherStrataMaterialInfo(FStrataMaterialInfo& StrataMaterialInfo, int32 OutputIndex) override;
	virtual FStrataOperator* StrataGenerateMaterialTopologyTree(class FMaterialCompiler* Compiler, class UMaterialExpression* Parent, int32 OutputIndex) override;
#endif
	//~ End UMaterialExpression Interface
};