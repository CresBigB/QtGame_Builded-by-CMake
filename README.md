# **伽波大冒险**

---

## 项目描述

​	本项目旨在完成一个类似于knife.io的生存者类游戏，在本项目中，设计有基本的游戏开始界面，智能的AiBot，以及各种趣味道具。玩家可以拾取地图上的Knife道具增加自身刀片的数量，或者拾取无敌道具进入无敌状态，亦可以拾取地图上的❤道具恢复生命值。

​	在游玩过程中，玩家可以通过WASD控制角色移动，当有其他角色进入到玩家的远程攻击范围内时，游戏会标记出指示线指定锁定的角色，这时便可点击鼠标左键发射一柄刀进行远程攻击，被发射的刀会自动的追钟目标角色直至命中或者到达最大飞行时间。~~开发者模式，按i增加新的Bot，j减少自身生命值，k增加五柄刀，l减少十柄刀。~~

​	游玩过程中，游戏会在一开始便生成五个攻击欲望极高的智能Bot，Bot会视情况选择拾取道具或者攻击玩家。请注意，Bot也会远程攻击，请各位玩家小心。

​	在游戏的开始界面提供了丰富的设置选项，包括但不限于游戏的帧率，可活动区域的大小，玩家大学与最大生命值，基础攻击力等等，更多的设置选项我相信能为各位玩家提供更为丰富的游戏体验。

---

	## 项目结构

### 文件树

```cpp
│  CMakeLists.txt
│  main.cpp
├─Forms
│      GameWindow.ui	//开始界面的UI文件
│      
├─Headers	//存储项目头文件
│      Bot.h	//包含Bot类的定义
│      Character.h	//包含角色Character类的定义，以及数种关于Character类的自定义事件的定义与实现
│      GameEngin.h	//包含游戏引擎GameEngin类的定义
│      GameStartWindow.h //游戏开始界面类的定义
│      GameView.h	//游戏主界面GameView类的定义
│      Grass.h	//草丛Grass类的定义
│      Headers.h //包含所用到的所有Qt的头文件，同时定义有辅助函数currentTime
│      Item.h	//物品Item类的定义以及与Item类相关的事件的定义与实现
│      Knife.h	//包含刀具Knife类以及刀圈KnifeRoll类的定义
│      KnifeProjectile.h //刀具飞行物KnifeProjectile类的定义
│      Quadtree.h	//四叉树节点QuadtreeNode类与四叉树Quadtree的定义
│      
├─Resources	//存储项目资源文件
│  │  resources.qrc
│  │  
│  └─Images	//项目中所用图片
│          Acceleration.png
│          background.jpg
│          grass.png
│          Heart.png
│          image.png
│          infinite.png
│          knife.png
│          knife_item.png
│          moving.gif
│          standing.png
│          
└─Sources
        Bot.cpp	//Bot类的实现
        Character.cpp	//Character类的实现
        GameEngin.cpp	//GameEngin类的实现
        GameStartWindow.cpp //GameStartWindow类的实现
        GameView.cpp	//GameView类的实现
        Grass.cpp	//Grass类的实现
        Item.cpp	//Item类的实现
        Knife.cpp	//Knife类的实现
        KnifeProjectile.cpp //KnifeProjectile类的实现
        Quadtree.cpp	//Quadtree类的实现
```

---

### 模块设计

|      类名       |                             描述                             |
| :-------------: | :----------------------------------------------------------: |
|   GameEngine    | 游戏引擎类，使用单例模式，负责整个游戏的事件处理以及帧率控制合 |
|    GameView     | 负责整个游戏界面的显示，存储地图上的所有物品，包括角色，草丛，道具以及飞行物等等，同时负责读取键盘输入以及鼠标案件，游戏中的绝大部分自定义事件由该类发出 |
| GameStartWindow |                 游戏的开始界面，提供设置功能                 |
|    Character    |                         基础的玩家类                         |
|       Bot       | 智能Ai类，继承自类Character，重写了其虚函数updateAI，以实现Ai的决策等功能 |
|      Grass      |                            草丛类                            |
|      Item       | 物品类，内有枚举Type以表示不同类型的物品，物品仅可通过类中的静态函数createItem进行创建，以防止创建出奇怪的物品 |
|      Knife      |                     刀具类，用于显示刀具                     |
|    KnifeRoll    |           玩家身边环绕的刀圈，内存储着数个Knife类            |
| KnifeProjectile |               继承自Knife，用于表示刀具投掷物                |
|  QuadtreeNode   |                        四叉树的节点类                        |
|    Quadtree     |                四叉树，用于全局物品的碰撞检测                |

