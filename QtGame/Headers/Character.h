#pragma once
#ifndef Character_H
#define Character_H

#include "Headers.h"
#include "Knife.h"

class Character :public QGraphicsItem
{
public:
	enum class Status :quint8 { Moving, Standing, Dead, Dying };
	enum class Effect :quint8 { None, Acceleration, Invincible };
	enum class Direction :qint8 { Up, Down, Left, Right };

	inline static double CharacterWidth = 150;
	inline static double CharacterHeight = 150;

	inline static int MaxHealthy = 100;
	inline static int BaseAttack = 5;
	inline static int MaxRemoteRadius = 650;
	inline static qreal BaseSpeed = 700; //像素每秒
	inline static qreal DeathAnimationDuration = 0.5; // 死亡动画持续时间（秒）

	Character(QGraphicsItem* parent = nullptr);
	~Character() override;

	void updateCharacter(const qreal deltaTime);
	virtual void move(const QPointF& direction);

public:
	[[nodiscard]] QRectF boundingRect() const override;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

	void addHealthy(const int num);
	void addEffect(Effect effect);
	void addKnife(const int number) const;

	void attackOtherCharacter(Character* character);

	[[deprecated("请改用getAttackRadius获取攻击半径")]]
	[[nodiscard]] QRectF getAttackBoundingRect() const
	{
		return this->knifeRoll->boundingRect();
	}

	[[nodiscard]] int getHealthy() const
	{
		return healthy;
	}
	[[nodiscard]] qreal getSpeed() const
	{
		return this->speed;
	}
	[[nodiscard]] int getAttack() const
	{
		return this->attack;
	}
	[[nodiscard]] qreal getAttackRadius() const
	{
		return this->knifeRoll->getRadius();
	}
	[[nodiscard]] QList<Effect> getAllEffects() const
	{
		return this->effects.keys();
	}
	[[nodiscard]] int getKnifeCount() const
	{
		return this->knifeRoll->getKnifeCount();
	}
	[[nodiscard]] bool isBot() const
	{
		return this->bot;
	}
	[[nodiscard]] bool isDead() const
	{
		return this->status == Status::Dead || this->status == Status::Dying;
	}

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

	void updatePosition(qreal deltaTime);
	void updateKnife() const;
	void updateEffect();
	virtual void updateAI(qreal deltaTime) {}
	void setStatus(Status status);

	bool bot = false;
	QPointF moveDirection; //速度向量

private:
	class StatusBar :public QGraphicsItem
	{
	public:
		explicit StatusBar(Character* parent) :QGraphicsItem(parent), parent(parent)
		{}

		[[nodiscard]] QRectF boundingRect() const override
		{
			return {};
		}

		void drawEffectIcon(QPainter* painter, const QList<Effect>& effects) const
		{
			QPixmap icon(QSize(20, 20));
			QRectF rect{ parent->boundingRect().x(), parent->boundingRect().y(), CharacterWidth, 15 };

			for (int i = 0; i < effects.size(); ++i)
			{
				switch (effects[i])
				{
				case Effect::None:
					icon.fill("#c80fd9");
					break;
				case Effect::Acceleration:
					icon.load(":/QtGame/Images/Acceleration.png");
					break;
				case Effect::Invincible:
					icon.load(":/QtGame/Images/infinite.png");
					break;
				}
				painter->drawPixmap(rect.x() + i * 35, rect.y(),
					icon.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation));
			}
		}

		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override
		{
			painter->save();
			painter->setRenderHint(QPainter::Antialiasing);
			this->drawEffectIcon(painter, parent->getAllEffects());
			int healthy = parent->getHealthy();
			painter->setPen(QColor("#2b5cd9"));
			painter->setBrush(QBrush("#d9237b"));
			QRectF rect{ parent->boundingRect().x(), parent->boundingRect().y(), CharacterWidth, 15 };
			painter->drawRoundedRect(rect.x(), rect.y() - 22,
				rect.width() - rect.width() * (MaxHealthy - healthy) / MaxHealthy, rect.height(), 5, 5);
			painter->restore();
		}
	private:
		Character* parent = nullptr;
	};

	int healthy = MaxHealthy;
	int attack = BaseAttack;
	qreal speed = BaseSpeed; //像素每秒
	QMap<Effect, qint64> effects;
	KnifeRoll* knifeRoll = nullptr;
	StatusBar* healthyBar = nullptr;
	QMovie* movingAnimation = nullptr;
	QPixmap standingImage;
	Status status = Status::Standing;
	qreal opacity = 1.0; // 透明度，1.0 为不透明，0.0 为完全透明
	qreal deathAnimationTime = 0.0; // 死亡动画已进行的时间
};

static const QEvent::Type CharacterDeadEventType = static_cast<QEvent::Type>(QEvent::User + 1);
class CharacterDeadEvent :public QEvent
{
public:
	CharacterDeadEvent(Character* character) :QEvent(CharacterDeadEventType), deadCharacter(character) {}

	[[nodiscard]] Character* character() const
	{
		return this->deadCharacter;
	}

private:
	Character* deadCharacter = nullptr;
};

static const QEvent::Type CharacterMoveEventType = static_cast<QEvent::Type>(QEvent::User + 4);
class CharacterMoveEvent : public QEvent
{
public:
	CharacterMoveEvent(Character* character, const QPointF& direction)
		: QEvent(CharacterMoveEventType), movedCharacter(character), moveDirection(direction)
	{}

	[[nodiscard]] Character* character() const
	{
		return movedCharacter;
	}
	[[nodiscard]] QPointF direction() const
	{
		return moveDirection;
	}

private:
	Character* movedCharacter = nullptr;
	QPointF moveDirection;
};

static const QEvent::Type CharacterAttackEventType = static_cast<QEvent::Type>(QEvent::User + 7);
class CharacterAttackEvent : public QEvent
{
public:
	explicit CharacterAttackEvent(Character* currentCharacter, Character* otherCharacter)
		: QEvent(CharacterAttackEventType), current(currentCharacter), otherCharacter(otherCharacter)
	{}

	[[nodiscard]] Character* character() const
	{
		return current;
	}
	[[nodiscard]] Character* other() const
	{
		return otherCharacter;
	}

private:
	Character* current = nullptr;
	Character* otherCharacter = nullptr;
};

static const QEvent::Type CharacterRemoteAttackEventType = static_cast<QEvent::Type>(QEvent::User + 8);
class CharacterRemoteAttackEvent : public QEvent
{
public:
	explicit CharacterRemoteAttackEvent(Character* currentCharacter, Character* otherCharacter)
		: QEvent(CharacterRemoteAttackEventType), current(currentCharacter), otherCharacter(otherCharacter)
	{}

	[[nodiscard]] Character* character() const
	{
		return current;
	}
	[[nodiscard]] Character* other() const
	{
		return otherCharacter;
	}

private:
	Character* current = nullptr;
	Character* otherCharacter = nullptr;
};


#endif
