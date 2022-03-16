#pragma once

#include <unordered_map>
#include <functional>
#include <iostream>

/*

	1. ClassCreator -> 生成对象的函数指针

	2. ClassFactory单例设计模式，其中使用哈希表保存<ClassName, ClassCreator>
		并提供Regist，GetClassObjectByName
	
	3. Register类，主要调用ClassFactory中的Regist函数

	利用宏简化定义，实现创建类的函数，绑定操作
		DEFINE_DYN_CREATE
		IMPLEMENT_DYN_CREATE
		REGIST_CLASS

*/

using ClassCreator = std::function<void* ()>;

#define DEFINE_DYN_CREATE(ClassName)						\
	ClassName* ClassName##Creator();						\

#define IMPLEMENT_DYN_CREATE(ClassName)						\
	ClassName* ClassName##Creator() {						\
		return new ClassName;								\
	}														\

#define REGIST_CLASS(ClassName)								\
	ClassCreator classCreator = ClassName##Creator;			\
	Register registor(#ClassName, classCreator);			\

class ClassFactory {

private:
	std::unordered_map <std::string, ClassCreator> NameToClass;

private:
	ClassFactory() {};
	ClassFactory(const ClassFactory& Source) = delete;
	ClassFactory& operator=(const ClassFactory& Source) = delete;
	ClassFactory(ClassFactory&& Source) = delete;
	ClassFactory& operator=(ClassFactory&& Source) = delete;

public:
	static ClassFactory& GetInstance();
	void Regist(std::string ClassName, ClassCreator classCreator);
	void* GetClassObjectByName(std::string ClassName);
};

ClassFactory& ClassFactory::GetInstance() {
	static ClassFactory classFactory;
	return classFactory;
}

void ClassFactory::Regist(std::string ClassName, ClassCreator classCreator) {
	NameToClass.emplace(ClassName, classCreator);
}

void* ClassFactory::GetClassObjectByName(std::string ClassName) {
	if (NameToClass.count(ClassName))
		return NameToClass[ClassName]();
	else
		return nullptr;
}

class Register {

public:
	Register(std::string ClassName, ClassCreator classCreator) {
		ClassFactory::GetInstance().Regist(ClassName, classCreator);
	}
};