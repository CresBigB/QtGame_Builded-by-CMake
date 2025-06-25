#include "Knife.h"

#include "GameEngin.h"

Knife::Knife(QGraphicsItem* parent) :QGraphicsItem(parent)
{
	//this->image = new QPixmap(":/QtGame/Images/knife.png");
	//if (this->image->isNull())
	//	this->image->fill(Qt::GlobalColor::red);
	this->image = QPixmap(":/QtGame/Images/knife.png")
		.scaled(20, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	if (this->image.isNull())
		this->image.fill("#c80fd9");

}

QRectF Knife::boundingRect() const
{
	return { 0,0,20,50 };
}

void Knife::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	painter->save();

	painter->setRenderHint(QPainter::Antialiasing);
	auto rect = this->boundingRect();
	painter->drawPixmap(rect, this->image, rect);

#ifdef  SHOW_BoundingRect
	painter->setPen(Qt::red);
	painter->drawRect(this->boundingRect());
#endif

	painter->restore();
}

KnifeRoll::KnifeRoll(QGraphicsItem* parent) : QGraphicsItem(parent)
{
	for (int i = 0; i < 4; ++i)
		this->addKnife();

	this->updateKnifePositions();
}

KnifeRoll::~KnifeRoll()
{
	for (auto knife : Knifes)
		delete knife;
}

void KnifeRoll::addKnife()
{
	Knife* knife = new Knife(this);
	Knifes.push_back(knife);
	this->updateKnifePositions();
}

void KnifeRoll::addKnife(const int count)
{
	for (int i = 0; i < count; ++i)
	{
		Knife* knife = new Knife(this);
		Knifes.push_back(knife);
	}
	this->updateKnifePositions();
}

void KnifeRoll::removeKnife()
{
	auto currentTime = Tool::currentTime();

	this->lastTime = currentTime;
	if (!Knifes.isEmpty())
	{
		Knife* knife = Knifes.takeLast();
		delete knife;
		this->updateKnifePositions();
	}
}

void KnifeRoll::removeKnife(const int number)
{
	auto currentTime = Tool::currentTime();

	this->lastTime = currentTime;
	for (int i = 0; i > number; --i)
	{
		if (!Knifes.isEmpty())
		{
			Knife* knife = Knifes.takeLast();
			knife->hide();
			delete knife;
			this->updateKnifePositions();
		}
		else break;
	}
}

void KnifeRoll::updateKnife(const qint64 currentTime)
{
	if (currentTime - this->lastTime >= 1000 && this->Knifes.size() < 4)
	{
		this->addKnife();
		this->lastTime = currentTime;
	}
}

void KnifeRoll::updatePosition(const qreal speed)
{
	angle += speed; // 根据速度更新角度
	if (angle >= 360.0) angle -= 360.0; // 保持角度在 0-360 范围内
	this->updateKnifePositions();
}

int KnifeRoll::getKnifeCount() const
{
	return this->Knifes.size();
}

qreal KnifeRoll::getRadius() const
{
	return this->radius;
}

void KnifeRoll::updateKnifePositions()
{
	int numKnives = Knifes.size();
	if (numKnives == 0) return;

	qreal angleStep = 360.0 / numKnives; // 每把刀之间的角度间隔
	qreal currentAngle = angle;

	for (auto knife : Knifes)
	{
		// 计算刀具位置（极坐标转换为直角坐标）
		qreal rad = qDegreesToRadians(currentAngle);
		qreal x = center.x() + radius * cos(rad);
		qreal y = center.y() + radius * sin(rad);
		knife->setPos(x, y);

		knife->setRotation(currentAngle + 90);

		currentAngle += angleStep;
	}

	if (this->Knifes.size() > 10)
		this->radius = minRadius + (this->Knifes.size() - 10) * 3;
	if (this->radius > maxRadius)
		this->radius = maxRadius;
}

QRectF KnifeRoll::boundingRect() const
{
	// 返回一个足够大的矩形，包含所有刀具
	//qreal knifeHeight = 50.0; // Knife 的高度
	//qreal extra = 20.0;       // 边界缓冲区
	//qreal diameter = 2 * (radius + knifeHeight + extra);
	//return QRectF(-radius - knifeHeight - extra, -radius - knifeHeight - extra, diameter, diameter);
	//qreal length = sqrt(this->radius * this->radius * 2) + 50;
	//return { -length / 2,-length / 2,length,length };
	auto length = radius - 10;
	return { -length,-length,length * 2,length * 2 };
}

void KnifeRoll::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
#ifdef  SHOW_BoundingRect
	painter->setPen(QColorConstants::Svg::red);
	painter->drawEllipse(-radius, -radius, radius * 2, radius * 2);
	painter->drawPoint(this->boundingRect().center());
	painter->drawRect(this->boundingRect());
#endif
}
