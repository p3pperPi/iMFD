#include "iMFD.h"

//#pragma GCC optimization_level 2

// 各種定義

iMFD_Class iMFD;
static int iMFD_Sensor::snsNum = 0;

#ifdef USBCON // Leonardo等 USB通信機能があるボード
	const HardwareSerial* SerialPtr = &Serial1;
	const Serial_* DebugSerialPtr = &Serial;
#else //Uno Mega等USB通信機能がないボード
	const HardwareSerial* SerialPtr = &Serial1;
	const HardwareSerial* DebugSerialPtr = &Serial;
#endif

iMFD_Sensor extSns[SENSOR_MAX];

//-------------------------------------------------------------------------------------------------
// Serial Reader (internal) -----------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
// constructer & destructor
#define READ_BUFFER_LENGTH 256

byte rBuff[READ_BUFFER_LENGTH][2];
int  rAddr = 0;
bool hasNewData = false;
bool buffNo = 0;
bool correctForm = false;

void UpdateRcvFrame(){
	while(SerialPtr->available()){
		byte rb = SerialPtr->read();
		if(rb == IMFD_START_BYTE){
			rAddr = 0;
			correctForm = true;
			rBuff[rAddr++][buffNo] = rb;
		}else if(correctForm && (rb == IMFD_STOP_BYTE)){
			hasNewData = true;
			rBuff[rAddr++][buffNo] = rb;
			buffNo = 1-buffNo;
		}else{
			rBuff[rAddr++][buffNo] = rb;
		}
		if(rAddr >= READ_BUFFER_LENGTH){
			correctForm = false;
			rAddr = 0;
		}
	}
}



//-------------------------------------------------------------------------------------------------
// class iMFD_Sensor ------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
// コンストラクタ・デストラクタ

iMFD_Sensor::iMFD_Sensor(bool readable){
	own  = readable;
	type = SENS_NA;
};

iMFD_Sensor::iMFD_Sensor(iMFD_SENSOR_TYPE snsType){
	own  = true;
	type = SENS_NA;
	setSensorType(snsType);
}

iMFD_Sensor::iMFD_Sensor(const iMFD_Sensor& obj){
	this->clone(obj);
}


iMFD_Sensor::~iMFD_Sensor(){
	//配列を1個ずらす?
	// →保留
};


//-------------------------------------------------------------------------------------------------
// 関数
// 他のiMFD_Sensorクラスのデータをコピーする
// 引数：コピー元のクラス
void iMFD_Sensor::clone(const iMFD_Sensor obj){
	this->own        = obj.own;
	this->val        = obj.val;
	this->instanceNo = obj.instanceNo;
	this->type       = obj.type;
};

// 他のiMFD_Sensorポインタのデータをコピーする
// 引数：コピー元のポインタ
void iMFD_Sensor::clone(const iMFD_Sensor* ptr){
	this->own        = ptr->own;
	this->val        = ptr->val;
	this->instanceNo = ptr->instanceNo;
	this->type       = ptr->type;
};

// センサの総数をクリアする
static void iMFD_Sensor::resetSnsCount(){
	snsNum = 0;
};


// センサタイプ未設定のクラスにセンサタイプを設定する
// 引数：設定するセンサタイプ
bool iMFD_Sensor::setSensorType(iMFD_SENSOR_TYPE snsType){
	// センサ数オーバー
	if(snsNum >= SENSOR_MAX)
		return false;

	// センサが既に割当済
	if(type != SENS_NA)
		return false;

	type = snsType;
	instanceNo = iMFD.calcNextInstance(snsType);

	iMFD.sens[snsNum]=this;
	snsNum++;

	return true;
};

void iMFD_Sensor::setOwn(bool isOwn){
	own = isOwn;
};


// センサのインスタンス番号を取得する
byte iMFD_Sensor::getInstanceNo(){
	return instanceNo;
};


// センサのインスタンス番号を設定する
void iMFD_Sensor::setInstanceNo(int no){
	instanceNo = no;
};


