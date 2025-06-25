#pragma once

#include "Character.h"
#include "Item.h"

class Bot : public Character
{
public:
	Bot(QGraphicsScene* scene, QGraphicsItem* parent = nullptr);

	void updateAI(qreal deltaTime) override;

private:

	QGraphicsScene* scene_ = nullptr;

	// AI决策辅助函数
	[[nodiscard]] Item* findNearestItem(Effect preferredEffect = Effect::None) const;
	[[nodiscard]] Character* findNearestEnemy() const;
	void moveTowards(const QPointF& target, qreal deltaTime);
	void fleeFrom(const QPointF& threatPos, qreal deltaTime);
	bool canPerformMeleeAttack(Character* target) const;
	bool canPerformRangedAttack(Character* target) const;
	void move(const QPointF& direction) override;
};