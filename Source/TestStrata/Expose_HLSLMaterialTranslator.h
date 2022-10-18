#pragma once
#include "MaterialCompiler.h"

#include "Engine/Private/Materials/HLSLMaterialDerivativeAutogen.h"
#include "Engine/Private/Materials/HLSLMaterialTranslator.h"


class Expose_HLSLMaterialTranslator : public FHLSLMaterialTranslator
{
public:
	// static auto getAddCodeChunkInner2()->int32(FMaterialCompiler::*)(uint64, const TCHAR*, EMaterialValueType, EDerivativeStatus,bool)
	// {
	// 	return static_cast<int32(FMaterialCompiler::*)(uint64, const TCHAR*, EMaterialValueType, EDerivativeStatus,bool)>(&Expose_HLSLMaterialTranslator::AddCodeChunkInner);
	// }
	
	// static auto callAddCodeChunkInner2(FMaterialCompiler* Compiler, uint64 Hash, const TCHAR* FormattedCode, EMaterialValueType Type, EDerivativeStatus DerivativeStatus, bool bInlined)
	// {
	// 	auto x = Expose_HLSLMaterialTranslator::getAddCodeChunkInner2();
	// 	
	// 	return (*Compiler.*x)(Hash, FormattedCode, Type, DerivativeStatus, bInlined);
	// }

	//Returns a function pointer
	static auto getStrataGetCastParameterCode()->FString((FHLSLMaterialTranslator::*)(int32 Index, EMaterialValueType DestType))
	{
		return &Expose_HLSLMaterialTranslator::StrataGetCastParameterCode;
	}

	//Calls the function pointer on FHLSLMaterialTranslator instead of Expose_HLSLMaterialTranslator
	static auto callStrataGetCastParameterCode(FMaterialCompiler* Compiler, int32 Index, EMaterialValueType DestType)
	{
		auto x = Expose_HLSLMaterialTranslator::getStrataGetCastParameterCode();
		auto MyCompiler = static_cast<FHLSLMaterialTranslator*>(Compiler);
		return (*MyCompiler.*x)(Index, DestType);
	}

	//Returns a function pointer
	static auto getAddCodeChunkInner()->int32(FMaterialCompiler::*)(EMaterialValueType, EDerivativeStatus, bool, const TCHAR*, ...)
	{
		return static_cast<int32(FMaterialCompiler::*)(EMaterialValueType, EDerivativeStatus, bool, const TCHAR*,...)>(&Expose_HLSLMaterialTranslator::AddCodeChunkInner);
	}

	//Calls the function pointer on FHLSLMaterialTranslator instead of Expose_HLSLMaterialTranslator
	template<typename ...Args>
	static auto callAddCodeChunkInner(FMaterialCompiler* Compiler, EMaterialValueType Type, EDerivativeStatus DerivativeStatus, bool bInlined, const TCHAR* Format, Args... args)
	{
		auto x = Expose_HLSLMaterialTranslator::getAddCodeChunkInner();

		return (*Compiler.*x)(Type, DerivativeStatus, bInlined,Format,args...);
	}


	//Returns a function pointer
	static auto GetGetParameterCode()->FString((FHLSLMaterialTranslator::*)(int32 Index, const TCHAR* Default))
	{
		return &Expose_HLSLMaterialTranslator::GetParameterCode;
	}

	//Calls the function pointer on FHLSLMaterialTranslator instead of Expose_HLSLMaterialTranslator
	static auto callGetParameterCode(FMaterialCompiler* Compiler,int32 Index, const TCHAR* Default=0)
	{
		auto x = Expose_HLSLMaterialTranslator::GetGetParameterCode();
		auto MyCompiler = static_cast<FHLSLMaterialTranslator*>(Compiler);
		
		return (*MyCompiler.*x)(Index, Default);
	}
	
};