### 算法介绍

#### 	1.游戏循环

​	在游戏中，使用`GameEngine`类负责整个游戏的逻辑处理以及通过`QTimer`定时器实现按帧刷新。通过重写`customEvent`函数实现对自定义事件的处理。

#### 	2.玩家移动

​	在`GameView`中，重写键盘事件，读取WASD的按下，将其转换为四个方向的移动，同时将四向移动转换为速度向量的形式，通过自定义的`CharacterMoveEvent`，将速度向量以及玩家角色的指针传递给`GameEngine`，由引擎调用其对应的move函数对角色的速度向量进行累加。同时，当其某一方向键松开时，由其发送一个与方向键方向相反的速度向量用于对玩家速度向量进行抵消，如此便实现了玩家的万向移动。

​	对于如何将速度向量转化成具体的移动，在玩家类中提供有一函数`updatePosition`，函数接受当前单帧的时间，根据当前帧时间，玩家当前速度，玩家的速度向量计算玩家位置在当前帧应处的位置，如此便真正的实现了玩家的移动。用单帧时间计算，不仅实现了玩家速度的稳定，同时也实现了玩家移动的顺滑程度，若仅采用对pos进行加减的操作，则不免的会出现类似于瞬移的情况，同时也会导致按键响应变慢。

#### 	3.角色动画切换

​	为了实现角色动画的切换，在Character类中定义了枚举类Status用于表示角色状态，包括移动中，静止以及死亡。当调用Character类的move函数时，其会自行对状态进行判断，若当前速度向量为0，则设置状态为静止，否则则设置状态为移动中。

​	在paint函数中，会根据不同的状态显示不同的玩家图片。当状态为静止时，则仅显示一张静态图片，移动中则使用`Qmovie`显示GIF，同时会根据移动方向的不同使角色有着不同的朝向。

#### 	4.道具效果

​	为实现特殊道具的特殊效果（如加速，无敌），在Character类中创建了一枚举类Effect用于表示可以对玩家造成的特殊效果，同时在Character类中有一`QMap`，用于存储各种效果的开始时间。当玩家拾取到物品使，`GameView`则会发出事件`CharacterPickUpItemEvent`，将拾取道具的角色的指针，被拾取道具的指针传递给游戏引擎，交由其处理。当物品为特殊物品使，引擎便会调用对应角色的`addEffect`函数为其添加特殊效果。

#### 5.碰撞检测

​	在本游戏中，对于玩家与物品的碰撞并没有简单的采用暴力搜索的方法进行，而是将所有的物品存储在一个四叉树中，使用四叉树对空间进行划分，分块的去进行查找，时间复杂度大约等于*O(n)*。在每一帧的刷新中，都会调用碰撞检测函数对所有的玩家进行检测，若有玩家触碰到了物品，则由`GameView`发出事件`CharacterPickUpItemEvent`，将拾取道具的角色的指针，被拾取道具的指针传递给游戏引擎，交由其处理。

## 类的详解

###  GameEngine类

---

#### 静态成员

```cpp
inline static int FPS = 120;	//游戏当前的最高帧率
inline static GameEngine* engine = nullptr; //单例模式实例化的地址
```

#### 公有函数

```cpp
static GameEngine* instance();	//用于获取单例的实例化
```

```cpp
void start();	//用于开启新游戏
```

```cpp
void releaseAll();	//释放所有游戏资源，重置到初始状态
```

#### 信号

```cpp
void backToMenu();	//返回主菜单
```

```cpp

```

#### 保护函数

```cpp
void customEvent(QEvent* event) override;	//用于自定义事件的处理
```

```cpp
void update();	//由定时器每帧调用，负责更新整个游戏
```

```cpp
void characterDeadEvent(const CharacterDeadEvent* event);	//用于处理玩家死亡事件
```

```cpp
void characterMoveEvent(const CharacterMoveEvent* event);	//用于处理角色的移动事件
```

```cpp
void characterPickUpItemEvent(const CharacterPickUpItemEvent* event); //用于处理角色拾取物品的事件
```

```cpp
void characterAttackEvent(const CharacterAttackEvent* event); //用于处理角色之间的近战攻击
```

```cpp
void characterRemoteAttackEvent(const CharacterRemoteAttackEvent* event); //用于处理远程攻击
```

