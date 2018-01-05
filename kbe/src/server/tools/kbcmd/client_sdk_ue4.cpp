/*
This source file is part of KBEngine
For the latest info, see http://www.kbengine.org/

Copyright (c) 2008-2018 KBEngine.

KBEngine is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

KBEngine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.
 
You should have received a copy of the GNU Lesser General Public License
along with KBEngine.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "kbcmd.h"
#include "client_sdk.h"
#include "client_sdk_ue4.h"	
#include "entitydef/entitydef.h"
#include "entitydef/scriptdef_module.h"
#include "entitydef/property.h"
#include "entitydef/method.h"
#include "entitydef/datatype.h"
#include "network/fixed_messages.h"

namespace KBEngine {	

static std::string headerBody = "/*\n\tGenerated by KBEngine!\n\tPlease do not modify this file!\n#REPLACE#\ttools = kbcmd\n*/\n\n";
static std::string moduleSuffix = "Base";

//-------------------------------------------------------------------------------------
ClientSDKUE4::ClientSDKUE4():
	ClientSDK(),
	initBody_()
{
}

//-------------------------------------------------------------------------------------
ClientSDKUE4::~ClientSDKUE4()
{

}

//-------------------------------------------------------------------------------------
std::string ClientSDKUE4::typeToType(const std::string& type)
{
	if (type == "INT8")
	{
		return "int8";
	}
	else if (type == "INT16")
	{
		return "int16";
	}
	else if (type == "INT32")
	{
		return "int32";
	}
	else if (type == "INT64")
	{
		return "int64";
	}
	else if (type == "UINT8")
	{
		return "uint8";
	}
	else if (type == "UINT16")
	{
		return "uint16";
	}
	else if (type == "UINT32")
	{
		return "uint32";
	}
	else if (type == "UINT64")
	{
		return "uint64";
	}
	else if (type == "FLOAT")
	{
		return "float";
	}
	else if (type == "DOUBLE")
	{
		return "double";
	}
	else if (type == "STRING")
	{
		return "FString";
	}
	else if (type == "UNICODE")
	{
		return "FString";
	}
	else if (type == "PYTHON")
	{
		return type;
	}
	else if (type == "PY_DICT")
	{
		return type;
	}
	else if (type == "PY_TUPLE")
	{
		return type;
	}
	else if (type == "PY_LIST")
	{
		return type;
	}
	else if (type == "BLOB")
	{
		return "byte[]";
	}
	else if (type == "ARRAY")
	{
		return "TArray";
	}
	else if (type == "FIXED_DICT")
	{
		return "TMap";
	}
#ifdef CLIENT_NO_FLOAT
	else if (type == "VECTOR2")
	{
		return "FVector2D";
	}
	else if (type == "VECTOR3")
	{
		return "FVector";
	}
	else if (type == "VECTOR4")
	{
		return "FVector4";
	}
#else
	else if (type == "VECTOR2")
	{
		return "FVector2D";
	}
	else if (type == "VECTOR3")
	{
		return "FVector";
	}
	else if (type == "VECTOR4")
	{
		return "FVector4";
	}
#endif
	else if (type == "MAILBOX")
	{
		return "TArray<uint8>";
	}

	return type;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::getArrayType(DataType* pDataType, std::string& outstr)
{
	if (pDataType->type() == DATA_TYPE_FIXEDARRAY)
	{
		FixedArrayType* pFixedArrayType = static_cast<FixedArrayType*>(pDataType);

		// 如果元素又是数组
		if (pFixedArrayType->getDataType()->type() == DATA_TYPE_FIXEDARRAY)
		{
			if (outstr.size() > 0)
				strutil::kbe_replace(outstr, pDataType->aliasName(), fmt::format("TArray<{}>", pFixedArrayType->getDataType()->aliasName()));
			else
				outstr = fmt::format("TArray<{}>", pFixedArrayType->getDataType()->aliasName());

			return getArrayType(pFixedArrayType->getDataType(), outstr);
		}
		else if (pFixedArrayType->getDataType()->type() == DATA_TYPE_FIXEDDICT)
		{
			if (outstr.size() > 0)
				strutil::kbe_replace(outstr, pDataType->aliasName(), pFixedArrayType->getDataType()->aliasName());
			else
				outstr = pFixedArrayType->getDataType()->aliasName();
		}
		else
		{
			if (outstr.size() > 0)
				strutil::kbe_replace(outstr, pDataType->aliasName(), typeToType(pFixedArrayType->getDataType()->getName()));
			else
				outstr = typeToType(pFixedArrayType->getDataType()->getName());
		}
	}

	outstr = fmt::format("TArray<{}>", outstr);
	return true;
}

//-------------------------------------------------------------------------------------
void ClientSDKUE4::onCreateTypeFileName()
{
	sourcefileName_ = "KBETypes.h";
}

//-------------------------------------------------------------------------------------
void ClientSDKUE4::onCreateEntityModuleFileName(const std::string& moduleName)
{
	sourcefileName_ = moduleName + moduleSuffix + ".h";
}

//-------------------------------------------------------------------------------------
void ClientSDKUE4::onCreateServerErrorDescrsModuleFileName()
{
	sourcefileName_ = "ServerErrorDescrs.h";

	currpath_ = basepath_ + "Source/KBEnginePlugins/Public/";
}

//-------------------------------------------------------------------------------------
void ClientSDKUE4::onCreateEngineMessagesModuleFileName()
{
	sourcefileName_ = "Messages.h";

	currpath_ = basepath_ + "Source/KBEnginePlugins/Public/";
}

//-------------------------------------------------------------------------------------
void ClientSDKUE4::onCreateDefsCustomTypesModuleFileName()
{
	sourcefileName_ = "CustomDataTypes.h";

	currpath_ = basepath_ + "Source/KBEnginePlugins/Public/";
}

//-------------------------------------------------------------------------------------
void ClientSDKUE4::onCreateEntityDefsModuleFileName()
{
	sourcefileName_ = "EntityDef.h";
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeServerErrorDescrsModuleBegin()
{
	sourcefileBody_ = headerBody;
	strutil::kbe_replace(sourcefileBody_, "#REPLACE#", "");

	sourcefileBody_ += "#pragma once\n\n";
	sourcefileBody_ += "#include \"KBECommon.h\"\n";

	sourcefileBody_ += "\n\n// defined in */res/server/server_errors.xml\n\n";
	sourcefileBody_ += fmt::format("class {}\n{{\npublic:\n", "ServerErrorDescrs");

	sourcefileBody_ += "\tServerErrorDescrs():\n\tserverErrs_()\n\t{\n";
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeServerErrorDescrsModuleErrDescr(int errorID, const std::string& errname, const std::string& errdescr)
{
	sourcefileBody_ += fmt::format("\t\t{{\n\t\t\tFKServerErr e;\n\t\t\te.id = {};\n\t\t\te.name = \"{}\";\n\t\t\te.descr = \"{}\";\n\n\t\t\tserverErrs_.Add(e.id, e);\n", errorID, errname, errdescr);
	sourcefileBody_ += "\t\t}\n\n";
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeServerErrorDescrsModuleEnd()
{
	sourcefileBody_ += "\t}\n\n";

	sourcefileBody_ += "\tvoid Clear()\n\t{\n\t\tserverErrs_.Empty();\n\t}\n\n";

	sourcefileBody_ += "\tFString ServerErrStr(uint16 id)\n\t{\n\t\tFKServerErr e = serverErrs_.FindRef(id);\n\t\treturn FString::Printf(TEXT(\"%s[%s]\"), *e.name, *e.descr);\n\t}\n\n";
	sourcefileBody_ += "\tFKServerErr ServerErr(uint16 id)\n\t{\n\t\treturn serverErrs_.FindRef(id);\n\t}\n\n";

	sourcefileBody_ += "\tprotected:\n\t\tTMap<uint16, FKServerErr> serverErrs_;";
	sourcefileBody_ += "\n};\n\n";
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEngineMessagesModuleBegin()
{
	ERROR_MSG(fmt::format("ClientSDK::writeEngineMessagesModuleBegin: Not Implemented!\n"));
	return false;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEngineMessagesModuleMessage(Network::ExposedMessageInfo& messageInfos, COMPONENT_TYPE componentType)
{
	ERROR_MSG(fmt::format("ClientSDK::writeEngineMessagesModuleMessage: Not Implemented!\n"));
	return false;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEngineMessagesModuleEnd()
{
	ERROR_MSG(fmt::format("ClientSDK::writeEngineMessagesModuleEnd: Not Implemented!\n"));
	return false;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypesBegin()
{
	sourcefileBody_ = headerBody;
	strutil::kbe_replace(sourcefileBody_, "#REPLACE#", "");

	sourcefileBody_ += "#pragma once\n\n";
	sourcefileBody_ += "#include \"KBECommon.h\"\n";

	sourcefileBody_ += "\n\n// defined in */scripts/entity_defs/types.xml\n\n";
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypesEnd()
{
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeBegin(std::string typeName, FixedDictType* pDataType)
{
	initBody_ = "";
	sourcefileBody_ += fmt::format("class {}\n{{\npublic:\n", typeName);
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeEnd(std::string typeName, FixedDictType* pDataType)
{
	if (initBody_.size() > 0)
	{
		initBody_ = std::string(":\n") + initBody_;
		initBody_.erase(initBody_.size() - 2);
	}

	sourcefileBody_ += fmt::format("\n\t{}(){}\n\t{{\n\t}}\n", typeName, initBody_);
	sourcefileBody_ += "\n};\n\n";
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeBegin(std::string typeName, FixedArrayType* pDataType, const std::string& parentClass)
{
	initBody_ = "";
	sourcefileBody_ += fmt::format("class {}{}\n{{\npublic:\n", typeName, (parentClass.size() > 0 ? std::string(" : ") + parentClass : ""));
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeEnd(std::string typeName, FixedArrayType* pDataType)
{
	if (initBody_.size() > 0)
	{
		initBody_ = std::string(":\n") + initBody_;
		initBody_.erase(initBody_.size() - 2);
	}

	sourcefileBody_ += fmt::format("\n\t{}(){}\n\t{{\n\t}}\n", typeName, initBody_);
	sourcefileBody_ += "\n};\n\n";
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeBegin(std::string typeName, DataType* pDataType)
{
	initBody_ = "";
	sourcefileBody_ += fmt::format("class {}\n{{\npublic:\n", typeName);
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeEnd(std::string typeName, DataType* pDataType)
{
	if (initBody_.size() > 0)
	{
		initBody_ = std::string(":\n") + initBody_;
		initBody_.erase(initBody_.size() - 2);
	}

	sourcefileBody_ += fmt::format("\n\t{}(){}\n\t{{\n\t}}\n", typeName, initBody_);
	sourcefileBody_ += "\n};\n\n";
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeItemType_AliasName(const std::string& itemName, const std::string& childItemName)
{
	ERROR_MSG("ClientSDKUE4::writeTypeItemType_AliasName(): nonsupport!\n");
	return false;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeItemType_INT8(const std::string& itemName, const std::string& childItemName)
{
	sourcefileBody_ += fmt::format("\tint8 {};\n", itemName);
	initBody_ += fmt::format("\t{}({}),\n", itemName, 0);
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeItemType_INT16(const std::string& itemName, const std::string& childItemName)
{
	sourcefileBody_ += fmt::format("\tint16 {};\n", itemName);
	initBody_ += fmt::format("\t{}({}),\n", itemName, 0);
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeItemType_INT32(const std::string& itemName, const std::string& childItemName)
{
	sourcefileBody_ += fmt::format("\tint32 {};\n", itemName);
	initBody_ += fmt::format("\t{}({}),\n", itemName, 0);
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeItemType_INT64(const std::string& itemName, const std::string& childItemName)
{
	sourcefileBody_ += fmt::format("\tint64 {};\n", itemName);
	initBody_ += fmt::format("\t{}({}),\n", itemName, 0);
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeItemType_UINT8(const std::string& itemName, const std::string& childItemName)
{
	sourcefileBody_ += fmt::format("\tuint8 {};\n", itemName);
	initBody_ += fmt::format("\t{}({}),\n", itemName, 0);
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeItemType_UINT16(const std::string& itemName, const std::string& childItemName)
{
	sourcefileBody_ += fmt::format("\tuint16 {};\n", itemName);
	initBody_ += fmt::format("\t{}({}),\n", itemName, 0);
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeItemType_UINT32(const std::string& itemName, const std::string& childItemName)
{
	sourcefileBody_ += fmt::format("\tuint32 {};\n", itemName);
	initBody_ += fmt::format("\t{}({}),\n", itemName, 0);
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeItemType_UINT64(const std::string& itemName, const std::string& childItemName)
{
	sourcefileBody_ += fmt::format("\tuint64 {};\n", itemName);
	initBody_ += fmt::format("\t{}({}),\n", itemName, 0);
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeItemType_FLOAT(const std::string& itemName, const std::string& childItemName)
{
	sourcefileBody_ += fmt::format("\tfloat {};\n", itemName);
	initBody_ += fmt::format("\t{}({}),\n", itemName, "0.0f");
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeItemType_DOUBLE(const std::string& itemName, const std::string& childItemName)
{
	sourcefileBody_ += fmt::format("\tdouble {};\n", itemName);
	initBody_ += fmt::format("\t{}({}),\n", itemName, "0.0");
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeItemType_STRING(const std::string& itemName, const std::string& childItemName)
{
	sourcefileBody_ += fmt::format("\tFString {};\n", itemName);
	initBody_ += fmt::format("\t{}({}),\n", itemName, "");
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeItemType_UNICODE(const std::string& itemName, const std::string& childItemName)
{
	sourcefileBody_ += fmt::format("\tFString {};\n", itemName);
	initBody_ += fmt::format("\t{}({}),\n", itemName, "");
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeItemType_PYTHON(const std::string& itemName, const std::string& childItemName)
{
	ERROR_MSG("ClientSDKUE4::writeTypeItemType_PYTHON(): nonsupport!\n");
	return false;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeItemType_PY_DICT(const std::string& itemName, const std::string& childItemName)
{
	ERROR_MSG("ClientSDKUE4::writeTypeItemType_PY_DICT(): nonsupport!\n");
	return false;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeItemType_PY_TUPLE(const std::string& itemName, const std::string& childItemName)
{
	ERROR_MSG("ClientSDKUE4::writeTypeItemType_PY_TUPLE(): nonsupport!\n");
	return false;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeItemType_PY_LIST(const std::string& itemName, const std::string& childItemName)
{
	ERROR_MSG("ClientSDKUE4::writeTypeItemType_PY_LIST(): nonsupport!\n");
	return false;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeItemType_BLOB(const std::string& itemName, const std::string& childItemName)
{
	sourcefileBody_ += fmt::format("\tTArray<uint8> {};\n", itemName);
	initBody_ += fmt::format("\t{}({}),\n", itemName, "");
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeItemType_ARRAY(const std::string& itemName, const std::string& childItemName, DataType* pDataType)
{
	std::string typeStr;
	getArrayType(pDataType, typeStr);
	sourcefileBody_ += fmt::format("\t{} {};\n", typeStr, itemName);
	initBody_ += fmt::format("\t{}({}),\n", itemName, "");
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeItemType_FIXED_DICT(const std::string& itemName, const std::string& childItemName, DataType* pDataType)
{
	sourcefileBody_ += fmt::format("\t{} {};\n", childItemName, itemName, childItemName);
	initBody_ += fmt::format("\t{}({}),\n", itemName, "");
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeItemType_VECTOR2(const std::string& itemName, const std::string& childItemName)
{
#ifdef CLIENT_NO_FLOAT
	sourcefileBody_ += fmt::format("\tFVector2D {};\n", itemName);
#else
	sourcefileBody_ += fmt::format("\tFVector2D {};\n", itemName);
#endif

	initBody_ += fmt::format("\t{}({}),\n", itemName, "");
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeItemType_VECTOR3(const std::string& itemName, const std::string& childItemName)
{
#ifdef CLIENT_NO_FLOAT
	sourcefileBody_ += fmt::format("\tFVector {};\n", itemName);
#else
	sourcefileBody_ += fmt::format("\tFVector {};\n", itemName);
#endif

	initBody_ += fmt::format("\t{}({}),\n", itemName, "");
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeItemType_VECTOR4(const std::string& itemName, const std::string& childItemName)
{
#ifdef CLIENT_NO_FLOAT
	sourcefileBody_ += fmt::format("\tFVector4 {};\n", itemName);
#else
	sourcefileBody_ += fmt::format("\tFVector4 {};\n", itemName);
#endif

	initBody_ += fmt::format("\t{}({}),\n", itemName, "");
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeTypeItemType_MAILBOX(const std::string& itemName, const std::string& childItemName)
{
	sourcefileBody_ += fmt::format("\tTArray<uint8> {};\n", itemName);
	initBody_ += fmt::format("\t{}({}),\n", itemName, "");
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityModuleBegin(ScriptDefModule* pEntityScriptDefModule)
{
	std::string newModuleName = fmt::format("{}{}", pEntityScriptDefModule->getName(), moduleSuffix);

	initBody_ = "\tEntity(),\n";

	sourcefileBody_ = headerBody;
	strutil::kbe_replace(sourcefileBody_, "#REPLACE#", fmt::format("\tPlease inherit this module, such as: (class {} : public {}{})\n", 
		pEntityScriptDefModule->getName(), pEntityScriptDefModule->getName(), moduleSuffix));

	sourcefileBody_ += "#pragma once\n";
	sourcefileBody_ += "#include \"KBETypes.h\"\n";
	sourcefileBody_ += "#include \"ServerErrorDescrs.h\"\n\n";
	sourcefileBody_ += "class Entity;\n";
	
	sourcefileBody_ += std::string("\n// defined in */scripts/entity_defs/") + pEntityScriptDefModule->getName() + ".def\n";

	sourcefileBody_ += fmt::format("class {} : public Entity\n{{\npublic:\n", newModuleName);
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityModuleEnd(ScriptDefModule* pEntityScriptDefModule)
{
	if (initBody_.size() > 0)
	{
		initBody_ = std::string(":\n") + initBody_;
		initBody_.erase(initBody_.size() - 2);
	}

	std::string newModuleName = fmt::format("{}{}", pEntityScriptDefModule->getName(), moduleSuffix);

	sourcefileBody_ += fmt::format("\n\t{}(){}\n\t{{\n\t}}\n", newModuleName, initBody_);
	sourcefileBody_ += fmt::format("\n\tvirtual ~{}()\n\t{{\n\t}}\n\n", newModuleName);
	sourcefileBody_ += "\n};\n\n";
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityProcessMessagesMethod(ScriptDefModule* pEntityScriptDefModule)
{
	sourcefileBody_ += fmt::format("\n\t\tvirtual void onRemoteMethodCall(Method* pMethod, MemoryStream* pMemoryStream)\n\t\t{{\n");
	sourcefileBody_ += fmt::format("\t\t\tswitch(methodUtype)\n\t\t\t{{\n");

	ScriptDefModule::METHODDESCRIPTION_MAP& clientMethods = pEntityScriptDefModule->getClientMethodDescriptions();
	ScriptDefModule::METHODDESCRIPTION_MAP::iterator methodIter = clientMethods.begin();
	for (; methodIter != clientMethods.end(); ++methodIter)
	{
		MethodDescription* pMethodDescription = methodIter->second;


		sourcefileBody_ += fmt::format("\t\t\t\tcase {}:\n", pMethodDescription->getUType());
		sourcefileBody_ += fmt::format("\t\t\t\t\tbreak;\n");

		std::vector<DataType*>& argTypes = pMethodDescription->getArgTypes();
		std::vector<DataType*>::iterator iter = argTypes.begin();

		//int i = 1;

		for (; iter != argTypes.end(); ++iter)
		{
		//	DataType* pDataType = (*iter);
		}
	}

	sourcefileBody_ += fmt::format("\t\t\t\tdefault:\n");
	sourcefileBody_ += fmt::format("\t\t\t\t\tbreak;\n");
	sourcefileBody_ += fmt::format("\t\t\t}};\n");
	sourcefileBody_ += "\t\t}\n";
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityProperty_INT8(ScriptDefModule* pEntityScriptDefModule,
	ScriptDefModule* pCurrScriptDefModule, PropertyDescription* pPropertyDescription)
{
	sourcefileBody_ += fmt::format("\tint8 {};\n", pPropertyDescription->getName());
	initBody_ += fmt::format("\t{}({}),\n", pPropertyDescription->getName(), 0);
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityProperty_INT16(ScriptDefModule* pEntityScriptDefModule,
	ScriptDefModule* pCurrScriptDefModule, PropertyDescription* pPropertyDescription)
{
	sourcefileBody_ += fmt::format("\tint16 {};\n", pPropertyDescription->getName());
	initBody_ += fmt::format("\t{}({}),\n", pPropertyDescription->getName(), 0);
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityProperty_INT32(ScriptDefModule* pEntityScriptDefModule,
	ScriptDefModule* pCurrScriptDefModule, PropertyDescription* pPropertyDescription)
{
	sourcefileBody_ += fmt::format("\tint32 {};\n", pPropertyDescription->getName());
	initBody_ += fmt::format("\t{}({}),\n", pPropertyDescription->getName(), 0);
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityProperty_INT64(ScriptDefModule* pEntityScriptDefModule,
	ScriptDefModule* pCurrScriptDefModule, PropertyDescription* pPropertyDescription)
{
	sourcefileBody_ += fmt::format("\tint64 {};\n", pPropertyDescription->getName());
	initBody_ += fmt::format("\t{}({}),\n", pPropertyDescription->getName(), 0);
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityProperty_UINT8(ScriptDefModule* pEntityScriptDefModule,
	ScriptDefModule* pCurrScriptDefModule, PropertyDescription* pPropertyDescription)
{
	sourcefileBody_ += fmt::format("\tuint8 {};\n", pPropertyDescription->getName());
	initBody_ += fmt::format("\t{}({}),\n", pPropertyDescription->getName(), 0);
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityProperty_UINT16(ScriptDefModule* pEntityScriptDefModule,
	ScriptDefModule* pCurrScriptDefModule, PropertyDescription* pPropertyDescription)
{
	sourcefileBody_ += fmt::format("\tuint16 {};\n", pPropertyDescription->getName());
	initBody_ += fmt::format("\t{}({}),\n", pPropertyDescription->getName(), 0);
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityProperty_UINT32(ScriptDefModule* pEntityScriptDefModule,
	ScriptDefModule* pCurrScriptDefModule, PropertyDescription* pPropertyDescription)
{
	sourcefileBody_ += fmt::format("\tuint32 {};\n", pPropertyDescription->getName());
	initBody_ += fmt::format("\t{}({}),\n", pPropertyDescription->getName(), 0);
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityProperty_UINT64(ScriptDefModule* pEntityScriptDefModule,
	ScriptDefModule* pCurrScriptDefModule, PropertyDescription* pPropertyDescription)
{
	sourcefileBody_ += fmt::format("\tuint64 {};\n", pPropertyDescription->getName());
	initBody_ += fmt::format("\t{}({}),\n", pPropertyDescription->getName(), 0);
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityProperty_FLOAT(ScriptDefModule* pEntityScriptDefModule,
	ScriptDefModule* pCurrScriptDefModule, PropertyDescription* pPropertyDescription)
{
	sourcefileBody_ += fmt::format("\tfloat {};\n", pPropertyDescription->getName());
	initBody_ += fmt::format("\t{}({}),\n", pPropertyDescription->getName(), "0.0f");
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityProperty_DOUBLE(ScriptDefModule* pEntityScriptDefModule,
	ScriptDefModule* pCurrScriptDefModule, PropertyDescription* pPropertyDescription)
{
	sourcefileBody_ += fmt::format("\tdouble {};\n", pPropertyDescription->getName());
	initBody_ += fmt::format("\t{}({}),\n", pPropertyDescription->getName(), "0.0");
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityProperty_STRING(ScriptDefModule* pEntityScriptDefModule,
	ScriptDefModule* pCurrScriptDefModule, PropertyDescription* pPropertyDescription)
{
	sourcefileBody_ += fmt::format("\tFString {};\n", pPropertyDescription->getName());
	initBody_ += fmt::format("\t{}({}),\n", pPropertyDescription->getName(), "");
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityProperty_UNICODE(ScriptDefModule* pEntityScriptDefModule,
	ScriptDefModule* pCurrScriptDefModule, PropertyDescription* pPropertyDescription)
{
	sourcefileBody_ += fmt::format("\tFString {};\n", pPropertyDescription->getName());
	initBody_ += fmt::format("\t{}({}),\n", pPropertyDescription->getName(), "");
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityProperty_PYTHON(ScriptDefModule* pEntityScriptDefModule,
	ScriptDefModule* pCurrScriptDefModule, PropertyDescription* pPropertyDescription)
{
	ERROR_MSG("ClientSDKUE4::writeEntityProperty_PYTHON(): nonsupport!\n");
	return false;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityProperty_PY_DICT(ScriptDefModule* pEntityScriptDefModule,
	ScriptDefModule* pCurrScriptDefModule, PropertyDescription* pPropertyDescription)
{
	ERROR_MSG("ClientSDKUE4::writeEntityProperty_PY_DICT(): nonsupport!\n");
	return false;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityProperty_PY_TUPLE(ScriptDefModule* pEntityScriptDefModule,
	ScriptDefModule* pCurrScriptDefModule, PropertyDescription* pPropertyDescription)
{
	ERROR_MSG("ClientSDKUE4::writeEntityProperty_PY_TUPLE(): nonsupport!\n");
	return false;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityProperty_PY_LIST(ScriptDefModule* pEntityScriptDefModule,
	ScriptDefModule* pCurrScriptDefModule, PropertyDescription* pPropertyDescription)
{
	ERROR_MSG("ClientSDKUE4::writeEntityProperty_PY_LIST(): nonsupport!\n");
	return false;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityProperty_BLOB(ScriptDefModule* pEntityScriptDefModule,
	ScriptDefModule* pCurrScriptDefModule, PropertyDescription* pPropertyDescription)
{
	sourcefileBody_ += fmt::format("\tTArray<uint8> {};\n", pPropertyDescription->getName());
	initBody_ += fmt::format("\t{}({}),\n", pPropertyDescription->getName(), "");
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityProperty_ARRAY(ScriptDefModule* pEntityScriptDefModule,
	ScriptDefModule* pCurrScriptDefModule, PropertyDescription* pPropertyDescription)
{
	if (std::string("ARRAY") == pPropertyDescription->getDataTypeName())
	{
		return writeTypeItemType_ARRAY(pPropertyDescription->getName(), pPropertyDescription->getDataType()->aliasName(), pPropertyDescription->getDataType());
	}
	else
	{
		sourcefileBody_ += fmt::format("\t{} {};\n", pPropertyDescription->getDataTypeName(), pPropertyDescription->getName());
		initBody_ += fmt::format("\t{}({}),\n", pPropertyDescription->getName(), "");
	} 

	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityProperty_FIXED_DICT(ScriptDefModule* pEntityScriptDefModule,
	ScriptDefModule* pCurrScriptDefModule, PropertyDescription* pPropertyDescription)
{
	sourcefileBody_ += fmt::format("\t{} {};\n", pPropertyDescription->getDataTypeName(), pPropertyDescription->getName());
	initBody_ += fmt::format("\t{}({}),\n", pPropertyDescription->getName(), "");
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityProperty_VECTOR2(ScriptDefModule* pEntityScriptDefModule,
	ScriptDefModule* pCurrScriptDefModule, PropertyDescription* pPropertyDescription)
{
#ifdef CLIENT_NO_FLOAT
	sourcefileBody_ += fmt::format("\t\tFVector2D {} = {};\n", pPropertyDescription->getName());
#else
	sourcefileBody_ += fmt::format("\t\tFVector2D {} = {};\n", pPropertyDescription->getName());
#endif

	initBody_ += fmt::format("\t{}({}),\n", pPropertyDescription->getName(), "");
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityProperty_VECTOR3(ScriptDefModule* pEntityScriptDefModule,
	ScriptDefModule* pCurrScriptDefModule, PropertyDescription* pPropertyDescription)
{
#ifdef CLIENT_NO_FLOAT
	sourcefileBody_ += fmt::format("\tFVector {};\n", pPropertyDescription->getName());
#else
	sourcefileBody_ += fmt::format("\tFVector {};\n", pPropertyDescription->getName());
#endif

	initBody_ += fmt::format("\t{}({}),\n", pPropertyDescription->getName(), "");
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityProperty_VECTOR4(ScriptDefModule* pEntityScriptDefModule,
	ScriptDefModule* pCurrScriptDefModule, PropertyDescription* pPropertyDescription)
{
#ifdef CLIENT_NO_FLOAT
	sourcefileBody_ += fmt::format("\tFVector4 {};\n", pPropertyDescription->getName());
#else
	sourcefileBody_ += fmt::format("\tFVector4 {};\n", pPropertyDescription->getName());
#endif

	initBody_ += fmt::format("\t{}({}),\n", pPropertyDescription->getName(), "");
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityProperty_MAILBOX(ScriptDefModule* pEntityScriptDefModule,
	ScriptDefModule* pCurrScriptDefModule, PropertyDescription* pPropertyDescription)
{
	sourcefileBody_ += fmt::format("\tTArray<uint8> {};\n", pPropertyDescription->getName());
	initBody_ += fmt::format("\t{}({}),\n", pPropertyDescription->getName(), "");
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityMethod(ScriptDefModule* pEntityScriptDefModule,
	ScriptDefModule* pCurrScriptDefModule, MethodDescription* pMethodDescription, const char* fillString)
{
	sourcefileBody_ += fmt::format("\tvirtual void {}({}) = 0; \n", pMethodDescription->getName(), fillString);
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityMethodArgs_ARRAY(FixedArrayType* pFixedArrayType, std::string& stackArgsTypeBody, const std::string& childItemName)
{
	std::string typeStr;
	getArrayType(pFixedArrayType, typeStr);
	stackArgsTypeBody += fmt::format("const {}&", typeStr);
	return true;
}

//-------------------------------------------------------------------------------------
bool ClientSDKUE4::writeEntityMethodArgs_Const_Ref(DataType* pDataType, std::string& stackArgsTypeBody)
{
	stackArgsTypeBody = std::string("const ") + stackArgsTypeBody + "&";
	return true;
}

//-------------------------------------------------------------------------------------
}
