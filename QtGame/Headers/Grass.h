#pragma once

#include "Headers.h"

class Grass :public QGraphicsItem
{
public:
	inline static qreal GrassWidth = 240;
	inline static qreal GrassHeight = 240;

	Grass(QGraphicsItem* parent = nullptr);

	[[nodiscard]] QRectF boundingRect() const override;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

protected:
	QPixmap image;
};

