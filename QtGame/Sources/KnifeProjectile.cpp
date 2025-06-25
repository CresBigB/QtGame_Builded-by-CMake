#include "KnifeProjectile.h"

KnifeProjectile::KnifeProjectile(Character* source, Character* target)
	: Knife(nullptr), source(source), target(target)
{
	image = QPixmap(":/QtGame/Images/knife.png")
		.scaled(20, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	if (image.isNull())
		image.fill("#c80fd9");

	this->setZValue(150);
}

QRectF KnifeProjectile::boundingRect() const
{
	return Knife::boundingRect();
}

void KnifeProjectile::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	Knife::paint(painter, option, widget);
}

void KnifeProjectile::updatePos(qreal deltaTime)
{
	if (!target || elapsedTime >= maxTravelTime)
		return;

	elapsedTime += deltaTime;

	if (!target || elapsedTime >= maxTravelTime)
		return;
	// 计算飞刀朝目标的方向
	QPointF direction = target->pos() - pos();
	qreal distance = sqrt(direction.x() * direction.x() + direction.y() * direction.y());
	if (distance < 10) return;

	// 归一化方向并移动
	direction /= distance;
	QPointF displacement = direction * speed * deltaTime;
	setPos(pos() + displacement);

	// 更新飞刀朝向目标的角度
	qreal angle = qAtan2(direction.y(), direction.x()) * 180 / M_PI;
	setRotation(angle + 90); // 调整飞刀方向
}

void KnifeProjectile::attack()
{
	if (this->hasReachedTarget() || this->elapsedTime == this->maxTravelTime)
	{
		this->hide();

		if (target->getAllEffects().contains(Character::Effect::Invincible))
			return;
		if (target->getKnifeCount())
			target->addKnife(-1);
		else
			target->addHealthy(-source->getAttack());
	}
}

bool KnifeProjectile::timeout() const
{
	return this->elapsedTime >= this->maxTravelTime;
}

bool KnifeProjectile::hasReachedTarget() const
{
	if (!target || elapsedTime >= maxTravelTime)
		return true;

	QPointF direction = target->pos() - this->pos();
	qreal distance = sqrt(direction.x() * direction.x() + direction.y() * direction.y());
	return distance < 10;
}
