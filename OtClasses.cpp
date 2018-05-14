//	ObjectTalk Scripting Language
//	Copyright 1993-2018 Johan A. Goossens
//
//	Licensed under the Apache License, Version 2.0 (the "License");
//	you may not use this file except in compliance with the License.
//	You may obtain a copy of the License at
//
//		http://www.apache.org/licenses/LICENSE-2.0
//
//	Unless required by applicable law or agreed to in writing, software
//	distributed under the License is distributed on an "AS IS" BASIS,
//	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	See the License for the specific language governing permissions and
//	limitations under the License.


//
//  Include files
//

#include <string>

#include "OtClasses.h"
#include "OtNucleus.h"


//
//  Type factory
//

static OtType OtObjectType()
{
    static OtType type = nullptr;
    
    if (!type)
    {
        type = OtTypeCreate<OtObject>("Object");
        
        type->set("__member__", OtFunctionCreate(
        std::function<OtValue(OtValue, std::string)>([] (OtValue o, std::string n)->OtValue
        {
            return OtMemberReferenceCreate(o, n);
        })));
        
        type->set("boolean", OtFunctionCreate(&OtObject::operator bool));
        type->set("integer", OtFunctionCreate(&OtObject::operator long));
        type->set("real", OtFunctionCreate(&OtObject::operator double));
        type->set("string", OtFunctionCreate(&OtObject::operator std::string));

        type->set("has", OtFunctionCreate(&OtObject::has));
        type->set("get", OtFunctionCreate(&OtObject::get));
        type->set("set", OtFunctionCreate(&OtObject::set));

        type->set("getClass", OtFunctionCreate(
        std::function<OtValue(OtValue)>([] (OtValue o)->OtValue
        {
            return OtValueCreate(o->getType());
        })));

        type->set("isKindOf", OtFunctionCreate(
        std::function<bool(OtValue, const std::string&)>([] (OtValue o, const std::string& n)->bool
        {
            return o->isKindOf(n);
        })));
    }

    return type;
}

static OtType OtInternalType()
{
    static OtType type = nullptr;
    
    if (!type)
    {
        type = OtTypeCreate<OtInternal>("Internal", OtObjectType());
    }
    
    return type;
}

static OtType OtClassType()
{
    static OtType type = nullptr;
    
    if (!type)
    {
        type = OtTypeCreate<OtClass>("Class", OtInternalType());

        type->set("__call__", OtFunctionCreateRaw(&OtClass::call));

        type->set("getName", OtFunctionCreate(&OtClass::getName));
        type->set("hasParent", OtFunctionCreate(&OtClass::hasParent));
        type->set("getParent", OtFunctionCreate(&OtClass::getParent));
        type->set("subClass", OtFunctionCreate(&OtClass::subClass));
        type->set("isKindOf", OtFunctionCreate(&OtClass::isKindOf));
    }
    
    return type;
}

static OtType OtContextReferenceType()
{
    static OtType type = nullptr;
    
    if (!type)
    {
        type = OtTypeCreate<OtContextReference>("ContextReference", OtInternalType());
        
        type->set("__deref__", OtFunctionCreateRaw(&OtContextReference::deref));
        type->set("__assign__", OtFunctionCreateRaw(&OtContextReference::assign));
    }
    
    return type;
}

static OtType OtMemberReferenceType()
{
    static OtType type = nullptr;
    
    if (!type)
    {
        type = OtTypeCreate<OtMemberReference>("MemberReference", OtInternalType());
        
        type->set("__deref__", OtFunctionCreate(&OtMemberReference::deref));
        type->set("__assign__", OtFunctionCreate(&OtMemberReference::assign));
    }
    
    return type;
}

static OtType OtArrayReferenceType()
{
    static OtType type = nullptr;
    
    if (!type)
    {
        type = OtTypeCreate<OtArrayReference>("ArrayReference", OtInternalType());

        type->set("__deref__", OtFunctionCreate(&OtArrayReference::deref));
        type->set("__assign__", OtFunctionCreate(&OtArrayReference::assign));
    }
    
    return type;
}

static OtType OtDictReferenceType()
{
    static OtType type = nullptr;
    
    if (!type)
    {
        type = OtTypeCreate<OtDictReference>("DictReference", OtInternalType());

        type->set("__deref__", OtFunctionCreate(&OtDictReference::deref));
        type->set("__assign__", OtFunctionCreate(&OtDictReference::assign));
    }
    
    return type;
}

static OtType OtPrimitiveType()
{
    static OtType type = nullptr;
    
    if (!type)
    {
        type = OtTypeCreate<OtPrimitive>("Primitive", OtObjectType());
    }
    
    return type;
}

