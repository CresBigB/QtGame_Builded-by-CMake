#include "GameView.h"

#include "Character.h"
#include "GameEngin.h"
#include "Item.h"

GameView::GameView(QGraphicsScene* scene, QWidget* parent) :QGraphicsView(scene, parent)
{
	QOpenGLWidget* glWidget = new QOpenGLWidget;
	QSurfaceFormat format;
	format.setSamples(4);
	format.setSwapInterval(1);
	format.setProfile(QSurfaceFormat::CoreProfile);
	glWidget->setFormat(format);
	this->setViewport(glWidget);

	this->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
	this->setDragMode(QGraphicsView::NoDrag);
	this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse); // 缩放锚点
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scene->setSceneRect(-GameView::groundWidth / 2, -GameView::groundHeight / 2, GameView::groundWidth, GameView::groundHeight);
}

GameView::~GameView()
{
	for (auto& i : this->grasses)
		delete i;

	delete this->player;
}

void GameView::test()
{
	QShortcut* shortcut1 = new QShortcut(QKeySequence("k"), this);
	shortcut1->setContext(Qt::ApplicationShortcut);
	connect(shortcut1, &QShortcut::activated, this, [this]
		{
			this->player->addKnife(10);
			DebugInfo << "Shortcut activated: Add Knife";
		});

	QShortcut* shortcut2 = new QShortcut(QKeySequence("l"), this);
	shortcut2->setContext(Qt::ApplicationShortcut);
	connect(shortcut2, &QShortcut::activated, this, [this]
		{
			this->player->addKnife(-10);
			DebugInfo << "Shortcut activated: Remove Knife";
		});

	QShortcut* shortcut3 = new QShortcut(QKeySequence("j"), this);
	shortcut3->setContext(Qt::ApplicationShortcut);
	connect(shortcut3, &QShortcut::activated, this, [this]
		{
			this->player->addHealthy(-15);
			DebugInfo << "Shortcut activated: Kill player";
		});

	QShortcut* shortcut4 = new QShortcut(QKeySequence("i"), this);
	shortcut4->setContext(Qt::ApplicationShortcut);
	connect(shortcut4, &QShortcut::activated, this, [this]
		{
			this->generateBot(1);
			DebugInfo << "Shortcut activated: Generate Character";
		});
}

bool GameView::inActiveSpace(const QPointF& pos) const
{
	return sqrt(pos.x() * pos.x() + pos.y() * pos.y()) < GameView::activeRadius;
}

const Character* GameView::getCurrentPlayer()
{
	return this->player;
}

void GameView::updateAll(const qreal deltaTime, const qreal currentTime)
{
	if (this->items.size() < 100)
		this->generateRandomItem();

	this->updateCollisionManager();
	this->updateCharactersCollision();

	this->updateCharacters(deltaTime);
	this->updateProjectiles(deltaTime);

	this->scene()->update();
	this->updateCamera();

	if (this->bots.empty() || this->player->isDead())
		QCoreApplication::postEvent(GameEngine::instance(), new GameFinishEvent);

#ifdef SHOW_BoundingRect
	this->collisionManager.visualize(this->scene());
#endif

}

void GameView::deleteBot(Character* character)
{
	this->bots.removeAll(character);
	//this->lastAttackTimes.remove(character);
	if (character->scene() == this->scene())
		this->scene()->removeItem(character);
}

void GameView::deleteItem(Item* item)
{
	this->items.removeAll(item);
	if (item->scene() == this->scene())
		this->scene()->removeItem(item);
}

void GameView::generateGrass(const int number)
{
	int cnt = 0;
	while (cnt < number)
	{
		double randomX = QRandomGenerator::global()->bounded(-GameView::groundWidth / 2, GameView::groundWidth);
		double randomY = QRandomGenerator::global()->bounded(-GameView::groundHeight / 2, GameView::groundHeight);
		if (this->inActiveSpace({ randomX,randomY }))
		{
			DebugInfo << "Generate grass:" << cnt + 1;

			auto grass = new Grass;
			grass->setPos(randomX, randomY);
			this->scene()->addItem(grass);
			this->grasses.push_back(grass);
			++cnt;
		}
	}
}

void GameView::setIndicativeLine(const bool hasLine, const QPointF& begin, const QPointF& end)
{
	this->indicativeLineBegin = begin;
	this->indicativeLineEnd = end;
	this->hasIndicativeLine = hasLine;
}

