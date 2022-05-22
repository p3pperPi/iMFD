#ifndef IMFD_H_
#define IMFD_H_
// pragma
#pragma GCC diagnostic warning "-fpermissive"



// フラグ

// MsTimerでの割り込みをしない
//#define NO_TIMER_INT

//デバッグ用フラグ
// ほかのハードからのシリアル受信機能を有効化する (EDGE の時は強制的に無効化される)
#define ENABLE_IMFD_RECEIVE

// ほかのハードへのシリアル送信機能を有効化する
#define ENABLE_IMFD_TRANSFER

// デバッグ用、送信データをシリアルモニタで見れる形でデータを出力する
// #define DEBUG_SEND_SERIAL_MONITOR

// デバッグ用、受信データをシリアルモニタで見れる形でデータを出力する
// #define DEBUG_RCV_SERIAL_MONITOR

// デバッグ用、シリアルモニタに自身に接続されたセンサのデータを全て表示する
// #define DEBUG_DATA_VIEWR_OWN_ONLY

// デバッグ用、シリアルモニタに自身に接続されていないセンサのデータを全て表示する
// #define DEBUG_DATA_VIEWR_NOT_OWN_ONLY

// デバッグ用、シリアルモニタにセンサのデータを全て表示する
// #define DEBUG_DATA_VIEWR_ALL

// デバッグ用、iMFDデバイスへのシリアル送信処理をスキップする
// #define DEBUG_SKIP_TRANSFER



#define IS_EDGE_DEVICE     true
#define IS_NOT_EDGE_DEVICE false


// includes
#include "Arduino.h"
#ifndef NO_TIMER_INT
	#include <MsTimer2.h>
#endif



// defines
#define SENSOR_MAX      32  //接続できるセンサの最大数
#define IMFD_START_BYTE 128 //
#define IMFD_STOP_BYTE  64  //

// edge ではないときに、1コマンド中に受信データを確認する回数
// interval/RCV_INTERRUPT_DIVISION [ms] で割り込みを実行する。
#define RCV_INTERRUPT_DIVISION 5

// enum
// iMFD センサータイプの定義
typedef enum iMFD_SENSOR_TYPE_T {
	AIR_FUEL         =  0,
	EXHAUST_GAS_TEMP =  1,
	FLUID_TEMP       =  2,
	VACCUM           =  3,
	BOOST            =  4,
	AIR_INTAKE_TEMP  =  5,
	RPM              =  6,
	CAR_SPEED        =  7,
	THROTTLE         =  8,
	ENGINE_LOAD      =  9,
	FUEL_PRESSURE    = 10,
	TIMING           = 11,
	MAP              = 12,
	MAF              = 13,
	FUEL_TRIM_SHORT  = 14,
	FUEL_TRIM_LONG   = 15,
	OXYGEN           = 16,
	FUEL_LEVEL       = 17,
	VOLT             = 18,
	KNOCK            = 19,
	DUTY             = 20,

	RESERVED_21      = 21,
	RESERVED_22      = 22,
	RESERVED_23      = 23,
	RESERVED_24      = 24,
	RESERVED_25      = 25,
	RESERVED_26      = 26,
	RESERVED_27      = 27,


	SENS_NA = 0xFFFF
}iMFD_SENSOR_TYPE;


// class iMFD_Sensor ------------------------------------------------------------------------------
// センサ単体の情報を保存するクラス
class iMFD_Sensor{
private:
	iMFD_SENSOR_TYPE type;       //センサータイプ
	unsigned int     val;        //センサーの値、通信に乗せる生値
	byte             instanceNo; //インスタンス番号
	bool             own;        //自分に接続されているセンサーかどうかのフラグ


public:
// 静的メンバ
	// センサの総数
	static int snsNum;

	// センサの総数をクリアする
	static void resetSnsCount();


	// 他のiMFD_Sensorクラスのデータをコピーする
	// 引数：コピー元のクラス
	void clone(const iMFD_Sensor obj);