static OtType OtBooleanType()
{
    static OtType type = nullptr;
    
    if (!type)
    {
        type = OtTypeCreate<OtBoolean>("Boolean", OtPrimitiveType());
        
        type->set("__and__", OtFunctionCreate(&OtBoolean::logicalAnd));
        type->set("__or__", OtFunctionCreate(&OtBoolean::logicalOr));
        type->set("__not__", OtFunctionCreate(&OtBoolean::logicalNot));

        type->set("__eq__", OtFunctionCreate(&OtBoolean::equal));
        type->set("__ne__", OtFunctionCreate(&OtBoolean::notEqual));
    }
    
    return type;
}

static OtType OtIntegerType()
{
    static OtType type = nullptr;
    
    if (!type)
    {
        type = OtTypeCreate<OtInteger>("Integer", OtPrimitiveType());

        type->set("__add__", OtFunctionCreate(&OtInteger::add));
        type->set("__sub__", OtFunctionCreate(&OtInteger::subtract));
        type->set("__mul__", OtFunctionCreate(&OtInteger::multiply));
        type->set("__div__", OtFunctionCreate(&OtInteger::divide));
        type->set("__mod__", OtFunctionCreate(&OtInteger::modulo));
        type->set("__pow__", OtFunctionCreate(&OtInteger::power));

        type->set("__inc__", OtFunctionCreate(&OtInteger::increment));
        type->set("__dec__", OtFunctionCreate(&OtInteger::decrement));

        type->set("__lshift__", OtFunctionCreate(&OtInteger::shiftLeft));
        type->set("__rshift__", OtFunctionCreate(&OtInteger::shiftRight));
        
        type->set("__bend__", OtFunctionCreate(&OtInteger::bitwiseAnd));
        type->set("__bor__", OtFunctionCreate(&OtInteger::bitwiseOr));
        type->set("__bxor__", OtFunctionCreate(&OtInteger::bitwiseXor));
        type->set("__bnot__", OtFunctionCreate(&OtInteger::bitwiseNot));

        type->set("__eq__", OtFunctionCreate(&OtInteger::equal));
        type->set("__ne__", OtFunctionCreate(&OtInteger::notEqual));
        type->set("__gt__", OtFunctionCreate(&OtInteger::greaterThan));
        type->set("__lt__", OtFunctionCreate(&OtInteger::lessThan));
        type->set("__ge__", OtFunctionCreate(&OtInteger::greaterEqual));
        type->set("__le__", OtFunctionCreate(&OtInteger::lessEqual));

        type->set("__neg__", OtFunctionCreate(&OtInteger::negate));
        
        type->set("abs", OtFunctionCreate(&OtInteger::abs));
        type->set("sign", OtFunctionCreate(&OtInteger::sign));
    }
    
    return type;
}

static OtType OtRealType()
{
    static OtType type = nullptr;
    
    if (!type)
    {
        type = OtTypeCreate<OtReal>("Real", OtPrimitiveType());

        type->set("__add__", OtFunctionCreate(&OtReal::add));
        type->set("__sub__", OtFunctionCreate(&OtReal::subtract));
        type->set("__mul__", OtFunctionCreate(&OtReal::multiply));
        type->set("__div__", OtFunctionCreate(&OtReal::divide));
        type->set("__pow__", OtFunctionCreate(&OtReal::power));

        type->set("__eq__", OtFunctionCreate(&OtReal::equal));
        type->set("__ne__", OtFunctionCreate(&OtReal::notEqual));
        type->set("__gt__", OtFunctionCreate(&OtReal::greaterThan));
        type->set("__lt__", OtFunctionCreate(&OtReal::lessThan));
        type->set("__ge__", OtFunctionCreate(&OtReal::greaterEqual));
        type->set("__le__", OtFunctionCreate(&OtReal::lessEqual));
        
        type->set("__neg__", OtFunctionCreate(&OtReal::negate));
        
        type->set("abs", OtFunctionCreate(&OtReal::abs));
        type->set("sign", OtFunctionCreate(&OtReal::sign));
        type->set("round", OtFunctionCreate(&OtReal::round));
        type->set("ceil", OtFunctionCreate(&OtReal::ceil));
        type->set("floor", OtFunctionCreate(&OtReal::floor));
        type->set("sin", OtFunctionCreate(&OtReal::sin));
        type->set("cos", OtFunctionCreate(&OtReal::cos));
        type->set("tan", OtFunctionCreate(&OtReal::tan));
        type->set("radians", OtFunctionCreate(&OtReal::radians));
        type->set("degrees", OtFunctionCreate(&OtReal::degrees));
    }
    
    return type;
}

