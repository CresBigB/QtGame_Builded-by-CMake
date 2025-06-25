#include "GameEngin.h"

GameEngine* GameEngine::instance()
{
	if (engine == nullptr)
		engine = new GameEngine;
	return engine;
}

void GameEngine::update()
{
	if (this->isEnd) return;

	qreal deltaTime = this->lastUpdateTimer.elapsed() / 1000.0; // 时间增量（秒）
	this->lastUpdateTimer.restart();

	this->view->updateAll(deltaTime, Tool::currentTime());
}

void GameEngine::start()
{
	system("cls");
	QCoreApplication::processEvents();
	this->releaseAll();
	this->totalTime.restart();
	this->view->initialize();
	this->view->show();
	this->isEnd = false;
	this->killCount = 0;
	this->timer->start(1000 / GameEngine::FPS);
}

void GameEngine::releaseAll()
{
	this->view->releaseAll();

	this->remoteAttackMap.clear();
	this->attackMap.clear();
}

void GameEngine::customEvent(QEvent* event)
{
	switch (event->type())
	{
	case CharacterMoveEventType:
		this->characterMoveEvent(dynamic_cast<const CharacterMoveEvent*>(event));
		break;
	case CharacterDeadEventType:
		this->characterDeadEvent(dynamic_cast<CharacterDeadEvent*>(event));
		break;
	case CharacterPickUpItemEventType:
		this->characterPickUpItemEvent(dynamic_cast<const CharacterPickUpItemEvent*>(event));
		break;
	case CharacterAttackEventType:
		this->characterAttackEvent(dynamic_cast<const CharacterAttackEvent*>(event));
		break;
	case CharacterRemoteAttackEventType:
		this->characterRemoteAttackEvent(dynamic_cast<const CharacterRemoteAttackEvent*>(event));
		break;
	case GameFinishEventType:
		this->gameFinishEvent(dynamic_cast<const GameFinishEvent*>(event));
		break;
	case StartNewGameEventType:
		this->startNewGameEvent(dynamic_cast<const StartNewGameEvent*>(event));
		break;
	default:;
	}
	QObject::customEvent(event);
}

void GameEngine::characterDeadEvent(const CharacterDeadEvent* event)
{
	auto character = event->character();
	if (character != this->view->getCurrentPlayer())
		this->view->deleteBot(character);
}

void GameEngine::characterMoveEvent(const CharacterMoveEvent* event)
{
	if (event == nullptr) return;

	event->character()->move(event->direction());
}

void GameEngine::characterPickUpItemEvent(const CharacterPickUpItemEvent* event)
{
	DebugInfo << "Pick up item";
	auto item = event->item();
	auto character = event->character();
	switch (item->getType())
	{
	case Item::Type::None:
		break;
	case Item::Type::Knife:
		character->addKnife(1);
		break;
	case Item::Type::Acceleration:
		character->addEffect(Character::Effect::Acceleration);
		break;
	case Item::Type::Invincible:
		character->addEffect(Character::Effect::Invincible);
		break;
	case Item::Type::Heart:
		character->addHealthy(15);
		break;
	}
	this->view->deleteItem(item);
}

void GameEngine::characterAttackEvent(const CharacterAttackEvent* event)
{
	auto currentTime = Tool::currentTime();
	if (this->attackMap.contains(event->character())
		&& currentTime - this->attackMap[event->character()][event->other()] < 200)
		return;

	this->attackMap[event->character()][event->other()] = currentTime;

	event->character()->attackOtherCharacter(event->other());
	event->other()->attackOtherCharacter(event->character());

	if (event->other()->getHealthy() == 0 && this->view->getCurrentPlayer() == event->character())
	{
		this->killCount++;
		DebugInfo << "Kill Count + 1";
	}
}

void GameEngine::characterRemoteAttackEvent(const CharacterRemoteAttackEvent* event)
{
	auto currentTime = Tool::currentTime();
	if (this->remoteAttackMap.contains(event->character())
		&& currentTime - this->remoteAttackMap[event->character()][event->other()] < 200)
		return;

	this->remoteAttackMap[event->character()][event->other()] = currentTime;

	this->view->createKnifeProjectile(event->character(), event->other());
}

void GameEngine::gameFinishEvent(const GameFinishEvent* event)
{
	this->isEnd = true;
	this->timer->stop();
	this->view->showGameOverScreen(this->killCount, this->totalTime.elapsed());
}

void GameEngine::startNewGameEvent(const StartNewGameEvent* event)
{
	this->start();
}

GameEngine::GameEngine(QObject* parent) :QObject(parent)
{
	this->setObjectName("GameEngine");

	this->gameScene = new QGraphicsScene(this);
	this->view = new GameView(this->gameScene);

	this->timer = new QTimer(this);

	this->view->test();

	timer->start(1000 / GameEngine::FPS);
	connect(timer, &QTimer::timeout, this, [this] {this->update(); });
}
