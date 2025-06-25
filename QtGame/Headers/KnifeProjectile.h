#pragma once
#include "Character.h"
#include "Headers.h"

class KnifeProjectile : public Knife
{
public:
	KnifeProjectile(Character* source, Character* target);

	[[nodiscard]] QRectF boundingRect() const override;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
	void updatePos(qreal deltaTime); // 更新飞刀位置
	void attack();

	[[nodiscard]] bool timeout() const;
	[[nodiscard]] bool hasReachedTarget() const; // 检查是否到达目标
	[[nodiscard]] Character* getTarget() const { return target; }

private:
	QPixmap image; // 飞刀图像
	Character* source; // 发射者
	Character* target; // 目标
	qreal speed = 1000; // 速度（像素/秒）
	qreal maxTravelTime = 2.4; // 最大飞行时间（秒）
	qreal elapsedTime = 0.0; // 已飞行时间
};