static OtType OtStringType()
{
    static OtType type = nullptr;
    
    if (!type)
    {
        type = OtTypeCreate<OtString>("String", OtPrimitiveType());

        type->set("__add__", OtFunctionCreate(&OtString::add));

        type->set("__eq__", OtFunctionCreate(&OtString::equal));
        type->set("__ne__", OtFunctionCreate(&OtString::notEqual));
        type->set("__gt__", OtFunctionCreate(&OtString::greaterThan));
        type->set("__lt__", OtFunctionCreate(&OtString::lessThan));
        type->set("__ge__", OtFunctionCreate(&OtString::greaterEqual));
        type->set("__le__", OtFunctionCreate(&OtString::lessEqual));
        
        type->set("len", OtFunctionCreate(&OtString::len));

        type->set("left", OtFunctionCreate(&OtString::left));
        type->set("right", OtFunctionCreate(&OtString::right));
        type->set("mid", OtFunctionCreate(&OtString::mid));

        type->set("find", OtFunctionCreate(&OtString::find));

        type->set("trim", OtFunctionCreate(&OtString::trim));
        type->set("ltrim", OtFunctionCreate(&OtString::ltrim));
        type->set("rtrim", OtFunctionCreate(&OtString::rtrim));
        type->set("compress", OtFunctionCreate(&OtString::compress));
        type->set("lower", OtFunctionCreate(&OtString::lower));
        type->set("upper", OtFunctionCreate(&OtString::upper));
    }
    
    return type;
}

static OtType OtFunctionType()
{
    static OtType type = nullptr;
    
    if (!type)
    {
        type = OtTypeCreate<OtFunction>("Function", OtPrimitiveType());

        type->set("__call__", OtFunctionCreateRaw(&OtFunction::execute));
    }
    
    return type;
}

static OtType OtBoundFunctionType()
{
    static OtType type = nullptr;
    
    if (!type)
    {
        type = OtTypeCreate<OtBoundFunction>("BoundFunction", OtPrimitiveType());

        type->set("__call__", OtFunctionCreateRaw(&OtBoundFunction::execute));
    }
    
    return type;
}

static OtType OtCollectionType()
{
    static OtType type = nullptr;
    
    if (!type)
    {
        type = OtTypeCreate<OtCollection>("Collection", OtObjectType());
    }
    
    return type;
}

static OtType OtArrayType()
{
    static OtType type = nullptr;
    
    if (!type)
    {
        type = OtTypeCreate<OtArray>("Array", OtCollectionType());

        type->set("__init__", OtFunctionCreateRaw(&OtArray::init));
        type->set("__index__", OtFunctionCreate(&OtArray::index));
        type->set("__add__", OtFunctionCreate(&OtArray::add));

        type->set("size", OtFunctionCreate(&OtArray::mySize));

        type->set("clone", OtFunctionCreate(&OtArray::clone));
        type->set("clear", OtFunctionCreate(&OtArray::clear));

        type->set("append", OtFunctionCreate(&OtArray::append));
        type->set("insert", OtFunctionCreate(&OtArray::insert));

        type->set("erase", OtFunctionCreate(&OtArray::erase));
        type->set("eraseMultiple", OtFunctionCreate(&OtArray::eraseMultiple));
        
        type->set("push", OtFunctionCreate(&OtArray::push));
        type->set("pop", OtFunctionCreate(&OtArray::pop));
    }
    
    return type;
}

static OtType OtDictType()
{
    static OtType type = nullptr;
    
    if (!type)
    {
        type = OtTypeCreate<OtDict>("Dict", OtCollectionType());

        type->set("__init__", OtFunctionCreateRaw(&OtDict::init));
        type->set("__index__", OtFunctionCreate(&OtDict::index));

        type->set("size", OtFunctionCreate(&OtDict::mySize));

        type->set("clone", OtFunctionCreate(&OtDict::clone));
        type->set("clear", OtFunctionCreate(&OtDict::clear));
        type->set("erase", OtFunctionCreate(&OtDict::eraseEntry));

        type->set("keys", OtFunctionCreate(&OtDict::keys));
        type->set("values", OtFunctionCreate(&OtDict::values));

    }
    
    return type;
}


//
//  Create a new value
//

inline OtValue CreateValue(OtType type, OtValue object)
{
    if (object)
        object->setType(type);
    
    else
        object = type->instantiate();
    
    return object;
}

//
//  Class factory
//

OtValue OtObjectClassCreate()
{
    return CreateValue(OtClassType(), std::make_shared<OtClass>(OtObjectType()));
}

OtValue OtInternalClassCreate()
{
    return CreateValue(OtClassType(), std::make_shared<OtClass>(OtInternalType()));
}

OtValue OtClassClassCreate()
{
    return CreateValue(OtClassType(), std::make_shared<OtClass>(OtClassType()));
}

OtValue OtMemberReferenceClassCreate()
{
    return CreateValue(OtClassType(), std::make_shared<OtClass>(OtMemberReferenceType()));
}

