#include "Character.h"
#include "GameEngin.h"

Character::Character(QGraphicsItem* parent) :QGraphicsItem(parent)
{
	this->standingImage = QPixmap(":/QtGame/Images/standing.png")
		.scaled(CharacterWidth, CharacterHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	this->movingAnimation = new QMovie(":/QtGame/Images/moving.gif");
	this->movingAnimation->setScaledSize(QSize(CharacterWidth, CharacterHeight));
	this->movingAnimation->setSpeed(100 * GameEngine::FPS / 60);
	this->healthyBar = new StatusBar(this);
	this->knifeRoll = new KnifeRoll(this);
	this->setZValue(100);
}

Character::~Character()
{
	delete this->movingAnimation;
	delete this->knifeRoll;
}

void Character::updateCharacter(const qreal deltaTime)
{
	if (this->status == Status::Dying)
	{
		// 更新死亡动画
		this->deathAnimationTime += deltaTime;
		//this->opacity = 0.5;
		this->opacity = 1.0 - (this->deathAnimationTime / DeathAnimationDuration); // 线性减少透明度
		//DebugInfo << this->opacity;
		if (this->opacity <= 0.0 && this->deathAnimationTime >= DeathAnimationDuration)
		{
			this->opacity = 0.0;
			this->status = Status::Dead;
			//this->setVisible(false); // 动画结束，隐藏角色
			QCoreApplication::postEvent(GameEngine::instance(), new CharacterDeadEvent(this));
		}
		//this->update(); // 触发重绘以应用新的透明度
		return;
	}

	this->updatePosition(deltaTime);
	this->updateKnife();
	this->updateEffect();
	this->knifeRoll->updatePosition(360 * deltaTime);
}

QRectF Character::boundingRect() const
{
	return { -CharacterWidth / 2,-CharacterHeight / 2,CharacterWidth,CharacterHeight };
}

void Character::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	painter->save();
#ifdef  SHOW_BoundingRect
	painter->setPen(Qt::red);
	painter->drawRect(this->boundingRect());
#endif

	painter->setRenderHint(QPainter::Antialiasing);
	QRectF sourceRect{ 0, 0, CharacterWidth, CharacterHeight };

	painter->setOpacity(this->opacity); // 应用透明度

	QRectF targetRect = this->boundingRect();
	// 检查是否向左移动（moveDirection.x() < 0），进行水平翻转
	if (moveDirection.x() < 0)
	{
		painter->translate(targetRect.center());
		painter->scale(-1, 1); // 水平翻转
		painter->translate(-targetRect.center());
	}

	if (status == Status::Standing || status == Status::Dying)
	{
		painter->drawPixmap(targetRect,
			this->standingImage,
			sourceRect);
	}
	else
	{
		painter->drawPixmap(targetRect,
			this->movingAnimation->currentPixmap(),
			sourceRect);
	}

	// 恢复画笔状态，避免影响其他绘制
	painter->restore();
}

void Character::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	DebugInfo << "Clicked Character Pos:" << this->pos();
}

void Character::addEffect(const Effect effect)
{
	auto currentTime = Tool::currentTime();
	this->effects[effect] = currentTime;

	if (effect == Effect::Acceleration)
	{
		DebugInfo << __FUNCTION__ << "Add Effect :Acceleration" << " || Time" << currentTime;
		this->speed = BaseSpeed * 1.3;
	}
}

void Character::updateEffect()
{
	auto currentTime = Tool::currentTime();

	auto i = this->effects.begin();
	while (i != this->effects.end())
	{
		if (currentTime - i.value() >= 5000)
		{
			if (i.key() == Effect::Acceleration)
			{
				DebugInfo << __FUNCTION__ << "Remove Effect :Acceleration";
				this->speed = 500;
			}
			i = this->effects.erase(i);
		}
		if (i == this->effects.end())
			break;
		++i;
	}
}

void Character::setStatus(const Status status)
{
	this->status = status;
	if (this->status == Status::Standing)
	{
		this->moveDirection = { 0,0 };
		this->movingAnimation->jumpToFrame(0);
		this->movingAnimation->stop();
		this->update();
	}
	else if (this->status == Status::Moving)
		this->movingAnimation->start();
}

void Character::addKnife(const int number) const
{
	if (number > 0)
		this->knifeRoll->addKnife(number);
	else if (number < 0)
		this->knifeRoll->removeKnife(number);
}

void Character::updateKnife() const
{
	auto currentTime = Tool::currentTime();

	this->knifeRoll->updateKnife(currentTime);
}

void Character::attackOtherCharacter(Character* character)
{
	if (character->getAllEffects().contains(Effect::Invincible) || character->isDead())
		return;

	if (character->knifeRoll->getKnifeCount() && this->knifeRoll->getKnifeCount())
	{
		character->addKnife(-1);
	}
	else if (this->knifeRoll->getKnifeCount())
		character->addHealthy(-this->attack);
}

void Character::move(const QPointF& direction)
{
	this->moveDirection += direction;

	if (this->moveDirection != QPointF{ 0,0 })
		this->setStatus(Status::Moving);
	else
	{
		this->moveDirection = { 0, 0 };
		this->setStatus(Status::Standing);
	}
}

void Character::updatePosition(const qreal deltaTime)
{
	// 计算新位置
	QPointF displacement = moveDirection * speed * deltaTime;
	QPointF newPos = this->pos() + displacement;

	// 获取 GameView 的 activeRadius 和圆心
	qreal activeRadius = GameView::activeRadius;
	QPointF center = { 0,0 };

	// 计算新位置到圆心的距离
	QPointF vectorToCenter = newPos - center;
	qreal distance = sqrt(vectorToCenter.x() * vectorToCenter.x() + vectorToCenter.y() * vectorToCenter.y());

	// 如果超出 activeRadius，限制到圆边界
	if (distance > activeRadius)
	{
		// 归一化向量并缩放到 activeRadius
		vectorToCenter /= distance; // 归一化
		newPos = center + vectorToCenter * activeRadius;
	}

	// 更新位置
	this->setPos(newPos);
}

void Character::addHealthy(const int num)
{
	this->healthy += num;
	if (this->healthy >= MaxHealthy)
		this->healthy = MaxHealthy;
	else if (this->healthy <= 0 && !this->isDead())
	{
		this->healthy = 0;
		this->status = Status::Dying;
	}
}
