#include "Bot.h"
#include "GameEngin.h"
#include <cmath>

Bot::Bot(QGraphicsScene* scene, QGraphicsItem* parent) : Character(parent), scene_(scene)
{
	this->bot = true;
}

//void Bot::updateAI(qreal deltaTime)
//{
//	this->updateCharacter(deltaTime);
//	if (this->isDead()) return;
//
//	// 获取GameView以访问道具和角色
//	GameView* gameView = dynamic_cast<GameView*>(scene_->views().isEmpty() ? nullptr : scene_->views().first());
//	if (!gameView)
//	{
//		DebugInfo << "Bot: 未找到GameView";
//		return;
//	}
//
//	// 根据生命值决定行为
//	if (this->getHealthy() > 50)
//	{
//		// 健康状态：优先拾取加速/无敌道具，其次飞刀，主动攻击
//		if (Item* targetItem = findNearestItem())		// 朝最近的道具移动
//		{
//			moveTowards(targetItem->pos(), deltaTime);
//		}
//		else // 无道具，寻找敌人攻击
//		{
//			Character* targetEnemy = gameView->getNearestCharacter(this);
//			if (targetEnemy && !targetEnemy->isDead()) // 检查飞刀数量条件
//			{
//				int ownKnives = this->getKnifeCount();
//				int enemyKnives = targetEnemy->getKnifeCount();
//				if (ownKnives - enemyKnives > 5)
//				{
//					if (canPerformRangedAttack(targetEnemy))	// 远程攻击
//					{
//						QCoreApplication::postEvent(GameEngine::instance(),
//							new CharacterRemoteAttackEvent(this, targetEnemy));
//					}
//					else if (canPerformMeleeAttack(targetEnemy)) // 近战攻击（由GameView::getAttackableCharacter处理）
//					{
//						moveTowards(targetEnemy->pos(), deltaTime);
//					}
//					else
//						fleeFrom(targetEnemy->pos(), deltaTime);
//				}
//				//else // 朝敌人移动但不攻击
//				//{
//				//	moveTowards(targetEnemy->pos(), deltaTime);
//				//}
//			}
//			else	// 无敌人或道具，进入闲置状态
//			{
//				this->setStatus(Status::Standing);
//			}
//		}
//	}
//	else
//	{
//		// 不健康状态：优先逃跑并拾取心形道具
//		Item* targetItem = findNearestItem(Character::Effect::None); // 优先心形
//		if (targetItem && targetItem->getType() == Item::Type::Heart)
//		{
//			moveTowards(targetItem->pos(), deltaTime);
//		}
//		else
//		{
//			Character* nearestEnemy = findNearestEnemy();
//			if (nearestEnemy && !nearestEnemy->isDead())
//			{
//				fleeFrom(nearestEnemy->pos(), deltaTime);
//			}
//			else if (targetItem)
//			{
//				// 无心形道具但有其他道具，移动拾取
//				moveTowards(targetItem->pos(), deltaTime);
//			}
//			else
//			{
//				// 无威胁或道具，闲置
//				this->setStatus(Status::Standing);
//			}
//		}
//	}
//}

void Bot::updateAI(qreal deltaTime)
{
	this->updateCharacter(deltaTime);
	if (this->isDead()) return;

	// 获取GameView以访问道具和角色
	GameView* gameView = dynamic_cast<GameView*>(scene_->views().isEmpty() ? nullptr : scene_->views().first());
	if (!gameView)
	{
		DebugInfo << "Bot: 未找到GameView";
		return;
	}

	// 根据生命值决定行为
	if (this->getHealthy() > 50)
	{
		Character* targetEnemy = gameView->getNearestCharacter(this);
		if (canPerformRangedAttack(targetEnemy))	// 远程攻击
		{
			QCoreApplication::postEvent(GameEngine::instance(),
				new CharacterRemoteAttackEvent(this, targetEnemy));
		}
		if (canPerformMeleeAttack(targetEnemy)) // 近战攻击（由GameView::getAttackableCharacter处理）
			moveTowards(targetEnemy->pos(), deltaTime);

		// 健康状态：优先拾取加速/无敌道具，其次飞刀，主动攻击
		if (Item* targetItem = findNearestItem())		// 朝最近的道具移动
			moveTowards(targetItem->pos(), deltaTime);
		else	// 无敌人或道具，进入闲置状态
			this->setStatus(Status::Standing);
	}
	else
	{
		// 不健康状态：优先逃跑并拾取心形道具
		Item* targetItem = findNearestItem(Effect::None); // 优先心形
		if (targetItem && targetItem->getType() == Item::Type::Heart)
			moveTowards(targetItem->pos(), deltaTime);
		else
		{
			Character* nearestEnemy = findNearestEnemy();
			if (nearestEnemy && !nearestEnemy->isDead())
				fleeFrom(nearestEnemy->pos(), deltaTime);
			else if (targetItem) // 无心形道具但有其他道具，移动拾取
				moveTowards(targetItem->pos(), deltaTime);
			else // 无威胁或道具，闲置
				this->setStatus(Status::Standing);
		}
	}
}