// センサのタイプを取得する
iMFD_SENSOR_TYPE iMFD_Sensor::getType(){
	return type;
};


// センサが自身に接続された物か判別する
bool iMFD_Sensor::isOwn(){
	return own;
}


unsigned int iMFD_Sensor::getVal(){
	return val;
}

void iMFD_Sensor::setVal(unsigned int value){
	val = value;
}

// 通信まわり
	// センサデータに対応した5byteのデータパケットを取得する
	// 引数：パケット番号
byte iMFD_Sensor::getPucket(byte no){
	switch(no){
		case 0:
			return ((type >> 6) & 0x3F);
		case 1:
			return ( type       & 0x3F);
		case 2:
			return ( instanceNo & 0x3F);
		case 3:
			return ((val  >> 6) & 0x3F);
		case 4:
			return ( val        & 0x3F);
		default :
			return 0;
	 }
};

/*
// 廃止
// データ入出力
	// センサタイプに応じたセンサデータを代入する
	// 引数：センサデータ
void  iMFD_Sensor::setSensData(float data){
 	val = iMFD_encode(data,type);
};

// センサタイプに応じたセンサデータを取得する
float iMFD_Sensor::getSensData(){
	return iMFD_decode(val,type);
}
*/

// 演算子
unsigned int iMFD_Sensor::operator +(const unsigned int num){
	return this->val + num;
}

unsigned int iMFD_Sensor::operator -(const unsigned int num){
	return this->val - num;
}

unsigned int iMFD_Sensor::operator *(const unsigned int num){
	return this->val * num;
}

unsigned int iMFD_Sensor::operator /(const unsigned int num){
	return this->val / num;
}

unsigned int iMFD_Sensor::operator %(const unsigned int num){
	return this->val % num;
}

unsigned int & iMFD_Sensor::operator =(const unsigned int num){
	return this->val = num;
}

iMFD_Sensor & iMFD_Sensor::operator =(const iMFD_Sensor &sns){
	this->clone(sns);
	return *this;
}

iMFD_Sensor::operator unsigned int() const{
	return val;
}


//-------------------------------------------------------------------------------------------------
// class iMFD_Sensor ------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
// コンストラクタ・デストラクタ

iMFD_Class::iMFD_Class(){
}

// 通信・割り込み開始
	// 引数：エッジデバイスかどうか=termジャンパの有無、送信インターバル、ボーレート
void iMFD_Class::begin(bool isEdgeDevice,unsigned int interval_ms,unsigned int baud){
	edge = isEdgeDevice;
	SerialPtr->begin(baud);

	#if defined(DEBUG_SEND_SERIAL_MONITOR)     || \
			defined(DEBUG_RCV_SERIAL_MONITOR)      || \
			defined(DEBUG_DATA_VIEWR_OWN_ONLY)     || \
			defined(DEBUG_DATA_VIEWR_NOT_OWN_ONLY) || \
			defined(DEBUG_DATA_VIEWR_ALL)
		if((Stream*)SerialPtr != (Stream*)DebugSerialPtr)
			DebugSerialPtr->begin(baud);
	#endif

	//タイマ割り込みで送信処理する
	#ifndef NO_TIMER_INT
		MsTimer2::set(interval_ms / (edge?1:RCV_INTERRUPT_DIVISION),timerHandler);
		MsTimer2::start();
	#endif
}


// 次に設定されるべきインスタンス番号を取得する
byte iMFD_Class::calcNextInstance(iMFD_SENSOR_TYPE snsType){
	byte ret = 0;
	for(int i = 0;i < iMFD_Sensor::snsNum;i++){
		if(sens[i]->getType() == snsType)
			ret = (ret > sens[i]->getInstanceNo() ? ret :  sens[i]->getInstanceNo()+1);
	}
	return ret;
}

// センサメンバを取得する。
bool iMFD_Class::getSensor(iMFD_Sensor& sns,iMFD_SENSOR_TYPE snsType,byte instanceNo){
	for(int i = 0;i < iMFD_Sensor::snsNum;i++){
		if(
			(sens[i]->getType()       == snsType   )&&
			(sens[i]->getInstanceNo() == instanceNo)
		){
			sns.clone(sens[i]);
			return true;
		}
	}
	return false;
}