void GameView::generateItem(const Item::Type type, const int number)
{
	for (int i = 0; i < number; ++i)
	{
		if (auto item = Item::createItem(type))
		{
			this->items.push_back(item);
			while (true)
			{
				double randomX = QRandomGenerator::global()->bounded(-groundWidth / 2, groundWidth);
				double randomY = QRandomGenerator::global()->bounded(-groundHeight / 2, groundHeight);
				if (this->inActiveSpace({ randomX,randomY }))
				{
					item->setPos(randomX, randomY);
					this->scene()->addItem(item);
					DebugInfo << QString("Generate item:(%1,%2)").arg(randomX).arg(randomY);
					break;
				}
			}
		}
	}
}

void GameView::generateBot(const int num)
{
	for (int i = 0; i < num; ++i)
	{
		auto bot = new Bot(this->scene());
		while (true)
		{
			double randomX = QRandomGenerator::global()->bounded(-GameView::groundWidth / 2, GameView::groundWidth);
			double randomY = QRandomGenerator::global()->bounded(-GameView::groundHeight / 2, GameView::groundHeight);
			if (this->inActiveSpace({ randomX,randomY }))
			{
				bot->setPos(randomX, randomY);
				this->bots.push_back(bot);
				//this->lastAttackTimes[bot] = 0;
				this->scene()->addItem(bot);
				DebugInfo << QString("Generate Character:(%1,%2)").arg(randomX).arg(randomY);
				break;
			}
		}
	}
}

QList<Item*> GameView::getAllItem()
{
	return this->items;
}

QList<Bot*> GameView::getAllBot()
{
	return this->bots;
}

void GameView::initialize()
{
	this->player = new Character;
	this->scene()->addItem(player);
	this->collisionManager.initialize(this->scene()->sceneRect());

	this->generateBot(BotNumber);
	this->generateGrass(grassNumber);
}

void GameView::releaseAll()
{
	for (auto& i : this->grasses)
		this->scene()->removeItem(i);
	for (auto& i : this->items)
		this->scene()->removeItem(i);
	for (auto& i : this->bots)
		this->scene()->removeItem(i);
	for (auto& i : this->projectiles)
		this->scene()->removeItem(i);

	this->items.clear();
	this->bots.clear();
	this->collisionManager.clear();
	this->grasses.clear();
	this->projectiles.clear();
	delete this->player;
	this->player = nullptr;
}

void GameView::showGameOverScreen(const int killCount, const qreal survivalTime)
{
	// 添加半透明背景矩形
	QGraphicsRectItem* bgRect = new QGraphicsRectItem(-350, -200, 700, 400);
	bgRect->setBrush(QColor(0, 0, 0, 180)); // 黑色，70%透明度
	this->scene()->addItem(bgRect);

	// 创建显示排名、击杀数和存活时间的文本
	QGraphicsTextItem* text = new QGraphicsTextItem(this->bots.empty() ? "胜利！" : "失败");
	QGraphicsTextItem* rankText = new QGraphicsTextItem(QString("排名: %1").arg(this->bots.size() + 1));
	QGraphicsTextItem* killText = new QGraphicsTextItem(QString("击杀数: %1").arg(killCount));
	QGraphicsTextItem* timeText = new QGraphicsTextItem(QString("存活时间: %1 秒").arg(survivalTime / 1000));

	// 设置字体和颜色以提高可读性
	QFont font("Arial", 24, QFont::Bold);
	text->setFont(font);
	rankText->setFont(font);
	killText->setFont(font);
	timeText->setFont(font);
	text->setDefaultTextColor(Qt::white);
	rankText->setDefaultTextColor(Qt::white);
	killText->setDefaultTextColor(Qt::white);
	timeText->setDefaultTextColor(Qt::white);

	// 垂直居中排列文本
	qreal y = -120;
	text->setPos(-text->boundingRect().width() / 2, y);
	y += 50;
	rankText->setPos(-rankText->boundingRect().width() / 2, y);
	y += 50;
	killText->setPos(-killText->boundingRect().width() / 2, y);
	y += 50;
	timeText->setPos(-timeText->boundingRect().width() / 2, y);

	// 添加文本到场景
	this->scene()->addItem(text);
	this->scene()->addItem(rankText);
	this->scene()->addItem(killText);
	this->scene()->addItem(timeText);

	// 创建“返回主界面”按钮
	QPushButton* mainMenuButton = new QPushButton("返回主界面");
	mainMenuButton->setFixedSize(200, 50);
	QGraphicsProxyWidget* mainMenuProxy = this->scene()->addWidget(mainMenuButton);
	mainMenuProxy->setPos(-100, y + 50); // 放置在文本下方

	// 创建“开启新游戏”按钮
	QPushButton* newGameButton = new QPushButton("开启新游戏");
	newGameButton->setFixedSize(200, 50);
	QGraphicsProxyWidget* newGameProxy = this->scene()->addWidget(newGameButton);
	newGameProxy->setPos(-100, y + 110); // 放置在“返回主界面”按钮下方

	// 设置层级，确保文本和按钮在背景之上
	bgRect->setZValue(9000);
	text->setZValue(10000);
	rankText->setZValue(10000);
	killText->setZValue(10000);
	timeText->setZValue(10000);
	mainMenuProxy->setZValue(10000);
	newGameProxy->setZValue(10000);

	// 连接按钮信号到槽函数
	connect(mainMenuButton, &QPushButton::clicked, this,
		[this, bgRect, text, rankText, killText, timeText, mainMenuProxy, newGameProxy]
		{
			this->hide();
			emit GameEngine::instance()->backToMenu();
			bgRect->hide();
			delete bgRect;
			text->deleteLater();
			rankText->deleteLater();
			killText->deleteLater();
			timeText->deleteLater();
			mainMenuProxy->deleteLater();
			newGameProxy->deleteLater();
		});
	connect(newGameButton, &QPushButton::clicked, this, [=]
		{
			QCoreApplication::postEvent(GameEngine::instance(), new StartNewGameEvent);
			bgRect->hide();
			delete bgRect;
			text->deleteLater();
			rankText->deleteLater();
			killText->deleteLater();
			timeText->deleteLater();
			mainMenuProxy->deleteLater();
			newGameProxy->deleteLater();
		});

	// 将视图居中于游戏结束界面
	this->centerOn(0, 0);
}