```cpp
void gameFinishEvent(const GameFinishEvent* event);	//处理游戏结束事件
```

```cpp
void startNewGameEvent(const StartNewGameEvent* event);	//处理开启新游戏事件
```

#### 私有成员变量

```cpp
bool isEnd = true;	//游戏是否已结束
int killCount = 0;	//玩家的击杀数

QTimer* timer = nullptr;	//定时器，负责帧刷新
QElapsedTimer lastUpdateTimer;	//记录当前的单帧事件
QElapsedTimer totalTime;	//记录游戏运行总时间

GameView* view = nullptr;	//游戏界面
QGraphicsScene* gameScene = nullptr;	//同上

QMap<Character*, QMap<Character*, qreal>> remoteAttackMap;	//存储玩家对某一玩家发起远程攻击的时间
QMap<Character*, QMap<Character*, qreal>> attackMap;	//存储玩家对某一玩家发起近战攻击的时间
```

### GameView类

---

#### 静态成员

```cpp
inline static double activeRadius = 3600;	//当前可活动范围的半径
inline static int groundWidth = 8000;	//场景的宽度
inline static int groundHeight = 8000;	//场景的高度
inline static int grassNumber = 50;	//生成的草丛的数量
inline static int BotNumber = 5;	//游戏生成的机器人的数量
```

#### 公有函数

```cpp
void initialize();	//对游戏场景进行初始化
void releaseAll();	//释放所有的游戏资源
void test();	//用于调试的快捷键
```

```cpp
//游戏结束界面，显示当前排名，击杀数以及生存时间
void showGameOverScreen(const int killCount, const qreal survivalTime);	
void updateAll(const qreal deltaTime, const qreal currentTime);	//用于更新游戏中的所有物品的状态
void deleteBot(Character* character);	//删除指定Bot
void deleteItem(Item* item);	//删除指定物品
//用于显示远程攻击的指示线
void setIndicativeLine(const bool hasLine, const QPointF& begin = {}, const QPointF& end = {});
//用于创建一个飞刀投掷物
void createKnifeProjectile(Character* source, Character* target);
```

```cpp
[[nodiscard]] bool inActiveSpace(const QPointF& pos) const;	//判断该点是否位于可活动范围
[[nodiscard]] const Character* getCurrentPlayer();	//获取当前玩家角色的地址
```

```cpp
QList<Item*> getAllItem();	//获取当前游戏中存在的所有物品
QList<Bot*> getAllCharacter();	//获取当前游戏中的所有未死亡的Bot
//获取距离传入的玩家距离最近的玩家
[[nodiscard]] Character* getNearestCharacter(Character* character);	
```

#### 保护函数

```cpp
void drawBackground(QPainter* painter, const QRectF& rect) override;	//绘制背景的网格线
void drawForeground(QPainter* painter, const QRectF& rect) override;	//绘制远程攻击的引导线
void keyPressEvent(QKeyEvent* event) override;	//WASD读入，发送角色移动事件
void keyReleaseEvent(QKeyEvent* event) override;	//WASD释放，消除玩家释放方向的向量
void mousePressEvent(QMouseEvent* event) override;	//按下鼠标左键触发远程攻击，发送远程攻击事件
```

```cpp
void generateGrass(const int number);	//生成num数量的草丛，位置随机
void generateBot(const int num);	//生成指定数量的Bot，位置随机
void generateRandomItem();	//生成随机数量的随机物品，不同物品生成概率不同
void generateItem(const Item::Type type, const int number);	//生成指定数量的指定物品
```

```cpp
void updateCharacters(const qreal deltaTime);	//更新所有Bot以及玩家的状态
void updateCollisionManager();	//更新四叉树碰撞检测
void updateCharactersCollision();	//对所有Bot以及玩家进行碰撞检测
void updateCamera();	//更新摄像头位置，使玩家始终剧中
void updateProjectiles(qreal deltaTime);	//更新所有的投掷物的状态，位置
```

#### 私有成员变量

```cpp
Quadtree collisionManager;	//基于四叉树的碰撞检测类
QList<Grass*> grasses;	//存储游戏场景中的所有的草丛
QList<Item*> items;	//存储游戏场景中的所有的物品
QList<Bot*> bots;	//存储未死亡的Bot
QList<KnifeProjectile*> projectiles;	//存储游戏场景中的所有投掷物
Character* player = nullptr;	//当前玩家的地址
QPointF indicativeLineBegin;	//远程攻击指示线的起始坐标
QPointF indicativeLineEnd;		//远程攻击指示线的结束坐标
bool hasIndicativeLine = false;	//是否绘制远程攻击指示线
```