// センサメンバのアドレスを取得する、存在しないセンサメンバを選択した場合NULLを返す
iMFD_Sensor* iMFD_Class::getSensorAddr(iMFD_SENSOR_TYPE snsType,byte instanceNo){
	for(int i = 0;i < iMFD_Sensor::snsNum;i++){
		if(
			(sens[i]->getType()       == snsType   )&&
			(sens[i]->getInstanceNo() == instanceNo)
		)
		return sens[i];
	}
	return NULL;
}

// デバイスがエッジデバイスかどうかを返す
bool iMFD_Class::isEdge(){
	return edge;
}


// 通信の内部処理
// データ送信関数
void iMFD_Class::sendiMFD(){
#ifdef DEBUG_SEND_SERIAL_MONITOR
	DebugSerialPtr->println("SEND DATA ->");
	DebugSerialPtr->print(IMFD_START_BYTE);
	DebugSerialPtr->print(",");
#endif
#ifndef DEBUG_SKIP_TRANSFER
	SerialPtr->write(IMFD_START_BYTE);
#endif

	for(int i =  0 ;i < iMFD_Sensor::snsNum ;i++){
		for(int j = 0;j < 5;j++){
			#ifdef DEBUG_SEND_SERIAL_MONITOR
				DebugSerialPtr->print(sens[i]->getPucket(j));
				DebugSerialPtr->print(",");
			#endif
			#ifndef DEBUG_SKIP_TRANSFER
				SerialPtr->write(sens[i]->getPucket(j));
			#endif
		}
	}

	#ifdef DEBUG_SEND_SERIAL_MONITOR
		DebugSerialPtr->println(IMFD_STOP_BYTE);
	#endif
	#ifndef DEBUG_SKIP_TRANSFER
		SerialPtr->write(IMFD_STOP_BYTE);
	#endif
}


// データ受信関数
bool iMFD_Class::receiveiMFD(){
	UpdateRcvFrame();

	int snsNo = 0;
	if(hasNewData){
		#ifdef DEBUG_RCV_SERIAL_MONITOR
			DebugSerialPtr->println("-> RECEIVE DATA");
			DebugSerialPtr->print("128,");
		#endif

		while(rBuff[snsNo*5+1][1-buffNo] != IMFD_STOP_BYTE){
			iMFD.recieveSensor(
				rBuff[snsNo*5+1][1-buffNo],
				rBuff[snsNo*5+2][1-buffNo],
				rBuff[snsNo*5+3][1-buffNo],
				rBuff[snsNo*5+4][1-buffNo],
				rBuff[snsNo*5+5][1-buffNo]
			);
			#ifdef DEBUG_RCV_SERIAL_MONITOR
				DebugSerialPtr->print(rBuff[snsNo*5+1][1-buffNo]);
				DebugSerialPtr->print(",");
				DebugSerialPtr->print(rBuff[snsNo*5+2][1-buffNo]);
				DebugSerialPtr->print(",");
				DebugSerialPtr->print(rBuff[snsNo*5+3][1-buffNo]);
				DebugSerialPtr->print(",");
				DebugSerialPtr->print(rBuff[snsNo*5+4][1-buffNo]);
				DebugSerialPtr->print(",");
				DebugSerialPtr->print(rBuff[snsNo*5+5][1-buffNo]);
				DebugSerialPtr->print(",");
			#endif

			snsNo++;
		}

		#ifdef DEBUG_RCV_SERIAL_MONITOR
			DebugSerialPtr->println(IMFD_STOP_BYTE);
		#endif

		hasNewData = false;
		return true;
	}else{
		return false;
	}
}

// センサメンバを追加する
void iMFD_Class::addSensor(iMFD_Sensor* ptr){
	sens[iMFD_Sensor::snsNum] = ptr;
	iMFD_Sensor::snsNum++;
}


