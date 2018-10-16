
/*
* Adapter层
* @date : 2018/09/24
* @author : jihang
*/

#include <iostream>

#include "HLAPortInterface.h"

#define READY_TO_RUN "ReadyToRun"

using namespace std;

class HLAPortAdapter : public CHLAPortInterface {

public:
	//成员信息
	string fedFile;
	string federationName;
	string federateName;
	long federateId;

public:
	//1.对象类定义

	//预设的，仿真控制，目前共三组
	ObjectNames objectNamesPause;
	ObjectHandles *objectHandlesPause;

	ObjectNames objectNamesContinue;
	ObjectHandles *objectHandlesContinue;

	ObjectNames objectNamesEnd;
	ObjectHandles *objectHandlesEnd;

	//所有的，用户添加，两个一组
	ObjectNames objectNamesB;
	ObjectHandles *objectHandlesB;

	//自己的，用户添加,发布的
	RTI::ObjectHandle handleB;

public:
	//2.构造函数
	HLAPortAdapter() {
		//预设的，仿真控制
		objectNamesPause.className = "ObjectRoot.PAUSE";
		objectNamesPause.attributeNumber = 1;
		objectNamesPause.attributeList.push_back("pause");

		objectNamesContinue.className = "ObjectRoot.CONTINUE";
		objectNamesContinue.attributeNumber = 1;
		objectNamesContinue.attributeList.push_back("continue");

		objectNamesEnd.className = "ObjectRoot.End";
		objectNamesEnd.attributeNumber = 1;
		objectNamesEnd.attributeList.push_back("end");

		//预设的，成员属性
		fedFile = "testfom.fed";
		federationName = "ExampleFederation";
		federateName = "fedB";
		isPause = false;
		isEnd = false;

		//自己的，用户添加
		objectNamesB.className = "ObjectRoot.B";
		objectNamesB.attributeNumber = 3;
		objectNamesB.attributeList.push_back("ba");
		objectNamesB.attributeList.push_back("bb");
		objectNamesB.attributeList.push_back("bc");
	};

	virtual ~HLAPortAdapter() {};

public:
	//3.用户方法
	void initial() {
		//预设的，仿真控制
		objectHandlesPause = initOneHandle(objectNamesPause);
		objectHandlesContinue = initOneHandle(objectNamesContinue);
		objectHandlesEnd = initOneHandle(objectNamesEnd);

		//自己的，用户添加
		objectHandlesB = initOneHandle(objectNamesB);
	}

	void pubAndSub() {
		//预设的，仿真控制，全为订阅
		subscribeOne(objectHandlesPause);
		subscribeOne(objectHandlesContinue);
		subscribeOne(objectHandlesEnd);

		//自己的，用户添加
		publishOne(objectHandlesB);
	}

	void rigister() {
		//自己的，用户添加
		handleB = registerObject(objectNamesB.className);
		cout << "Registered Object, handle = " << handleB << endl;
	}

	void processData(RTI::ObjectHandle theObject,
		const RTI::AttributeHandleValuePairSet& theAttributes, const char* theTag)
		throw(RTI::ObjectNotKnown, RTI::AttributeNotKnown, RTI::FederateOwnsAttributes,
			RTI::FederateInternalError) {

		//预设的，收到仿真控制信息
		//先由对象句柄获取对象类句柄
		int classHandle = getObjectClass(theObject);
		//根据对象类句柄判断
		if (classHandle == objectHandlesPause->classHandle) {
			cout << "i should pause" << endl;
			isPause = true;
		}
		if (classHandle == objectHandlesContinue->classHandle) {
			cout << "i should continue" << endl;
			isPause = false;
		}
		if (classHandle == objectHandlesEnd->classHandle) {
			cout << "i should end" << endl;
			isEnd = true;
		}
		//自己的，用户添加，收到数据信息
	}

	void simulate() {
		while (true) {
			if (isEnd) {
				del();
				resignFederation();
				//由运行管理器销毁
				//destroyFederation(federationName);
				break;
			}
			send(handleB);
			advanceTime(1.0);
		}
	}

	void send(RTI::ObjectHandle handle) {
		//自己的，用户添加，发送数据信息
		RTI::AttributeHandleValuePairSet *attributes = RTI::AttributeSetFactory::create(3);
		char aaValue[16], abValue[16], acValue[16];
		sprintf_s(aaValue, "haha");
		sprintf_s(abValue, "hehe");
		sprintf_s(acValue, "hihi");
		attributes->add(objectHandlesB->handleList[0], aaValue, (RTI::ULong)strlen(aaValue) + 1);
		attributes->add(objectHandlesB->handleList[1], abValue, (RTI::ULong)strlen(abValue) + 1);
		attributes->add(objectHandlesB->handleList[2], acValue, (RTI::ULong)strlen(acValue) + 1);
		sendObject(handle, attributes);
	}

	void del() {
		deleteOne(handleB);
	}
};