### Character类

---

#### 枚举类

```cpp
enum class Status :quint8 { Moving, Standing, Dead };	//角色状态
enum class Effect :quint8 { None, Acceleration, Invincible };	//特殊物品的效果
enum class Direction :qint8 { Up, Down, Left, Right };	//移动方向
```

#### 静态变量

```cpp
inline static double CharacterWidth = 150;	//角色高度
inline static double CharacterHeight = 150;	//角色宽度

inline static int MaxHealthy = 100;	//角色最大生命值
inline static int BaseAttack = 5;	//角色的基础攻击
inline static int MaxRemoteRadius = 650; //角色的远程攻击半径
inline static qreal BaseSpeed = 700; //像素每秒
```

#### 公有函数

```cpp
void updateCharacter(const qreal deltaTime); //更新角色状态，位置
virtual void move(const QPointF& direction); //传入角色的移动向量，控制角色的移动方向，对向量自动归一化
```

```cpp
void addHealthy(const int num);	//添加/减少生命值，正数增加负数减少
void addEffect(Effect effect);	//添加特殊效果
void addKnife(const int number) const;	//添加/减少刀具数量，正数增加负数减少
```

```cpp
void attackOtherCharacter(Character* character);	//攻击其他角色
```

```cpp
[[nodiscard]] int getHealthy() const;	//获取当前生命值
[[nodiscard]] qreal getSpeed() const;	//获取当前速度
[[nodiscard]] int getAttack() const;	//获取当前攻击力
[[nodiscard]] QRectF getAttackBoundingRect() const;	//获取当前近战攻击的范围（正方形）
[[nodiscard]] qreal getAttackRadius() const;	//获取当前近战攻击的半径（刀圈半径）
[[nodiscard]] QList<Effect> getAllEffects() const;	//获取角色当前持有的所有特殊效果
[[nodiscard]] int getKnifeCount() const;	//获取角色当前刀具数量
[[nodiscard]] bool isBot() const;	//当前角色是否为Bot
[[nodiscard]] bool isDead() const;	//当前角色是否已死亡
```

#### 保护函数

```cpp
void updatePosition(qreal deltaTime);	//根据单帧时间更新角色位置
void updateKnife() const;	//更新角色自身刀圈
void updateEffect();	//更新特殊效果，超时则自动移除
virtual void updateAI(qreal deltaTime) {}	//虚函数，交由Bot类实现具体的AI逻辑
```

#### 保护成员变量

```cpp
bool bot = false;	//是否为Bot
QPointF moveDirection; //速度向量
```

#### 私有类中类

```cpp
class StatusBar :public QGraphicsItem	//实现角色血条以及特殊效果图标的显示
```

#### 私有成员变量

```cpp
int healthy = MaxHealthy;	//当前生命值
int attack = BaseAttack;	//当前攻击力
qreal speed = BaseSpeed; //当前速度，像素每秒
QMap<Effect, qint64> effects;	//当前所持有的特殊效果以及效果的开始时间
KnifeRoll* knifeRoll = nullptr;	//角色真身的刀圈
StatusBar* healthyBar = nullptr;   //角色的状态栏
QMovie* movingAnimation = nullptr; //存储角色的移动动画
QPixmap standingImage;	//存储角色的站立的静态图片
Status status = Status::Standing;	//角色当前状态
```

### Bot类

---

#### 公有函数

```cpp
void updateAI(qreal deltaTime) override;	//用于刷新AI的决策
```

#### 私有函数

```cpp
// AI决策辅助函数
//以生命值是否健康为状态，设置不同的优先级，根据优先级选取距离自身最近的物品
[[nodiscard]] Item* findNearestItem(Effect preferredEffect = Effect::None) const;
[[nodiscard]] Character* findNearestEnemy() const;	//寻找距离自己最近的其他角色
void moveTowards(const QPointF& target, qreal deltaTime);	//朝指定点移动
void fleeFrom(const QPointF& threatPos, qreal deltaTime);	//远离指定点
bool canPerformMeleeAttack(Character* target) const;	//判断是否可对某一角色发起攻击
bool canPerformRangedAttack(Character* target) const;	//判断是否可以对某一角色进行远程攻击
```

#### 私有成员变量

```cpp
QGraphicsScene* scene_ = nullptr;	//存储当前角色所在场景的地址
```