// 受信した5byteのデータパケットからセンサデータを更新する、対応するセンサが無ければ新規に作る
// 引数、パケット1~5
void iMFD_Class::recieveSensor(byte b1,byte b2,byte b3,byte b4,byte b5){
	iMFD_SENSOR_TYPE rType       = (iMFD_SENSOR_TYPE)((0x3F&b1)<<6 | (0x3F&b2));
	byte             rInstanceNo =                    (0x3F&b3)                ;
	unsigned int     rData       =                    (0x3F&b4)<<6 | (0x3F&b5) ;

	iMFD_Sensor* sns = getSensorAddr(rType,rInstanceNo);
	if(sns == NULL){
		sns = &extSns[iMFD_Sensor::snsNum];
		sns->setSensorType(rType);
		sns->setInstanceNo(rInstanceNo);
		sns->setOwn(false);

//		DebugSerialPtr->print("Add Sensor : ");
//		DebugSerialPtr->print(rType);
//		DebugSerialPtr->print(" / ");
//		DebugSerialPtr->print(rInstanceNo);

//		addSensor(sns);
	}else if(sns->isOwn()){
		sns->setInstanceNo(calcNextInstance(rType));
		sns = &extSns[iMFD_Sensor::snsNum];
		sns->setSensorType(rType);
		sns->setInstanceNo(rInstanceNo);
		sns->setOwn(false);

//		DebugSerialPtr->print("shift Sensor : ");
//		DebugSerialPtr->print(rType);
//		DebugSerialPtr->print(" / ");
//		DebugSerialPtr->print(rInstanceNo);

//		addSensor(sns);

	}
	sns->setVal(rData);
}



//-------------------------------------------------------------------------------------------------
// Functions --------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
/*
// 廃止
unsigned int iMFD_encode(float value,iMFD_SENSOR_TYPE snsType){
	unsigned int ret = 0;
	switch(snsType){
		case EXHAUST_GAS_TEMP:
			ret = (unsigned int) ((value - 32) / 1.8);
			break;

		default:
			ret = 0;
			break;
	}
	return (unsigned int) ret;
}
float        iMFD_decode(int   raw  ,iMFD_SENSOR_TYPE snsType){
	float ret = 0;
	switch(snsType){
		case EXHAUST_GAS_TEMP:
			ret = ((float) raw) * 1.8 + 32;
			break;

		default:
			ret = 0;
			break;
	}
	return (float)ret;
}
*/

char label[7];
void timerHandler(){
		if(iMFD.isEdge()){
			// デバイスがデイジーチェーンのエッジにある場合
			// →受信処理はいらないので、イベントが実行されたらデータを送信する。
			#ifdef ENABLE_IMFD_TRANSFER
				iMFD.sendiMFD();
			#endif
		}else{
			// デバイスがデイジーチェーンのエッジではない場合
			// →新たなデータを受信した時だけ、データを送信する。
			#ifdef ENABLE_IMFD_RECEIVE
				if(iMFD.receiveiMFD()){
					#ifdef ENABLE_IMFD_TRANSFER
						iMFD.sendiMFD();
					#endif
				}
			#endif
		}


	#ifdef DEBUG_DATA_VIEWR_OWN_ONLY
		DebugSerialPtr->println("---------- OWN SENSOR MONITOR ----------");
		for(int i = 0;i < iMFD_Sensor::snsNum;i++){
			if(iMFD.sens[i]->isOwn()){
				if(!getDataLabel7(iMFD.sens[i]->getType(),label))
					DebugSerialPtr->print  (iMFD.sens[i]->getType());

				DebugSerialPtr->print  (label);
				DebugSerialPtr->print  ("(");
				DebugSerialPtr->print  (iMFD.sens[i]->getInstanceNo());
				DebugSerialPtr->print  ("):");
				DebugSerialPtr->println(iMFD.sens[i]->getVal());
			}
		}
		DebugSerialPtr->println("----------------------------------------");
	#endif

	#ifdef DEBUG_DATA_VIEWR_NOT_OWN_ONLY
		DebugSerialPtr->println("--------- SLAVE SENSOR MONITOR ---------");
		for(int i = 0;i < iMFD_Sensor::snsNum;i++){
			if(!iMFD.sens[i]->isOwn()){
				if(!getDataLabel7(iMFD.sens[i]->getType(),label))
					DebugSerialPtr->print  (iMFD.sens[i]->getType());

				DebugSerialPtr->print  (label);
				DebugSerialPtr->print  ("(");
				DebugSerialPtr->print  (iMFD.sens[i]->getInstanceNo());
				DebugSerialPtr->print  ("):");
				DebugSerialPtr->println(iMFD.sens[i]->getVal());
			}
		}
		DebugSerialPtr->println("----------------------------------------");
	#endif

	#ifdef DEBUG_DATA_VIEWR_ALL
		DebugSerialPtr->println("------------ SENSOR MONITOR ------------");
		for(int i = 0;i < iMFD_Sensor::snsNum;i++){
			if(!getDataLabel7(iMFD.sens[i]->getType(),label))
				DebugSerialPtr->print  (iMFD.sens[i]->getType());

			DebugSerialPtr->print  (label);
			DebugSerialPtr->print  ("(");
			DebugSerialPtr->print  (iMFD.sens[i]->getInstanceNo());
			DebugSerialPtr->print  ("):");
			DebugSerialPtr->println(iMFD.sens[i]->getVal());
		}
		DebugSerialPtr->println("----------------------------------------");
	#endif
};