	// 他のiMFD_Sensorポインタのデータをコピーする
	// 引数：コピー元のクラス
	void clone(const iMFD_Sensor* ptr);



// コンストラクタ・デストラクタ
	iMFD_Sensor(bool readable = true);
	iMFD_Sensor(iMFD_SENSOR_TYPE snsType);
	iMFD_Sensor(const iMFD_Sensor& obj);
	~iMFD_Sensor();

// センサの設定
	// センサタイプ未設定のクラスにセンサタイプを設定する
	// 引数：設定するセンサタイプ
	bool setSensorType(iMFD_SENSOR_TYPE snsType);

	// 自分に接続されたセンサか設定する
	void setOwn(bool isOwn);

	// センサのインスタンス番号を取得する
	byte getInstanceNo();

	// センサのインスタンス番号を設定する
	void setInstanceNo(int no);

	// センサのタイプを取得する
	iMFD_SENSOR_TYPE getType();

	// センサが自身に接続された物か判別する
	bool isOwn();

	// 値を取得する
	unsigned int getVal();

	// 値をセットする
	void setVal(unsigned int value);

// 通信まわり
	// センサデータに対応した5byteのデータパケットを取得する
	// 引数：パケット番号
	byte getPucket(byte no);

/*
// 廃止
// データ入出力
	// センサタイプに応じたセンサデータを代入する
	// 引数：センサデータ
	void  setSensData(float data);

	// センサタイプに応じたセンサデータを取得する
	float getSensData();
*/



// 演算子
	unsigned int operator +(const unsigned int num);
	unsigned int operator -(const unsigned int num);
	unsigned int operator *(const unsigned int num);
	unsigned int operator /(const unsigned int num);
	unsigned int operator %(const unsigned int num);
	unsigned int &operator =(const unsigned int num);
	iMFD_Sensor &operator =(const iMFD_Sensor &sns);
	operator unsigned int() const;

};



// class iMFD_Class ------------------------------------------------------------------------------
// 全てのセンサデータと通信周りの情報を保存するクラス
// 通信のタイミング管理はタイマ割り込みで行う。
class iMFD_Class{
	bool edge;

public:
	// センサクラスのアドレス配列
	iMFD_Sensor* sens[SENSOR_MAX];


// コンストラクタ・デストラクタ
	iMFD_Class();


// 通信・割り込み開始
	// 引数：エッジデバイスかどうか=termジャンパの有無、送信インターバル、ボーレート
	void begin(bool isEdgeDevice = false,unsigned int interval_ms = 100,unsigned int baud = 19200);

	// 次に設定されるべきインスタンス番号を取得する
	byte calcNextInstance(iMFD_SENSOR_TYPE snsType);

	// センサメンバを取得する、存在しないセンサメンバを選択した場合falseを返す
	bool getSensor(iMFD_Sensor& sns,iMFD_SENSOR_TYPE snsType,byte instanceNo = 0);

	// センサメンバのアドレスを取得する、存在しないセンサメンバを選択した場合NULLを返す
	iMFD_Sensor* getSensorAddr(iMFD_SENSOR_TYPE snsType,byte instanceNo = 0);

	// デバイスがエッジデバイスかどうかを返す
	bool isEdge();

// 通信の内部処理
	// データ送信関数
	void sendiMFD();

	// データ受信関数
	// 返り値：新しいデータを受信したかどうか
	bool receiveiMFD();

// 通信処理の内部関数
// -> privateにする？
	// センサメンバを追加する
	void addSensor(iMFD_Sensor* ptr);

	// 受信した5byteのデータパケットからセンサデータを更新する、対応するセンサが無ければ新規に作る
	// 引数、パケット1~5
	void recieveSensor(byte b1,byte b2,byte b3,byte b4,byte b5);

};


// functions
unsigned int iMFD_encode(float val,iMFD_SENSOR_TYPE snsType);
float        iMFD_decode(int   raw,iMFD_SENSOR_TYPE snsType);
void         timerHandler();

bool         getDataLabel7(iMFD_SENSOR_TYPE type,char* str);//length : 7(includes NULL)

//extern
extern iMFD_Class iMFD;

#endif