Item* Bot::findNearestItem(const Effect preferredEffect) const
{
	Item* nearestItem = nullptr;
	qreal minDistance = std::numeric_limits<qreal>::max();
	bool preferHeart = (this->getHealthy() <= 50);

	// 从GameView获取道具
	GameView* gameView = dynamic_cast<GameView*>(scene_->views().isEmpty() ? nullptr : scene_->views().first());
	if (!gameView) return nullptr;

	for (Item* item : gameView->getAllItem())
	{
		if (!item || !item->scene()) continue;

		// 根据生命值和优先效果决定道具选择
		bool isPreferred = false;
		if (preferHeart && item->getType() == Item::Type::Heart)
			isPreferred = true;
		else if (!preferHeart)
		{
			if (preferredEffect == Effect::Invincible &&
				item->getType() == Item::Type::Invincible)
			{

				isPreferred = true;
			}
			else if (item->getType() == Item::Type::Knife && this->getKnifeCount() <= 15)
				isPreferred = (preferredEffect == Character::Effect::None);
		}

		if (!preferHeart && item->getType() == Item::Type::Heart)
			continue; // 健康时忽略心形道具

		QPointF vector = item->pos() - this->pos();
		qreal distance = sqrt(vector.x() * vector.x() + vector.y() * vector.y());
		if (distance < minDistance && (isPreferred || item->getType() != Item::Type::None))
		{
			minDistance = distance;
			nearestItem = item;
		}
	}

	return nearestItem;
}

Character* Bot::findNearestEnemy() const
{
	GameView* gameView = dynamic_cast<GameView*>(scene_->views().isEmpty() ? nullptr : scene_->views().first());
	if (!gameView) return nullptr;

	Character* nearestEnemy = nullptr;
	qreal minDistance = std::numeric_limits<qreal>::max();

	// 检查玩家和其他Bot
	if (!gameView->getCurrentPlayer()->isDead())
	{
		QPointF vector = gameView->getCurrentPlayer()->pos() - this->pos();
		qreal distance = sqrt(vector.x() * vector.x() + vector.y() * vector.y());
		if (distance < minDistance)
		{
			minDistance = distance;
			nearestEnemy = const_cast<Character*>(gameView->getCurrentPlayer());
		}
	}

	for (Bot* bot : gameView->getAllBot())
	{
		if (bot != this && !bot->isDead())
		{
			QPointF vector = bot->pos() - this->pos();
			qreal distance = sqrt(vector.x() * vector.x() + vector.y() * vector.y());
			if (distance < minDistance)
			{
				minDistance = distance;
				nearestEnemy = bot;
			}
		}
	}

	return nearestEnemy;
}

void Bot::moveTowards(const QPointF& target, qreal deltaTime)
{
	QPointF vector = target - this->pos();
	qreal distance = sqrt(vector.x() * vector.x() + vector.y() * vector.y());
	// 避免微小抖动
	if (distance > 1.0)
	{
		vector /= distance; // 归一化
		this->move(vector);
		this->updatePosition(deltaTime);
	}
	else
		this->setStatus(Status::Standing);
}

void Bot::fleeFrom(const QPointF& threatPos, qreal deltaTime)
{
	QPointF vector = this->pos() - threatPos;
	qreal distance = sqrt(vector.x() * vector.x() + vector.y() * vector.y());
	if (distance < 1050.0)
	{ // 敌人进入攻击范围时逃跑
		vector /= distance; // 归一化
		this->move(vector);
		this->updatePosition(deltaTime);
	}
	else
		this->setStatus(Status::Standing);
}

bool Bot::canPerformMeleeAttack(Character* target) const
{
	if (!target || target->getAllEffects().contains(Effect::Invincible))
		return false;

	return this->getKnifeCount() - target->getKnifeCount() >= 3;
}

bool Bot::canPerformRangedAttack(Character* target) const
{
	if (!target || this->getKnifeCount() == 0
		|| target->getAllEffects().contains(Effect::Invincible)
		|| this->getKnifeCount() - target->getKnifeCount() < 5)
		return false;

	qreal distance = sqrt(pow(target->x() - this->x(), 2) + pow(target->y() - this->y(), 2));
	return distance <= MaxRemoteRadius - 100;
}

void Bot::move(const QPointF& direction)
{
	qreal length = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());
	this->moveDirection = (length > 0) ? direction / length * 0.35 : QPointF{ 0, 0 };

	if (this->moveDirection != QPointF{ 0,0 })
	{
		this->setStatus(Status::Moving);
	}
	else
	{
		this->moveDirection = { 0, 0 };
		this->setStatus(Status::Standing);
	}
}