bool getDataLabel7(iMFD_SENSOR_TYPE type,char* str){
	switch(type){
		case AIR_FUEL:
			strcpy(str,"AirFel");
			break;
		case EXHAUST_GAS_TEMP:
			strcpy(str,"EGTtmp");
			break;
		case FLUID_TEMP:
			strcpy(str,"FLDtmp");
			break;
		case VACCUM:
			strcpy(str,"VACCUM");
			break;
		case BOOST:
			strcpy(str,"BOOST ");
			break;
		case AIR_INTAKE_TEMP:
			strcpy(str,"INTAKE");
			break;
		case RPM:
			strcpy(str,"RPM   ");
			break;
		case CAR_SPEED:
			strcpy(str,"CARspd");
			break;
		case THROTTLE:
			strcpy(str,"THROTL");
			break;
		case ENGINE_LOAD:
			strcpy(str,"ENGINE");
			break;
		case FUEL_PRESSURE:
			strcpy(str,"FELprs");
			break;
		case TIMING:
			strcpy(str,"TIMING");
			break;
		case MAP:
			strcpy(str,"MAP   ");
			break;
		case MAF:
			strcpy(str,"MAF   ");
			break;
		case FUEL_TRIM_SHORT:
			strcpy(str,"FLtrmS");
			break;
		case FUEL_TRIM_LONG:
			strcpy(str,"FLtrmL");
			break;
		case OXYGEN:
			strcpy(str,"OXYGEN");
			break;
		case FUEL_LEVEL:
			strcpy(str,"FEL Lv");
			break;
		case VOLT:
			strcpy(str,"VOLT  ");
			break;
		case KNOCK:
			strcpy(str,"KNOCK ");
			break;
		case DUTY:
			strcpy(str,"DUTY  ");
			break;
		case RESERVED_21:
			strcpy(str,"N/A 21");
			break;
		case RESERVED_22:
			strcpy(str,"N/A 22");
			break;
		case RESERVED_23:
			strcpy(str,"N/A 23");
			break;
		case RESERVED_24:
			strcpy(str,"N/A 24");
			break;
		case RESERVED_25:
			strcpy(str,"N/A 25");
			break;
		case RESERVED_26:
			strcpy(str,"N/A 26");
			break;
		case RESERVED_27:
			strcpy(str,"N/A 27");
			break;

// for debug
		case 63:
			strcpy(str,"DBG 63");
			break;
		case 64:
			strcpy(str,"DBG 64");
			break;

		default :
			strcpy(str,"UNKOWN");
			return false;
			break;
	}
	return true;
};//length :
