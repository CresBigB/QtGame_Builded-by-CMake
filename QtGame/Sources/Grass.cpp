#include "Grass.h"

Grass::Grass(QGraphicsItem* parent)
{
	this->image = QPixmap(":/QtGame/Images/grass.png")
		.scaled(GrassWidth, GrassHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	if (this->image.isNull())
	{
		DebugInfo << "Grass image load failed";
		this->image.fill(QColor("#c80fd9"));
	}
	this->setZValue(1000);
}

QRectF Grass::boundingRect() const
{
	return { -GrassWidth / 2,-GrassHeight / 2,GrassWidth,GrassHeight };
}

void Grass::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	painter->save();

	auto rect = this->boundingRect();
	QRectF sourceRect{ 0, 0, GrassWidth, GrassHeight };
	painter->drawPixmap(rect,
		this->image,
		sourceRect);

#ifdef SHOW_BoundingRect
	painter->setPen(Qt::red);
	painter->drawRect(rect);
#endif

	painter->restore();
}