### KnifeRoll类

---

#### 静态成员变量

```cpp
inline static int minRadius = 150;	//刀圈最小半径
inline static int maxRadius = 400;	//刀圈最大半径
```

#### 公有函数

```cpp
void updatePosition(qreal speed); //更新旋转位置
void updateKnife(const qint64 currentTime);	//当总刀数小于四，则每隔一秒增加一柄刀
```

```cpp
void addKnife();	//添加一把刀具
void addKnife(const int count);	//添加指定数量的刀具
void removeKnife();	//移除一把刀具并记录时间
void removeKnife(const int number);	//移除指定数量刀具并记录时间
```

```cpp
[[nodiscard]] int getKnifeCount() const;	//获取刀的数量
[[nodiscard]] qreal getRadius() const;	//获取当前刀圈的半径
```

#### 私有函数

```cpp
void updateKnifePositions(); //更新所有刀具的位置和朝向
```

#### 私有成员变量

```cpp
QList<Knife*> Knifes;	//存储刀具
qreal angle = 0.0;	//当前旋转角度
qreal radius = 150.0;	//旋转半径
QPointF center = { 0, 0 };	//旋转中心
qint64 lastTime = 0;	//最后减少刀具的时间
```

### QuadtreeNode类

---

#### 公有函数

```cpp
[[nodiscard]] std::vector<QRectF> getAllBounds() const;	//获取当前节点以及子节点的区块大小
//查询指定范围内的所有物品
[[nodiscard]] std::vector<QGraphicsItem*> query(const QRectF& range) const;	
[[nodiscard]] QRectF getBound() const;	//获取当前节点的区块大小
```

#### 私有静态常量

```cpp
static const int MAX_OBJECTS = 12; //每个节点最大物体数
static const int MAX_DEPTH = 10;   //最大深度
```

#### 私有成员函数

```cpp
[[nodiscard]] bool isLeaf() const; //判断该节点是否为叶子节点
void subdivide(); //分割节点
[[nodiscard]] std::vector<int> getQuadrant(const QRectF& itemBounds) const; //获取传入区域所属象限
```

#### 私有成员变量

```cpp
QRectF bounds_;		//节点边界
int depth_;			//当前深度
std::vector<QGraphicsItem*> objects_;			//存储的物体
std::unique_ptr<QuadtreeNode> children_[4]; 	//子节点
```

### Quadtree类

---

#### 公有函数

```cpp
void initialize(const QRectF& rect);	//初始化，参数为欲分割的空间的大小
void visualize(QGraphicsScene* scene) const;	//是否显示区块边界
void addItem(QGraphicsItem* item) const;	//向四叉树中添加物品
void clear() const;		//清空四叉树
```

```cpp
//获取指定区块内的所有物体
std::vector<QGraphicsItem*> findItemSectional(const QRectF& rect);
//获取与该物体发送碰撞的所有物体
std::vector<QGraphicsItem*> findCollisions(const QGraphicsItem* item) const;
//获取指定点周围一定范围内的物品（范围为角色远程攻击的半径）
[[nodiscard]] std::vector<QGraphicsItem*> findItemsAroundPoint(const QPointF& point) const;
```

#### 私有成员变量

```cpp
std::unique_ptr<QuadtreeNode> root_;	//存储四叉树根节点
```

## 辅助函数

​	在本项目的文件Headers.h中定义了唯一一个辅助函数，用于获取当前系统时间

```cpp
namespace Tool
{
	inline qreal currentTime()
	{
		auto now = std::chrono::system_clock::now();
		auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
			now.time_since_epoch()
		).count();

		return currentTime;
	}
}
```



1. ` void Character::addEffect(const Effect effect)`：获取特殊效果开始时间
2. `void Character::updateEffect()`：获取当前时间，与效果开始时间相减，判断特殊效果是否结束
3. `void Character::updateKnife() const`：获取当前时间，传给`KnifeRoll`更新刀圈状态
4. `void GameEngine::characterAttackEvent(const CharacterAttackEvent* event)`：获取当前时间，判断当前角色近战攻击是否超过频率每秒五次
5. `void GameEngine::characterRemoteAttackEvent(const CharacterRemoteAttackEvent* event)`：获取当前时间，判断当前角色远程攻击是否超过频率每秒五次
6. `void KnifeRoll::removeKnife()`和`void KnifeRoll::removeKnife(const int number)`：获取当前时间，表示最后减少刀具的时间















​	