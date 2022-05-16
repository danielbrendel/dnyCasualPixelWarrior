#include "scriptint.h"
#include <scriptstdstring\scriptstdstring.h>
#include <scriptmath\scriptmath.h>
#include "utils.h"
#include "vars.h"
#include "game.h"

/*
	Casual Game Engine (dnyCasualGameEngine) developed by Daniel Brendel

	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

namespace Scripting {
	CScriptInt::CScriptInt(const std::string& szScriptDir, void* pCallbackFunction) : m_bInitialized(false)
	{
		//Construct object

		this->Initialize(szScriptDir, pCallbackFunction);
	}

	bool CScriptInt::Initialize(const std::string& szScriptDir, void* pCallbackFunction)
	{
		//Initialize scripting interface object

		if (/*(!szScriptDir.length()) ||*/ (!pCallbackFunction))
			return false;

		//Initialize AngelScript
		this->m_pScriptEngine = asCreateScriptEngine();
		if (!this->m_pScriptEngine)
			return false;

		//Set message callback function
		if (AS_FAILED(this->m_pScriptEngine->SetMessageCallback(asFUNCTION(pCallbackFunction), 0, asCALL_CDECL))) {
			this->m_pScriptEngine->ShutDownAndRelease();
			return false;
		}

		//Register string type
		RegisterStdString(this->m_pScriptEngine);

		//Register array type
		RegisterScriptArray(this->m_pScriptEngine, false);

		//Register math functions
		RegisterScriptMath(this->m_pScriptEngine);

		//Save script path
		this->m_szScriptPath = szScriptDir;

		//Set and return indicator value
		return this->m_bInitialized = true;
	}

	void CScriptInt::Shutdown(void)
	{
		//Shutdown script interface object

		if (!this->m_bInitialized)
			return;

		//Unload scripts
		for (size_t i = 0; i < this->m_vScripts.size(); i++) {
			this->UnloadScript(i);
		}

		//Shutdown AngelScript
		if (this->m_pScriptEngine) {
			this->m_pScriptEngine->ShutDownAndRelease();
			this->m_pScriptEngine = nullptr;
		}

		//Clear indicator
		this->m_bInitialized = false;
	}

	HSISCRIPT CScriptInt::LoadScript(const std::string& szScriptName)
	{
		//Load script
		
		if ((!this->m_bInitialized) || (!szScriptName.length()))
			return SI_INVALID_ID;

		CScriptBuilder oScriptBuilder;

		//Set include callback
		oScriptBuilder.SetIncludeCallback(&ScriptInt_IncludeCallback, nullptr);
			
		//Start new module
		if (AS_FAILED(oScriptBuilder.StartNewModule(this->m_pScriptEngine, szScriptName.c_str())))
			return SI_INVALID_ID;
		
		//Add script file to module
		if (AS_FAILED(oScriptBuilder.AddSectionFromFile((/*this->m_szScriptPath +*/ szScriptName).c_str())))
			return SI_INVALID_ID;
		
		//Build script module
		if (AS_FAILED(oScriptBuilder.BuildModule()))
			return SI_INVALID_ID;
		
		//Setup struct
		si_script_s sScriptData;
		sScriptData.szName = szScriptName;
		sScriptData.pModule = this->m_pScriptEngine->GetModule(szScriptName.c_str()); //Get pointer to script module
		if (!sScriptData.pModule)
			return SI_INVALID_ID;
		
		//Add to list
		this->m_vScripts.push_back(sScriptData);

		return this->m_vScripts.size() - 1; //Return entry ID
	}

	bool CScriptInt::UnloadScript(const HSISCRIPT hScript)
	{
		//Unload a script

		if (!this->m_bInitialized)
			return false;

		//Validate script handle
		if ((hScript == SI_INVALID_ID) || (hScript >= this->m_vScripts.size()))
			return false;

		//Discard module object
		this->m_vScripts[hScript].pModule->Discard();

		//Unregister from AngelScript
		this->m_pScriptEngine->DiscardModule(this->m_vScripts[hScript].szName.c_str());

		//Remove from list
		this->m_vScripts.erase(this->m_vScripts.begin() + hScript);

		return true;
	}
	
	bool CScriptInt::CallScriptFunction(const HSISCRIPT hScript, const bool bIsName, const std::string& szFunctionNameOrDeclaration, const std::vector<si_func_arg>* pArgs, void* pResult, func_args_e eResultType)
	{
		//Call function from script file

		if (!this->m_bInitialized)
			return false;

		//Validate script handle
		if ((hScript == SI_INVALID_ID) || (hScript >= this->m_vScripts.size()))
			return false;
		
		//Get function by declaration
		asIScriptFunction* pFunction = (bIsName) ? this->m_vScripts[hScript].pModule->GetFunctionByName(szFunctionNameOrDeclaration.c_str()) : this->m_vScripts[hScript].pModule->GetFunctionByDecl(szFunctionNameOrDeclaration.c_str());
		if (!pFunction)
			return false;
		
		//Create calling context
		asIScriptContext* pContext = this->m_pScriptEngine->CreateContext();
		if (!pContext)
			return false;
		
		//Prepare call stack
		if (AS_FAILED(pContext->Prepare(pFunction))) {
			pContext->Release();
			return false;
		}
		
		//Push arguments if desired
		if (pArgs) {
			int iArgResult = 0;

			for (size_t i = 0; i < pArgs->size(); i++) { //Enumerate through arguments
				switch ((*pArgs)[i].eType) {
					//Pass argument according to type
				case FA_BYTE:
					iArgResult = pContext->SetArgByte((asUINT)i, (*pArgs)[i].byte);
					break;
				case FA_WORD:
					iArgResult = pContext->SetArgWord((asUINT)i, (*pArgs)[i].word);
					break;
				case FA_DWORD:
					iArgResult = pContext->SetArgDWord((asUINT)i, (*pArgs)[i].dword);
					break;
				case FA_QWORD:
					iArgResult = pContext->SetArgQWord((asUINT)i, (*pArgs)[i].qword);
					break;
				case FA_FLOAT:
					iArgResult = pContext->SetArgFloat((asUINT)i, (*pArgs)[i].flt);
					break;
				case FA_DOUBLE:
					iArgResult = pContext->SetArgDouble((asUINT)i, (*pArgs)[i].dbl);
					break;
				case FA_STRING:
					iArgResult = pContext->SetArgAddress((asUINT)i, (*pArgs)[i].pstr);
					break;
				case FA_POINTER:
					iArgResult = pContext->SetArgAddress((asUINT)i, (*pArgs)[i].ptr);
					break;
				case FA_OBJECT:
					iArgResult = pContext->SetArgObject((asUINT)i, (*pArgs)[i].ptr);
					break;
				default:
					pContext->Release();
					return false;
					break;
				}

				//Validate result
				if (AS_FAILED(iArgResult)) {
					pContext->Release();
					return false;
				}
			}
		}

		//Call function
		if (!AS_EXECUTED(pContext->Execute())) {
			pContext->Release();
			return false;
		}

		//Get return value if desired
		if (pResult) {
			switch (eResultType) {
				//Get result value according to type
			case FA_BYTE:
				*(asBYTE*)pResult = pContext->GetReturnByte();
				break;
			case FA_WORD:
				*(asWORD*)pResult = pContext->GetReturnWord();
				break;
			case FA_DWORD:
				*(asDWORD*)pResult = pContext->GetReturnDWord();
				break;
			case FA_QWORD:
				*(asQWORD*)pResult = pContext->GetReturnQWord();
				break;
			case FA_FLOAT:
				*(float*)pResult = pContext->GetReturnFloat();
				break;
			case FA_DOUBLE:
				*(double*)pResult = pContext->GetReturnDouble();
				break;
			case FA_STRING:
				*(std::string*)pResult = *(std::string*)pContext->GetReturnObject();
				break;
			case FA_POINTER:
				*(void**)pResult = pContext->GetReturnAddress();
				break;
			case FA_OBJECT:
				*(void**)pResult = pContext->GetReturnObject();
				break;
			default:
				pContext->Release();
				return false;
				break;
			}
		}

		pContext->Release();

		return true;
	}

	bool CScriptInt::CallScriptFunction(asIScriptFunction* pFunction, const std::vector<si_func_arg>* pArgs, void* pResult, func_args_e eResultType)
	{
		//Call function from script file

		if (!this->m_bInitialized)
			return false;

		if (!pFunction)
			return false;

		//Create calling context
		asIScriptContext* pContext = this->m_pScriptEngine->CreateContext();
		if (!pContext)
			return false;

		//Prepare call stack
		if (AS_FAILED(pContext->Prepare(pFunction))) {
			pContext->Release();
			return false;
		}

		//Push arguments if desired
		if (pArgs) {
			int iArgResult = 0;

			for (size_t i = 0; i < pArgs->size(); i++) { //Enumerate through arguments
				switch ((*pArgs)[i].eType) {
					//Pass argument according to type
				case FA_BYTE:
					iArgResult = pContext->SetArgByte((asUINT)i, (*pArgs)[i].byte);
					break;
				case FA_WORD:
					iArgResult = pContext->SetArgWord((asUINT)i, (*pArgs)[i].word);
					break;
				case FA_DWORD:
					iArgResult = pContext->SetArgDWord((asUINT)i, (*pArgs)[i].dword);
					break;
				case FA_QWORD:
					iArgResult = pContext->SetArgQWord((asUINT)i, (*pArgs)[i].qword);
					break;
				case FA_FLOAT:
					iArgResult = pContext->SetArgFloat((asUINT)i, (*pArgs)[i].flt);
					break;
				case FA_DOUBLE:
					iArgResult = pContext->SetArgDouble((asUINT)i, (*pArgs)[i].dbl);
					break;
				case FA_STRING:
					iArgResult = pContext->SetArgAddress((asUINT)i, (*pArgs)[i].pstr);
					break;
				case FA_POINTER:
					iArgResult = pContext->SetArgAddress((asUINT)i, (*pArgs)[i].ptr);
					break;
				case FA_OBJECT:
					iArgResult = pContext->SetArgObject((asUINT)i, (*pArgs)[i].ptr);
					break;
				default:
					pContext->Release();
					return false;
					break;
				}

				//Validate result
				if (AS_FAILED(iArgResult)) {
					pContext->Release();
					return false;
				}
			}
		}

		//Call function
		if (!AS_EXECUTED(pContext->Execute())) {
			pContext->Release();
			return false;
		}

		//Get return value if desired
		if (pResult) {
			switch (eResultType) {
				//Get result value according to type
			case FA_BYTE:
				*(asBYTE*)pResult = pContext->GetReturnByte();
				break;
			case FA_WORD:
				*(asWORD*)pResult = pContext->GetReturnWord();
				break;
			case FA_DWORD:
				*(asDWORD*)pResult = pContext->GetReturnDWord();
				break;
			case FA_QWORD:
				*(asQWORD*)pResult = pContext->GetReturnQWord();
				break;
			case FA_FLOAT:
				*(float*)pResult = pContext->GetReturnFloat();
				break;
			case FA_DOUBLE:
				*(double*)pResult = pContext->GetReturnDouble();
				break;
			case FA_STRING:
				*(std::string*)pResult = *(std::string*)pContext->GetReturnObject();
				break;
			case FA_POINTER:
				*(void**)pResult = pContext->GetReturnAddress();
				break;
			default:
				pContext->Release();
				return false;
				break;
			}
		}

		pContext->Release();

		return true;
	}

	bool CScriptInt::ScriptFunctionExists(const HSISCRIPT hScript, const std::string& szFunctionName)
	{
		//Check if a script function does exist

		if (!this->m_bInitialized)
			return false;

		//Validate script handle
		if ((hScript == SI_INVALID_ID) || (hScript >= this->m_vScripts.size()))
			return false;

		//Get function by name
		asIScriptFunction* pFunction = this->m_vScripts[hScript].pModule->GetFunctionByName(szFunctionName.c_str());

		return pFunction != nullptr;
	}

	HSIENUM CScriptInt::RegisterEnumeration(const std::string& szName)
	{
		//Register new enumeration

		if (!this->m_bInitialized)
			return SI_INVALID_ID;

		//Register new enum
		if (AS_FAILED(this->m_pScriptEngine->RegisterEnum(szName.c_str())))
			return SI_INVALID_ID;

		//Setup data struct
		si_enum_s sEnum;
		sEnum.szName = szName;

		//Add to list
		this->m_vEnums.push_back(sEnum);

		return this->m_vEnums.size() - 1; //Return entry ID
	}

	bool CScriptInt::AddEnumerationValue(const HSIENUM hEnum, const std::string& szName, const int iValue)
	{
		//Add value to enumeration

		if (!this->m_bInitialized)
			return false;

		//Validate enum handle
		if ((hEnum == SI_INVALID_ID) || (hEnum >= this->m_vEnums.size()))
			return false;

		return (!AS_FAILED(this->m_pScriptEngine->RegisterEnumValue(this->m_vEnums[hEnum].szName.c_str(), szName.c_str(), iValue)));
	}

	bool CScriptInt::RegisterTypeDef(const std::string& szType, const std::string& szAlias)
	{
		//Register new typedef
		
		return (!AS_FAILED(this->m_pScriptEngine->RegisterTypedef(szAlias.c_str(), szType.c_str())));
	}

	bool CScriptInt::RegisterFuncDef(const std::string& szFuncDef)
	{
		//Register new function definition

		return (!AS_FAILED(this->m_pScriptEngine->RegisterFuncdef(szFuncDef.c_str())));
	}

	HSISTRUCT CScriptInt::RegisterStructure(const std::string& szStructName, size_t uiStructSize)
	{
		//Register new structure

		if (!this->m_bInitialized)
			return SI_INVALID_ID;

		//Register new object type
		if (AS_FAILED(this->m_pScriptEngine->RegisterObjectType(szStructName.c_str(), (int)uiStructSize, asOBJ_VALUE | asOBJ_POD)))
			return SI_INVALID_ID;

		//Setup data struct
		si_struct_s sStruct;
		sStruct.szName = szStructName;
		sStruct.uiSize = uiStructSize;

		//Add to list
		this->m_vStructs.push_back(sStruct);

		return this->m_vStructs.size() - 1; //Return entry ID
	}

	bool CScriptInt::AddStructureMember(const HSISTRUCT hStruct, const std::string& szMember, int iOffset)
	{
		//Add member to structure

		if (!this->m_bInitialized)
			return false;

		//Validate struct handle
		if ((hStruct == SI_INVALID_ID) || (hStruct >= this->m_vStructs.size()))
			return false;

		//Add member to structure
		return (!AS_FAILED(this->m_pScriptEngine->RegisterObjectProperty(this->m_vStructs[hStruct].szName.c_str(), szMember.c_str(), iOffset)));
	}

	HSICLASS CScriptInt::RegisterClass_(const std::string& szClassName, size_t uiClassSize, const asDWORD dwObjectFlags)
	{
		//Register new class

		if (!this->m_bInitialized)
			return SI_INVALID_ID;

		//Register new object type
		if (AS_FAILED(this->m_pScriptEngine->RegisterObjectType(szClassName.c_str(), (int)uiClassSize, dwObjectFlags)))
			return SI_INVALID_ID;

		//Setup data struct
		si_class_s sClass;
		sClass.szName = szClassName;
		sClass.uiSize = uiClassSize;

		//Add to list
		this->m_vClasses.push_back(sClass);

		return this->m_vClasses.size() - 1; //Return entry ID
	}

	bool CScriptInt::AddClassMember(const HSICLASS hClass, const std::string& szMember, int iOffset)
	{
		//Add member to class

		if (!this->m_bInitialized)
			return false;

		//Validate class handle
		if ((hClass == SI_INVALID_ID) || (hClass >= this->m_vClasses.size()))
			return false;

		//Add member to class
		return (!AS_FAILED(this->m_pScriptEngine->RegisterObjectProperty(this->m_vClasses[hClass].szName.c_str(), szMember.c_str(), iOffset)));
	}

	bool CScriptInt::AddClassMethod(const HSICLASS hClass, const std::string& szFunction, const asSFuncPtr& pFunction)
	{
		//Add method to class

		if (!this->m_bInitialized)
			return false;

		//Validate class handle
		if ((hClass == SI_INVALID_ID) || (hClass >= this->m_vClasses.size()))
			return false;

		//Add method to class
		return (!AS_FAILED(this->m_pScriptEngine->RegisterObjectMethod(this->m_vClasses[hClass].szName.c_str(), szFunction.c_str(), pFunction, asCALL_THISCALL)));
	}

	bool CScriptInt::AddClassBehaviour(const HSICLASS hClass, const asEBehaviours eBehavior, const std::string& szTypeDef, const asSFuncPtr& pMethod)
	{
		//Add behaviour to class

		if (!this->m_bInitialized)
			return false;

		//Validate class handle
		if ((hClass == SI_INVALID_ID) || (hClass >= this->m_vClasses.size()))
			return false;

		//Add behaviour to class
		return (!AS_FAILED(this->m_pScriptEngine->RegisterObjectBehaviour(this->m_vClasses[hClass].szName.c_str(), eBehavior, szTypeDef.c_str(), pMethod, asCALL_THISCALL)));
	}

	bool CScriptInt::RegisterFunction(const std::string& szFunction, const void* pFunction, asECallConvTypes eCallingConvention)
	{
		//Register function

		if (!this->m_bInitialized)
			return false;

		if ((!szFunction.length()) || (!pFunction))
			return false;

		//Register global function
		return (!AS_FAILED(this->m_pScriptEngine->RegisterGlobalFunction(szFunction.c_str(), asFUNCTION(pFunction), eCallingConvention)));
	}

	bool CScriptInt::RegisterGlobalVariable(const std::string& szDef, void* pVariable)
	{
		//Register global variable object

		if (!this->m_bInitialized)
			return false;

		if ((!szDef.length()) || (!pVariable))
			return false;

		//Register global function
		return (!AS_FAILED(this->m_pScriptEngine->RegisterGlobalProperty(szDef.c_str(), pVariable)));
	}

	asITypeInfo* CScriptInt::GetTypeInfo(const std::string& szTypeText, bool bNameOrDef)
	{
		//Get type info either by name or typedef

		if (!this->m_bInitialized)
			return nullptr;

		if (bNameOrDef)
			return this->m_pScriptEngine->GetTypeInfoByName(szTypeText.c_str());

		return this->m_pScriptEngine->GetTypeInfoByDecl(szTypeText.c_str());
	}

	asIScriptObject* CScriptInt::AllocClass(const HSISCRIPT hScript, const std::string& szClassName)
	{
		//Allocate class instance of script

		if (!szClassName.length())
			return nullptr;

		if (hScript >= this->m_vScripts.size())
			return nullptr;

		//Get type info
		asITypeInfo* pTypeInfo = this->m_pScriptEngine->GetTypeInfoByDecl(szClassName.c_str());
		if (!pTypeInfo)
			return nullptr;

		//Query allocation factory
		asIScriptFunction* pFactory = pTypeInfo->GetFactoryByDecl((szClassName + "@ New_" + szClassName + "()").c_str());
		if (!pFactory) {
			pTypeInfo->Release();
			return nullptr;
		}

		//Create calling context
		asIScriptContext* pContext = this->m_pScriptEngine->CreateContext();
		if (!pContext) {
			pTypeInfo->Release();
			return nullptr;
		}

		//Prepare and call function
		pContext->Prepare(pFactory);
		pContext->Execute();

		//Obtain handle to returned object
		asIScriptObject* pObject = *(asIScriptObject**)pContext->GetAddressOfReturnValue();
		if (!pObject) {
			pContext->Release();
			pTypeInfo->Release();
		}
		
		//Increment reference counter in order to store object
		pObject->AddRef();

		return pObject;
	}

	bool CScriptInt::CallScriptMethod(const HSISCRIPT hScript, asIScriptObject* pClassInstance, const std::string& szMethodDef, const std::vector<si_func_arg>* pArgs, void* pResult, func_args_e eResultType)
	{
		if (!this->m_bInitialized)
			return false;

		if (!pClassInstance)
			return false;
		
		//Create calling context
		asIScriptContext* pContext = this->m_pScriptEngine->CreateContext();
		if (!pContext)
			return false;

		//Query object type
		asITypeInfo* pTypeInfo = pClassInstance->GetObjectType();
		if (!pTypeInfo) {
			pContext->Release();
			return false;
		}

		//Query class method
		asIScriptFunction* pFunction = pTypeInfo->GetMethodByDecl(szMethodDef.c_str());
		if (!pFunction) {
			pContext->Release();
			return false;
		}

		//Prepare call stack
		if (AS_FAILED(pContext->Prepare(pFunction))) {
			pContext->Release();
			return false;
		}

		//Set object instance pointer
		if (AS_FAILED(pContext->SetObject(pClassInstance))) {
			pContext->Release();
			return false;
		}

		//Push arguments if desired
		if (pArgs) {
			int iArgResult = 0;

			for (size_t i = 0; i < pArgs->size(); i++) { //Enumerate through arguments
				switch ((*pArgs)[i].eType) {
					//Pass argument according to type
				case FA_BYTE:
					iArgResult = pContext->SetArgByte((asUINT)i, (*pArgs)[i].byte);
					break;
				case FA_WORD:
					iArgResult = pContext->SetArgWord((asUINT)i, (*pArgs)[i].word);
					break;
				case FA_DWORD:
					iArgResult = pContext->SetArgDWord((asUINT)i, (*pArgs)[i].dword);
					break;
				case FA_QWORD:
					iArgResult = pContext->SetArgQWord((asUINT)i, (*pArgs)[i].qword);
					break;
				case FA_FLOAT:
					iArgResult = pContext->SetArgFloat((asUINT)i, (*pArgs)[i].flt);
					break;
				case FA_DOUBLE:
					iArgResult = pContext->SetArgDouble((asUINT)i, (*pArgs)[i].dbl);
					break;
				case FA_STRING:
					iArgResult = pContext->SetArgAddress((asUINT)i, (*pArgs)[i].pstr);
					break;
				case FA_POINTER:
					iArgResult = pContext->SetArgAddress((asUINT)i, (*pArgs)[i].ptr);
					break;
				case FA_OBJECT:
					iArgResult = pContext->SetArgObject((asUINT)i, (*pArgs)[i].ptr);
					break;
				default:
					pContext->Release();
					return false;
					break;
				}

				//Validate result
				if (AS_FAILED(iArgResult)) {
					pContext->Release();
					return false;
				}
			}
		}

		//Call function
		if (!AS_EXECUTED(pContext->Execute())) {
			pContext->Release();
			return false;
		}

		//Get return value if desired
		if (pResult) {
			switch (eResultType) {
				//Get result value according to type
			case FA_BYTE:
				*(asBYTE*)pResult = pContext->GetReturnByte();
				break;
			case FA_WORD:
				*(asWORD*)pResult = pContext->GetReturnWord();
				break;
			case FA_DWORD:
				*(asDWORD*)pResult = pContext->GetReturnDWord();
				break;
			case FA_QWORD:
				*(asQWORD*)pResult = pContext->GetReturnQWord();
				break;
			case FA_FLOAT:
				*(float*)pResult = pContext->GetReturnFloat();
				break;
			case FA_DOUBLE:
				*(double*)pResult = pContext->GetReturnDouble();
				break;
			case FA_STRING:
				*(std::string*)pResult = *(std::string*)pContext->GetReturnObject();
				break;
			case FA_POINTER:
				*(void**)pResult = pContext->GetReturnAddress();
				break;
			case FA_OBJECT:
				*(void**)pResult = pContext->GetReturnAddress();
				break;
			default:
				pContext->Release();
				return false;
				break;
			}
		}

		pContext->Release();

		return true;
	}

	bool CScriptInt::RegisterInterface(const std::string& szName)
	{
		//Register interface

		return (!(AS_FAILED(this->m_pScriptEngine->RegisterInterface(szName.c_str()))));
	}
	
	bool CScriptInt::RegisterInterfaceMethod(const std::string& szIfName, const std::string& szTypedef)
	{
		//Register interface method

		return (!(AS_FAILED(this->m_pScriptEngine->RegisterInterfaceMethod(szIfName.c_str(), szTypedef.c_str()))));
	}

	int ScriptInt_IncludeCallback(const char* include, const char* from, CScriptBuilder* builder, void* userParam)
	{
		//Called for include directives

		std::string szInclude = std::string(include);
	
		if (szInclude.find("${COMMON}") != std::string::npos) {
			//Replace common directive variable
			szInclude = Utils::ReplaceString(szInclude, "${COMMON}", Utils::ConvertToAnsiString(wszBasePath) + "packages\\.common");
		} else {
			if (std::string(from).find(Utils::ReplaceString(Utils::ConvertToAnsiString(wszBasePath), "\\", "/") + "packages/.common") == 0) { //Provide path to scripts from common path context
				szInclude = Utils::ConvertToAnsiString(wszBasePath) + "packages\\.common\\entities\\" + szInclude;
			} else { //Provide absolute path to relative includes from package path
				szInclude = Utils::ConvertToAnsiString(Game::pGame->GetPackagePath()) + "entities\\" + szInclude;
			}
		}

		builder->AddSectionFromFile(szInclude.c_str());

		return 0;
	}
}