#pragma once

/*
	Casual Game Engine (dnyCasualGameEngine) developed by Daniel Brendel

	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#include "shared.h"
#include <angelscript.h>
#include <scriptbuilder\scriptbuilder.h>
#include <scriptarray\scriptarray.h>

//Convenience macros
#define AS_FAILED(r) (r < 0)
#define AS_EXECUTED(r) (r == asEXECUTION_FINISHED)
#define SI_INVALID_ID ((size_t)-1)
#define BEGIN_PARAMS(lv) std::vector<Scripting::si_func_arg> lv; Scripting::si_func_arg lv##_sSIArg_;
#define PUSH_PARAM(t, n, v, lv) lv##_sSIArg_.eType = t; lv##_sSIArg_.##n = v; lv.push_back(lv##_sSIArg_);
#define PUSH_BOOL(var) PUSH_PARAM(Scripting::FA_BYTE, byte, (byte)var, vArgs);
#define PUSH_BYTE(var) PUSH_PARAM(Scripting::FA_BYTE, byte, var, vArgs);
#define PUSH_WORD(var) PUSH_PARAM(Scripting::FA_WORD, word, var, vArgs);
#define PUSH_DWORD(var) PUSH_PARAM(Scripting::FA_DWORD, dword, var, vArgs);
#define PUSH_QWORD(var) PUSH_PARAM(Scripting::FA_QWORD, qword, var, vArgs);
#define PUSH_FLOAT(var) PUSH_PARAM(Scripting::FA_FLOAT, flt, var, vArgs);
#define PUSH_DOUBLE(var) PUSH_PARAM(Scripting::FA_DOUBLE, dbl, var, vArgs);
#define PUSH_STRING(var) PUSH_PARAM(Scripting::FA_STRING, pstr, &var, vArgs);
#define PUSH_POINTER(var) PUSH_PARAM(Scripting::FA_POINTER, ptr, var, vArgs);
#define PUSH_OBJECT(var) PUSH_PARAM(Scripting::FA_OBJECT, ptr, var, vArgs);
#define END_PARAMS(lv) lv.clear();

/* Scripting environment */
namespace Scripting {
	enum func_args_e {
		FA_VOID,
		FA_BYTE,
		FA_WORD,
		FA_DWORD,
		FA_QWORD,
		FA_FLOAT,
		FA_DOUBLE,
		FA_STRING,
		FA_POINTER,
		FA_OBJECT
	};

	typedef size_t HSIHANDLE;
	typedef HSIHANDLE HSISCRIPT;
	typedef HSIHANDLE HSIENUM;
	typedef HSIHANDLE HSISTRUCT;
	typedef HSIHANDLE HSICLASS;
	typedef size_t D3DFONTDESC;
	typedef struct si_struct_s si_class_s;

	struct si_script_s {
		std::string szName;
		asIScriptModule* pModule;
	};

	struct si_enum_s {
		std::string szName;
	};

	struct si_struct_s {
		std::string szName;
		size_t uiSize;
	};

	struct si_func_arg {
		func_args_e eType;
		union {
			asBYTE byte;
			asWORD word;
			asDWORD dword;
			asQWORD qword;
			float flt;
			double dbl;
			std::string* pstr;
			void* ptr;
		};
	};

	int ScriptInt_IncludeCallback(const char* include, const char* from, CScriptBuilder* builder, void* userParam);

	/* Scripting interface component */
	class CScriptInt {
	private:
		bool m_bInitialized;
		std::string m_szScriptPath;
		asIScriptEngine* m_pScriptEngine;
		std::vector<si_script_s> m_vScripts;
		std::vector<si_enum_s> m_vEnums;
		std::vector<si_struct_s> m_vStructs;
		std::vector<si_class_s> m_vClasses;

		friend int ScriptInt_IncludeCallback(const char* include, const char* from, CScriptBuilder* builder, void* userParam);
	public:
		CScriptInt() : m_bInitialized(false) {}
		CScriptInt(const std::string& szScriptDir, void* pCallbackFunction);
		~CScriptInt() { if (this->m_bInitialized) this->Shutdown(); }

		virtual inline bool IsReady(void) { return this->m_bInitialized; }

		virtual bool Initialize(const std::string& szScriptDir, void* pCallbackFunction);
		virtual void Shutdown(void);

		virtual HSISCRIPT LoadScript(const std::string& szScriptName);
		virtual bool UnloadScript(const HSISCRIPT hScript);

		virtual bool CallScriptFunction(const HSISCRIPT hScript, const bool bIsName, const std::string& szFunctionNameOrDeclaration, const std::vector<si_func_arg>* pArgs, void* pResult, func_args_e eResultType = FA_VOID);
		bool CallScriptFunction(asIScriptFunction* pFunction, const std::vector<si_func_arg>* pArgs, void* pResult, func_args_e eResultType = FA_VOID);
		virtual bool ScriptFunctionExists(const HSISCRIPT hScript, const std::string& szFunctionName);

		virtual HSIENUM RegisterEnumeration(const std::string& szName);
		virtual bool AddEnumerationValue(const HSIENUM hEnum, const std::string& szName, const int iValue);
		virtual bool RegisterTypeDef(const std::string& szType, const std::string& szAlias);
		virtual bool RegisterFuncDef(const std::string& szFuncDef);
		virtual HSISTRUCT RegisterStructure(const std::string& szStructName, size_t uiStructSize);
		virtual bool AddStructureMember(const HSISTRUCT hStruct, const std::string& szMember, int iOffset);
		virtual bool RegisterFunction(const std::string& szFunction, const void* pFunction, asECallConvTypes eCallingConvention = asCALL_CDECL);
		virtual bool RegisterGlobalVariable(const std::string& szDef, void* pVariable);
		virtual HSICLASS RegisterClass_(const std::string& szClassName, size_t uiClassSize, const asDWORD dwObjectFlags);
		virtual bool AddClassMember(const HSICLASS hClass, const std::string& szMember, int iOffset);
		virtual bool AddClassMethod(const HSICLASS hClass, const std::string& szFunction, const asSFuncPtr& pFunction);
		virtual bool AddClassBehaviour(const HSICLASS hClass, const asEBehaviours eBehavior, const std::string& szTypeDef, const asSFuncPtr& pMethod);
		virtual asITypeInfo* GetTypeInfo(const std::string& szTypeText, bool bNameOrDef);
		virtual asIScriptObject* AllocClass(const HSISCRIPT hScript, const std::string& szClassName);
		virtual bool CallScriptMethod(const HSISCRIPT hScript, asIScriptObject* pClassInstance, const std::string& szMethodDef, const std::vector<si_func_arg>* pArgs, void* pResult, func_args_e eResultType = FA_VOID);
		virtual bool RegisterInterface(const std::string& szName);
		virtual bool RegisterInterfaceMethod(const std::string& szIfName, const std::string& szTypedef);
	};
}