void GameView::drawBackground(QPainter* painter, const QRectF& rect)
{
	painter->save();

	painter->setRenderHint(QPainter::Antialiasing);
	painter->fillRect(rect, QColor(100, 100, 100, 100));
	painter->setBrush(QColor(240, 240, 240));
	painter->setPen(Qt::NoPen);
	painter->drawEllipse(QPointF(0, 0), GameView::activeRadius, GameView::activeRadius);

	// 绘制网格
	painter->setPen(QPen(Qt::gray, 1.0));
	qreal gridSize = Item::ItemHeight;
	qreal left = qRound(rect.left() / gridSize) * gridSize;
	qreal top = qRound(rect.top() / gridSize) * gridSize;

	for (qreal x = left; x <= rect.right(); x += gridSize)
		painter->drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
	for (qreal y = top; y <= rect.bottom(); y += gridSize)
		painter->drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));

	painter->restore();
}

void GameView::drawForeground(QPainter* painter, const QRectF& rect)
{
	if (!hasIndicativeLine) return;

	painter->save();
	painter->setPen(QPen(QBrush("#e33d51"), 10, Qt::DashLine));
	painter->drawLine(indicativeLineBegin, indicativeLineEnd);
	painter->restore();
}

void GameView::keyPressEvent(QKeyEvent* event)
{
	if (event->isAutoRepeat()) return;
	using Direction = Character::Direction;
	switch (event->key())
	{
	case Qt::Key_W:
		QCoreApplication::postEvent(GameEngine::instance(), new CharacterMoveEvent(this->player,
			getMoveVector(Direction::Up)));
		break;
	case Qt::Key_A:
		QCoreApplication::postEvent(GameEngine::instance(), new CharacterMoveEvent(this->player,
			getMoveVector(Direction::Left)));
		break;
	case Qt::Key_S:
		QCoreApplication::postEvent(GameEngine::instance(), new CharacterMoveEvent(this->player,
			getMoveVector(Direction::Down)));
		break;
	case Qt::Key_D:
		QCoreApplication::postEvent(GameEngine::instance(), new CharacterMoveEvent(this->player,
			getMoveVector(Direction::Right)));
		break;

	default:break;
	}
}

void GameView::keyReleaseEvent(QKeyEvent* event)
{
	if (event->isAutoRepeat()) return;
	using Direction = Character::Direction;
	switch (event->key())
	{
	case Qt::Key_W:
		QCoreApplication::postEvent(GameEngine::instance(), new CharacterMoveEvent(this->player,
			getMoveVector(Direction::Down)));
		break;
	case Qt::Key_A:
		QCoreApplication::postEvent(GameEngine::instance(), new CharacterMoveEvent(this->player,
			getMoveVector(Direction::Right)));
		break;
	case Qt::Key_S:
		QCoreApplication::postEvent(GameEngine::instance(), new CharacterMoveEvent(this->player,
			getMoveVector(Direction::Up)));
		break;
	case Qt::Key_D:
		QCoreApplication::postEvent(GameEngine::instance(), new CharacterMoveEvent(this->player,
			getMoveVector(Direction::Left)));
		break;
	default:break;
	}
}

void GameView::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (auto other = this->getNearestCharacter(this->player))
		{
			if (other && !other->isDead())
				QCoreApplication::postEvent(GameEngine::instance(),
					new CharacterRemoteAttackEvent(this->player, other));
		}
	}
	QGraphicsView::mousePressEvent(event);
}

