#pragma once
#ifndef GameView_H
#define GameView_H

#include "Bot.h"
#include "Character.h"
#include "Headers.h"
#include "Grass.h"
#include "Item.h"
#include "KnifeProjectile.h"
#include "Quadtree.h"

class GameView :public QGraphicsView
{
public:
	inline static double activeRadius = 3600;
	inline static int groundWidth = 8000;
	inline static int groundHeight = 8000;
	inline static int grassNumber = 50;
	inline static int BotNumber = 5;

	GameView(QGraphicsScene* scene, QWidget* parent = nullptr);
	~GameView() override;

	void initialize();
	void releaseAll();
	void test();

	void showGameOverScreen(const int killCount, const qreal survivalTime);
	void updateAll(const qreal deltaTime, const qreal currentTime);
	void deleteBot(Character* character);
	void deleteItem(Item* item);
	void setIndicativeLine(const bool hasLine, const QPointF& begin = {}, const QPointF& end = {});
	void createKnifeProjectile(Character* source, Character* target);

	[[nodiscard]] bool inActiveSpace(const QPointF& pos) const;
	[[nodiscard]] const Character* getCurrentPlayer();

	QList<Item*> getAllItem();
	QList<Bot*> getAllBot();
	[[nodiscard]] Character* getNearestCharacter(const Character* character) const;

protected:
	void drawBackground(QPainter* painter, const QRectF& rect) override;
	void drawForeground(QPainter* painter, const QRectF& rect) override;
	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;

	void generateGrass(const int number);
	void generateBot(const int num);
	void generateRandomItem();
	void generateItem(const Item::Type type, const int number);

	void attackableCharacter(Character* character);
	void collisionDetection(Character* character) const;

	void updateCharacters(const qreal deltaTime);
	void updateCollisionManager();
	void updateCharactersCollision();
	void updateCamera();
	void updateProjectiles(qreal deltaTime);
	[[nodiscard]] QPointF getMoveVector(Character::Direction direction) const;

private:
	Quadtree collisionManager;
	QList<Grass*> grasses;
	QList<Item*> items;
	QList<Bot*> bots;
	QList<KnifeProjectile*> projectiles;
	Character* player = nullptr;
	QPointF indicativeLineBegin;
	QPointF indicativeLineEnd;
	bool hasIndicativeLine = false;
};

static const QEvent::Type GameFinishEventType = static_cast<QEvent::Type>(QEvent::User + 9);
class GameFinishEvent : public QEvent
{
public:
	explicit GameFinishEvent()
		: QEvent(GameFinishEventType)
	{}
};

static const QEvent::Type StartNewGameEventType = static_cast<QEvent::Type>(QEvent::User + 10);
class StartNewGameEvent : public QEvent
{
public:
	explicit StartNewGameEvent()
		: QEvent(StartNewGameEventType)
	{}
};

#endif
