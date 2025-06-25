#pragma once

#include "Headers.h"

class Knife :public QGraphicsItem
{
public:
	Knife(QGraphicsItem* parent = nullptr);
	[[nodiscard]] QRectF boundingRect() const override;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:

	QPixmap image;
};

class KnifeRoll : public QGraphicsItem
{
public:
	KnifeRoll(QGraphicsItem* parent = nullptr);
	~KnifeRoll() override;

	inline static int minRadius = 150;
	inline static int maxRadius = 400;

	void addKnife();	// 添加刀具
	void addKnife(const int count);	// 添加刀具
	void removeKnife();	// 移除刀具
	void removeKnife(const int number);	// 移除刀具
	void updateKnife(const qint64 currentTime);

	void updatePosition(qreal speed); // 更新旋转位置
	[[nodiscard]] int getKnifeCount() const;
	[[nodiscard]] qreal getRadius() const;
	[[nodiscard]] QRectF boundingRect() const override;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
	void updateKnifePositions(); // 更新所有刀具的位置和朝向

	QList<Knife*> Knifes;     // 存储刀具
	qreal angle = 0.0;        // 当前旋转角度
	qreal radius = 150.0;      // 旋转半径
	QPointF center = { 0, 0 };  // 旋转中心
	qint64 lastTime = 0;
};