void GameView::generateRandomItem()
{
	int type = QRandomGenerator::global()->bounded(1, 100);

	if (type < 60)
		this->generateItem(Item::Type::Knife, 1);
	else if (type < 75)
		this->generateItem(Item::Type::Heart, 1);
	else if (type < 95)
		this->generateItem(Item::Type::Acceleration, 1);
	else
		this->generateItem(Item::Type::Invincible, 1);
}

void GameView::updateCharacters(const qreal deltaTime)
{
	this->player->updateCharacter(deltaTime);
	if (auto nearestCharacter = this->getNearestCharacter(this->player))
		this->setIndicativeLine(true, player->pos(), nearestCharacter->pos());
	else
		this->setIndicativeLine(false);

	this->attackableCharacter(this->player);

	for (const auto& i : this->bots)
	{
		//i->updateCharacter(deltaTime);
		i->updateAI(deltaTime);
		this->attackableCharacter(i);
	}
}

void GameView::updateCollisionManager()
{
	this->collisionManager.clear();

	this->collisionManager.addItem(this->player);

	for (const auto& i : this->items)
		this->collisionManager.addItem(i);

	for (const auto& i : this->bots)
		this->collisionManager.addItem(i);
}

void GameView::updateCamera()
{
	this->update();
	// 平滑过渡到玩家位置
	QPointF currentCenter = this->mapToScene(this->viewport()->rect().center());
	QPointF targetCenter = this->player->pos();
	QPointF newCenter = currentCenter + (targetCenter - currentCenter) * 0.1; // 插值平滑
	this->centerOn(newCenter);
}

void GameView::updateProjectiles(const qreal deltaTime)
{
	for (const auto& i : this->projectiles)
	{
		i->updatePos(deltaTime);
		if (i->hasReachedTarget())
		{
			i->attack();
			this->projectiles.removeAll(i);
		}
		if (i->scene() == this->scene() && i->timeout())
			this->scene()->removeItem(i);
	}
}

void GameView::collisionDetection(Character* character) const
{
	if (!character) return;
	auto items = this->collisionManager.findCollisions(character);
	for (const auto i : items)
	{
		if (auto item = dynamic_cast<Item*>(i))
		{
			QCoreApplication::postEvent(GameEngine::instance(),
				new CharacterPickUpItemEvent(character, item));
		}
	}
}


void GameView::updateCharactersCollision()
{
	this->collisionDetection(this->player);
	for (const auto& i : this->bots)
		this->collisionDetection(i);
}

void GameView::attackableCharacter(Character* character)
{
	for (const auto& other : this->bots)
	{
		if (other != character)
		{
			qreal length = std::sqrt(std::pow(other->x() - character->x(), 2)
				+ std::pow(other->y() - character->y(), 2));

			if (length < character->getAttackRadius() + other->getAttackRadius() && !other->isDead())
			{
				QCoreApplication::postEvent(GameEngine::instance(),
					new CharacterAttackEvent(character, other));
			}
		}
	}
}

void GameView::createKnifeProjectile(Character* source, Character* target)
{
	if (!source || !target || source->getKnifeCount() == 0)
		return;

	// 从发射者的KnifeRoll移除一把刀
	source->addKnife(-1);

	// 创建并添加新的飞刀
	auto projectile = new KnifeProjectile(source, target);
	projectile->setPos(source->pos());
	this->scene()->addItem(projectile);
	projectiles.push_back(projectile);
}

QPointF GameView::getMoveVector(const Character::Direction direction) const
{
	using Direction = Character::Direction;
	QPointF vector;
	switch (direction)
	{
	case Direction::Up:    vector = { 0, -1 }; break;
	case Direction::Down:  vector = { 0, 1 }; break;
	case Direction::Left:  vector = { -1, 0 }; break;
	case Direction::Right: vector = { 1, 0 }; break;
	}
	return vector;
}

Character* GameView::getNearestCharacter(const Character* character) const
{
	auto items = collisionManager.findItemsAroundPoint(character->pos());
	// 筛选角色并找到最近的一个
	Character* nearestCharacter = nullptr;
	qreal minDistance = std::numeric_limits<qreal>::max();

	for (QGraphicsItem* item : items)
	{
		if (Character* result = dynamic_cast<Character*>(item))
		{
			if (result != character && !result->isDead()) // 排除玩家自身
			{
				QPointF vector = result->pos() - character->pos();
				qreal distance = sqrt(vector.x() * vector.x() + vector.y() * vector.y());
				if (distance < minDistance && distance <= Character::MaxRemoteRadius)
				{
					minDistance = distance;
					nearestCharacter = result;
				}
			}
		}
	}

	return nearestCharacter;
}
