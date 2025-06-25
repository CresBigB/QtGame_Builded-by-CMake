#pragma once
#ifndef GameEngine_H
#define  GameEngine_H
#include "Character.h"
#include "Headers.h"
#include "GameView.h"
#include "Item.h"
#include "Quadtree.h"

class GameEngine :public QObject
{
	Q_OBJECT
public:
	GameEngine(const GameEngine&) = delete;
	GameEngine& operator=(const GameEngine&) = delete;

	inline static int FPS = 120;

	static GameEngine* instance();

	void start();
	void releaseAll();

signals:
	void backToMenu();

protected:
	void customEvent(QEvent* event) override;

	void update();
	void characterDeadEvent(const CharacterDeadEvent* event);
	void characterMoveEvent(const CharacterMoveEvent* event);
	void characterPickUpItemEvent(const CharacterPickUpItemEvent* event);
	void characterAttackEvent(const CharacterAttackEvent* event);
	void characterRemoteAttackEvent(const CharacterRemoteAttackEvent* event);
	void gameFinishEvent(const GameFinishEvent* event);
	void startNewGameEvent(const StartNewGameEvent* event);

private:
	GameEngine(QObject* parent = nullptr);
	inline static GameEngine* engine = nullptr;

	bool isEnd = true;
	int killCount = 0;

	QTimer* timer = nullptr;
	QElapsedTimer lastUpdateTimer;
	QElapsedTimer totalTime;

	GameView* view = nullptr;
	QGraphicsScene* gameScene = nullptr;

	QMap<Character*, QMap<Character*, qreal>> remoteAttackMap;
	QMap<Character*, QMap<Character*, qreal>> attackMap;
};
#endif