OtValue OtArrayReferenceClassCreate()
{
    return CreateValue(OtClassType(), std::make_shared<OtClass>(OtArrayReferenceType()));
}

OtValue OtDictReferenceClassCreate()
{
    return CreateValue(OtClassType(), std::make_shared<OtClass>(OtDictReferenceType()));
}

OtValue OtPrimitiveClassCreate()
{
    return CreateValue(OtClassType(), std::make_shared<OtClass>(OtPrimitiveType()));
}

OtValue OtBooleanClassCreate()
{
    return CreateValue(OtClassType(), std::make_shared<OtClass>(OtBooleanType()));
}

OtValue OtIntegerClassCreate()
{
    return CreateValue(OtClassType(), std::make_shared<OtClass>(OtIntegerType()));
}

OtValue OtRealClassCreate()
{
    return CreateValue(OtClassType(), std::make_shared<OtClass>(OtRealType()));
}

OtValue OtStringClassCreate()
{
    return CreateValue(OtClassType(), std::make_shared<OtClass>(OtStringType()));
}

OtValue OtFunctionClassCreate()
{
    return CreateValue(OtClassType(), std::make_shared<OtClass>(OtFunctionType()));
}

OtValue OtBoundFunctionClassCreate()
{
    return CreateValue(OtClassType(), std::make_shared<OtClass>(OtBoundFunctionType()));
}

OtValue OtCollectionClassCreate()
{
    return CreateValue(OtClassType(), std::make_shared<OtClass>(OtCollectionType()));
}

OtValue OtArrayClassCreate()
{
    return CreateValue(OtClassType(), std::make_shared<OtClass>(OtArrayType()));
}

OtValue OtDictClassCreate()
{
    return CreateValue(OtClassType(), std::make_shared<OtClass>(OtDictType()));
}


//
//  Value factory
//

OtValue OtValueCreate()
{
    return CreateValue(OtObjectType(), nullptr);

}

OtValue OtValueCreate(bool boolean)
{
    return CreateValue(OtBooleanType(), std::make_shared<OtBoolean>(boolean));
}

OtValue OtValueCreate(long integer)
{
    return CreateValue(OtIntegerType(), std::make_shared<OtInteger>(integer));
}

OtValue OtValueCreate(size_t integer)
{
    return CreateValue(OtIntegerType(), std::make_shared<OtInteger>((long) integer));
}

OtValue OtValueCreate(double real)
{
    return CreateValue(OtRealType(), std::make_shared<OtReal>(real));
}

OtValue OtValueCreate(const std::string& string)
{
    return CreateValue(OtStringType(), std::make_shared<OtString>(string));
}

OtValue OtValueCreate(OtType type)
{
    return CreateValue(OtClassType(), std::make_shared<OtClass>(type));
}

OtValue OtValueCreate(size_t count, OtExecutable executable)
{
    return CreateValue(OtFunctionType(), std::make_shared<OtFunction>(count, executable));
}

OtValue OtValueCreate(size_t count, OtCode code, const std::vector<std::string>& names)
{
    return CreateValue(OtFunctionType(), std::make_shared<OtFunction>(count, code, names));
}

OtValue OtBoundFunctionCreate(OtValue object, OtValue function)
{
    return CreateValue(OtBoundFunctionType(), std::make_shared<OtBoundFunction>(object, function));
}

OtValue OtArrayCreate()
{
    return CreateValue(OtArrayType(), nullptr);
}

OtValue OtArrayCreate(size_t count, OtValue* values)
{
    std::shared_ptr<OtArray> array = std::make_shared<OtArray>();
    array->init(count, values);
    return CreateValue(OtArrayType(), array);
}

OtValue OtDictCreate()
{
    return CreateValue(OtDictType(), nullptr);
}

OtValue OtDictCreate(size_t count, OtValue* values)
{
    std::shared_ptr<OtDict> dict = std::make_shared<OtDict>();
    dict->init(count, values);
    return CreateValue(OtDictType(), dict);
}

OtValue OtContextReferenceCreate(const std::string& member)
{
    return CreateValue(OtContextReferenceType(), std::make_shared<OtContextReference>(member));
}

OtValue OtMemberReferenceCreate(OtValue object, const std::string& member)
{
    return CreateValue(OtMemberReferenceType(), std::make_shared<OtMemberReference>(object, member));
}

OtValue OtArrayReferenceCreate(OtValue array, size_t index)
{
    return CreateValue(OtArrayReferenceType(), std::make_shared<OtArrayReference>(OtTypeCast<OtArray>(array), index));
}

OtValue OtDictReferenceCreate(OtValue dict, const std::string& index)
{
    return CreateValue(OtDictReferenceType(), std::make_shared<OtDictReference>(OtTypeCast<OtDict>(dict), index));
}
