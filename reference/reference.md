# iMFD リファレンスと使い方
大まかな使い方は[ページ下部](#大まかな使い方)へ。

## 注意事項
タイミング管理に`MsTimer2`を使用しています。  
`MsTimer2`が使用できないボードを使用する際は、[機能設定](#機能設定)の`NO_TIMER_INT`を有効化してください。

通信に使用するシリアルポートは、iMFD.cpp先頭の`SerialPtr`で定義しています。

## リファレンス
---
### iMFD_SENSOR_TYPE
センサの種類を指定する列挙型。

<details><summary> リストの一覧 </summary><div>

単位が※の物は、下部の計算式かiMFDのドキュメントに換算式が載っているのでそれを参照。

| 名前                |   値   |         説明         | 単位 |
| ------------------- |:------:|:--------------------:|:----:|
| ` AIR_FUEL        ` |   0    |        空燃比        |  ※1  |
| ` EXHAUST_GAS_TEMP` |   1    |     廃棄ガス温度     |  ℃   |
| ` FLUID_TEMP      ` |   2    |         水温         |  ℃   |
| ` VACCUM          ` |   3    |        吸気圧        |  ※2  |
| ` BOOST           ` |   4    |       ブースト       |  ※3  |
| ` AIR_INTAKE_TEMP ` |   5    |       吸気温度       |  ℃   |
| ` RPM             ` |   6    |         RPM          |  ※4  |
| ` CAR_SPEED       ` |   7    |         車速         |  ※5  |
| ` THROTTLE        ` |   8    |    スロットル位置    |  %   |
| ` ENGINE_LOAD     ` |   9    |     エンジン負荷     |  %   |
| ` FUEL_PRESSURE   ` |   10   |         燃圧         |  ※6  |
| ` TIMING          ` |   11   |      タイミング      |  ※7  |
| ` MAP             ` |   12   |  マニホールド絶対圧  | kPa  |
| ` MAF             ` |   13   |      流入空気量      | g/s  |
| ` FUEL_TRIM_SHORT ` |   14   |    短期燃料消費量    |  ※8  |
| ` FUEL_TRIM_LONG  ` |   15   |    長期燃料消費量    |  ※9  |
| ` OXYGEN          ` |   16   |   狭帯域酸素センサ   |  %   |
| ` FUEL_LEVEL      ` |   17   |      燃料レベル      |  %   |
| ` VOLT            ` |   18   |         電圧         | ※10  |
| ` KNOCK           ` |   19   |        ノック        | ※11  |
| ` DUTY            ` |   20   | デューティーサイクル |  ※12    |
| ` RESERVED_21     ` |   21   |      内部データ      |  -   |
| ` RESERVED_22     ` |   22   |      内部データ      |  -   |
| ` RESERVED_23     ` |   23   |      内部データ      |  -   |
| ` RESERVED_24     ` |   24   |      内部データ      |  -   |
| ` RESERVED_25     ` |   25   |      内部データ      |  -   |
| ` RESERVED_26     ` |   26   |      内部データ      |  -   |
| ` RESERVED_27     ` |   27   |      内部データ      |  -   |
| ` SENS_NA         ` | 0xFFFF |        未設定        |  -   |

※1 ドキュメント参照。  
※2 $ \rm{val} = \frac{760.4 - P[\rm{mm/Hg}]}{2.23} $  
※3 $ \rm{val} = 329.47 \times P[\rm{kg/cm^2}] $  
※4 $ \rm{val} = \frac{n[\rm{rpm}]}{19.55} $  
※5 $ \rm{val} = 3.97 \times v[\rm{km/h}] $  
※6 ドキュメント参照。  
※7 $ \rm{val} = 64 + \theta[\rm{degree}] $  
※8 $ \rm{val} = 100 + r[%] $  
※9 $ \rm{val} = 100 + r[%]$  
※10 $\rm{val} = 51.15 \times v[\rm{V}]$  
※11 $\rm{val} = 204.6 \times v[\rm{V(0\sim 5)}]$  
※12 $\rm{val} = 10.23 \times r[%]$

</div></details>


---
### iMFD_Sensor
センサ単体の情報を保存するクラス。

#### iMFD_Sensor(iMFD_SENSOR_TYPE snsType)
センサデータを保存するクラスのインスタンスを新たに生成する。  
引数`iMFD_SENSOR_TYPE snsType`:[センサタイプ](#imfd_sensor_type)。省略可。省略した場合[`bool setSensorType(iMFD_SENSOR_TYPE snsType)`](#bool-setsensortypeimfd_sensor_type-snstype)でセンサタイプを設定する。

<details><summary> 例 </summary><div>

```c
iMFD_Sensor sns_af;
iMFD_Sensor sns_egt(EXHAUST_GAS_TEMP);

```

</div></details>


#### bool setSensorType(iMFD_SENSOR_TYPE snsType)
センサタイプを設定する。一度設定したセンサタイプは変更できない。  
引数`iMFD_SENSOR_TYPE snsType`:[センサタイプ](#imfd_sensor_type)。  
返り値：設定に成功した場合true。センサタイプが設定済みだった場合と、センサの数が上限を超えていた場合false。

<details><summary> 例 </summary><div>

```c
iMFD_Sensor sns_af;

void setup(){
	sns_af.setSensorType(AIR_FUEL);
}

```

</div></details>

### byte getInstanceNo()
センサのインスタンス番号を取得する。  
一つのデータバス上に同じタイプのセンサが複数接続されている場合、それぞれの識別の為インスタンス番号が上流から順に0,1,2...と設定される。
プログラム内で定義した`iMFD_Sensor`のインスタンス番号は自動で割り振られる。  
返り値：インスタンス番号

<details><summary> 例 </summary><div>

```c
iMFD_Sensor sns_af1(AIR_FUEL);
iMFD_Sensor sns_af2(AIR_FUEL);

void loop(){
	byte af_no = sns_af1.getInstanceNo(); // af_no == 0
	af_no      = sns_af2.getInstanceNo(); // af_no == 1
}

```

</div></details>

### iMFD_SENSOR_TYPE getType()
センサタイプを取得する。
返り値：現在の[センサタイプ](#imfd_sensor_type)。

<details><summary> 例 </summary><div>

```c
iMFD_Sensor sns_af(AIR_FUEL);

void loop(){
	iMFD_SENSOR_TYPE type = sns_af.getType(); // type == AIR_FUEL
}

```

</div></details>


### bool isOwn()
センサが自身のArduinoに接続されたデバイスかを判定する。  
返り値：センサが自身に接続された物である場合`true`、下流に接続された別のiMFDデバイスの物である場合`false`

<details><summary> 例 </summary><div>

```c
iMFD_Sensor sns_af(AIR_FUEL);

void loop(){
	bool own = sns_af.isOwn(); // own == true
}

```

</div></details>


### 演算子
`unsigned int`型の値に対して四則演算(`+`,`-`,`*`,`/`})と余り(`%`)を使用可能。代入演算子(`=`)を使っての値の代入も可能。

<details><summary> 例 </summary><div>

```c
iMFD_Sensor sns_af(AIR_FUEL);
unsigned int value;

void loop(){
	sns_af = 100;
	value = sns_af + 1;
	value = sns_af - 2;
	value = sns_af * 3;
	value = sns_af / 4;
	value = sns_af % 5;
}

```

</div></details>

---
## iMFD_Sensor
データバスに接続されたセンサデータ全てを保存するクラス。  
自身に接続されたセンサのデータと、その下流に接続されたiMFDシリーズ製品のデータを纏めている。
iMFD.cpp内で`iMFD_Class iMFD`が定義されている。基本的には`iMFD_Class iMFD`のメンバを使用する。  

### begin(bool isEdgeDevice,unsigned int interval_ms,unsigned int baud)
iMFDデバイスとの通信を開始する。  
引数`bool isEdgeDevice`：デバイスがエッジデバイスかどうかを設定する。iMFDデバイスのTERMピンと同等のフラグ。TRUEでエッジデバイス。デフォルトではfalse。  
引数`unsigned int interval_ms`：上位のiMFDデバイスにデータを送信する間隔。省略可。iMFDシリーズの標準は100ms。  
引数`unsigned int baud`：シリアル通信で他のiMFDデバイスにデータを送信する際のボーレート。省略可。iMFDシリーズの標準は19200。  

<details><summary> 例 </summary><div>

```c
void setup(){
	iMFD.begin();
	// 20ms おきにデータを上げたい時(上位の機器がそれに対応している必要がある)
	// iMFD.begin(20);
}

```

</div></details>

### bool getSensor(iMFD_Sensor& sns,iMFD_SENSOR_TYPE snsType,byte instanceNo)
特定のセンサインスタンスを取得する。  
自身に接続されたセンサと、自身の下流に接続されたiMFDデバイスのセンサを取得できる。  
引数`iMFD_Sensor& sns`：取得したデータを代入するセンサインスタンス(代入先のインスタンスはセンサタイプが指定されていない必要がある)  
引数`iMFD_SENSOR_TYPE snsType,`：取得するセンサデータの[センサタイプ](#imfd_sensor_type)。  
引数`byte instanceNo`：取得するセンサデータの[インスタンス番号](#bytegetinstanceno)。省略した場合、最初のインスタンス(=0)を取得する。  
返り値：センサデータを取得できた場合`true`。存在しないセンサを指定した場合、代入先にセンサタイプを指定したインスタンスを指定した場合`false`。

<details><summary> 例 </summary><div>

```c
//
// 下流にWideband Air/FuelのiMFDセンサが接続されているとする。
// 構成
//                             ↓インスタンスno.=0
// …--[TX  Arduino  RX]--[TX  iMFD Air/Fuel センサ(TERM)  RX]
//          ↑このArduinoのプログラム
//            インスタンスNo.=1
//
//
iMFD_Sensor sns_af(AIR_FUEL);
unsigned long value;

void loop(){
	iMFD_Sensor sns;
	sns_af = 100;

	if(iMFD.getSensor(sns,AIR_FUEL))   // インスタンスNo.==0 →一番下流のセンサ(接続されたiMFDセンサ)のデータを取得。
	{
		// センサが接続されていた時の処理
		value = sns; // センサデータを代入
	}else{
		// センサが接続されていなかった時の処理
	}

	if(iMFD.getSensor(sns,AIR_FUEL,1)) // インスタンスNo.==1 →下流から2番目のセンサ(Arduino自身のセンサ=sns_af)のデータを取得。
	{
		// センサが接続されていた時の処理
		value = sns; // センサデータを代入, value == 100
	}else{
		// センサが接続されていなかった時の処理
	}


	// エラーとなる場合
	if(iMFD.getSensor(sns,AIR_FUEL,2)){ // インスタンスNo.==2のセンサは存在しない→falseを返す。
		//実行されない。
	}

	if(iMFD.getSensor(sns,EXHAUST_GAS_TEMP)){ // EXHAUST_GAS_TEMPのセンサは存在しない→falseを返す。
		//実行されない。
	}

	if(iMFD.getSensor(sns_af,AIR_FUEL)){ // sns_afは既にAIR_FUELセンサとして設定されているので代入ができない→falseを返す。
		//実行されない。
	}

}

```

</div></details>



---
## その他設定、デバッグ機能等

### 機能設定
iMFD.h内の`#define`をコメントアウトすることで、挙動を変更できる。
(未実装の機能あり。)   

- 動作の設定
	- `NO_TIMER_INT`
	プログラム内の時間管理に`MsTimer2`を使用しなくする。`MsTimer2`が使用できないボードを使用している場合、コメントアウトを解除する。  
	コメントアウトを解除した場合、`loop`文の中で送信処理を行う必要がある。[void timerHandler()](#void-timerhandler)を参照
- デバッグ用の機能
	- `ENABLE_IMFD_RECEIVE`  
	接続されたiMFDセンサからのデータ取得を行う。コメントアウトすると、デバッグを含む一切のデータの取得をスキップする。
	- `ENABLE_IMFD_TRANSFER`  
	接続されたiMFDモニタへのデータ送信を行う。コメントアウトすると、デバッグを含む一切のデータの送信をしない。
	- `ENABLE_IMFD_RECEIVE`  
	下流のiMFDデバイスからのデータ受信を有効にする。コメントアウトでデータ受信無効化。エッジデバイスの際は強制的に受信が無効化される。
	- `ENABLE_IMFD_TRANSFER`  
	上流のiMFDデバイスへのデータ受信を有効にする。コメントアウトでデータ送信無効化。
	- `DEBUG_TO_SERIAL_MONITOR`  
	コメントアウト解除で、Arduino mega 2560のような複数のSerialを持つHWで、iMFDに送信したデータをシリアルモニタにも送信するようにする。  
	シリアルモニタへ送信するシリアルポートと、 iMFDに送信するシリアルポートはiMFD.cppの先頭で設定している。
	- `DEBUG_DATA_VIEWR_OWN_ONLY`  
	コメントアウト解除で、シリアルポートに自身に接続されたセンサデータのデバッグ表示を行う。
	シリアルモニタへ送信するシリアルポートと、 iMFDに送信するシリアルポートはiMFD.cppの先頭で設定している。
	- `DEBUG_DATA_VIEWR_NOT_OWN_ONLY`  
	コメントアウト解除で、シリアルポートに下流に接続されたセンサデータのデバッグ表示を行う。
	シリアルモニタへ送信するシリアルポートと、 iMFDに送信するシリアルポートはiMFD.cppの先頭で設定している。
	- `DEBUG_DATA_VIEWR_ALL`  
	コメントアウト解除で、シリアルポートにすべてのセンサデータのデバッグ表示を行う。
	シリアルモニタへ送信するシリアルポートと、 iMFDに送信するシリアルポートはiMFD.cppの先頭で設定している。
	- `DEBUG_SKIP_TRANSFER`  
	コメントアウト解除で、上流のiMFDへのデータ送信をスキップする。  
	`DEBUG_TO_SERIAL_MONITOR`をONにしたとき、Arduino Unoなどを使ってるとシリアルモニタへ送信するシリアルポートとiMFDのシリアルポートが一緒のシリアルポートになる。  
	1つのシリアルポートにデータが混在してしまい見にくくなるため、有効化を推奨。

### void timerHandler()
データの送受信を行う関数。`MsTimer2`により、指定されたインターバルごとに実行される。  
[機能設定](#機能設定)で`NO_TIMER_INT`を設定した場合、割り込みによる定期実行が発生しなくなるので、loop文で定期的に`timerHander()`を実行する必要がある。

<details><summary> 例 </summary><div>

```c
void loop(){
	// だいたい100msごとにデータを処理する
	delay(100);
	timerHandler();
}

```

</div></details>

---

## 大まかな使い方
### Arduinoに接続されたセンサのデータを他のiMFDに送る
1. `setup`文、`loop`文の外側でセンサ変数を定義する。型は`iMFD_Sensor`。  
センサ変数は初期化時にどのタイプのセンサかを定義する。
```c
iMFD_Sensor GAS(EXHAUST_GAS_TEMP);
// ~~
void setup(){
	// ~~
}
```
1. `setup文`の中でiMFDを開始する。デフォルトでは100msおきにデータを上げるが、周期を変えたい時は引数を追加する。
```c
void setup(){
	iMFD.begin();
	// 20ms おきにデータを上げたい時
	// iMFD.begin(20);
}
```
1. `loop`文の中で値を代入する。値を代入すると勝手にその値をiMFDに送信してくれる。  
通信に載っているデータをそのまま使う場合、=で代入したり、+で演算もできる。
```c
iMFD_Sensor GAS(EXHAUST_GAS_TEMP);
void setup(){
	iMFD.begin();
}
void loop(){
	GAS = 100; //センサデータを代入。
	// iMFDモニタには、100℃=212Fが表示される。
}
```

### 他のiMFDに接続されたセンサのデータをArduinoで取得する
1. `setup文`の中でiMFDを開始する。デフォルトでは100msおきにデータを受信するが、周期を変えたい時は引数を追加する。
```c
void setup(){
	iMFD.begin();
	// 20ms おきにデータを上げたい時
	// iMFD.begin(20);
}
```
1. `loop`文の中などで、センサデータを保存するための変数を定義する。
```c
void loop(){
	iMFD_Sensor gas;
	// ---
}
```
1.`iMFD.getSensor()`でセンサを探す。この関数は該当するセンサが無いと、`false`を返す。  
引数は、上で定義したセンサ変数とセンサのタイプ。センサが複数接続されていて場合、何番目のセンサを取得するかは2つめの引数で指定する。
```c
void loop(){
	iMFD_Sensor gas;
	if(iMFD.getSensor(gas,EXHAUST_GAS_TEMP))
	// 3個目の EGTセンサを見たい時
	// if(iMFD.getSensor(gas,EXHAUST_GAS_TEMP,2))
	{
		// センサが接続されていた時の処理
	}else{
		// センサが接続されていなかった時の処理
	}
}
```
1.値を取得する。
```c
void setup(){
	iMFD.begin();
}
void loop(){
	iMFD_Sensor gas;
	if(iMFD.getSensor(gas,EXHAUST_GAS_TEMP))
	{
		int data = gas; // 取得したセンサデータ
	}else{
		// センサが接続されていなかった時の処理
	}
}
```
