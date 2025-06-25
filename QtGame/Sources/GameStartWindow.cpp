#include "GameStartWindow.h"

#include "GameEngin.h"

GameStartWindow::GameStartWindow(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	this->loadSetting();
	this->saveSetting();
	connect(GameEngine::instance(), &GameEngine::backToMenu, this, &GameStartWindow::gameOver);
}

GameStartWindow::~GameStartWindow()
{}

void GameStartWindow::saveSetting() const
{
	QSettings settings("settings.ini", QSettings::IniFormat);
	settings.setValue("FPS", ui.FPS->currentIndex());
	settings.setValue("BotNumber", ui.botNumber->value());
	settings.setValue("activeRadius", ui.activeRadius->value());
	settings.setValue("characterSize", ui.characterSize->value());
	settings.setValue("characterHealthy", ui.characterHealthy->value());
	settings.setValue("characterAttack", ui.characterAttack->value());
	settings.setValue("moveSpeed", ui.moveSpeed->value());
	settings.setValue("itemSize", ui.itemSize->value());
	settings.setValue("knifeRollMaxRadius", ui.knifeRollMaxRadius->value());
	settings.setValue("grassNum", ui.grassNum->value());

	settings.setValue("defaultFPS", 3);
	settings.setValue("defaultBotNumber", 5);
	settings.setValue("defaultActiveRadius", 3600);
	settings.setValue("defaultCharacterSize", 150);
	settings.setValue("defaultCharacterHealthy", 100);
	settings.setValue("defaultCharacterAttack", 5);
	settings.setValue("defaultMoveSpeed", 700);
	settings.setValue("defaultItemSize", 60);
	settings.setValue("defaultKnifeRollMaxRadius", 400);
	settings.setValue("defaultGrassNum", 50);
}

void GameStartWindow::loadSetting() const
{
	QSettings settings("settings.ini", QSettings::IniFormat);
	int FPS = settings.value("FPS").toInt();

	ui.FPS->setCurrentIndex(FPS);
	ui.botNumber->setValue(settings.value("BotNumber").toInt());
	ui.activeRadius->setValue(settings.value("activeRadius").toInt());
	ui.characterSize->setValue(settings.value("characterSize").toInt());
	ui.characterHealthy->setValue(settings.value("characterHealthy").toInt());
	ui.characterAttack->setValue(settings.value("characterAttack").toInt());
	ui.moveSpeed->setValue(settings.value("moveSpeed").toInt());
	ui.itemSize->setValue(settings.value("itemSize").toInt());
	ui.knifeRollMaxRadius->setValue(settings.value("knifeRollMaxRadius").toInt());
	ui.grassNum->setValue(settings.value("grassNum").toInt());
}

void GameStartWindow::acceptSetting() const
{
	GameEngine::FPS = ui.FPS->currentText().toInt();
	GameView::BotNumber = ui.botNumber->value();
	GameView::activeRadius = ui.activeRadius->value();
	GameView::groundWidth = GameView::activeRadius * 2 + 400;
	GameView::groundHeight = GameView::activeRadius * 2 + 400;
	Character::CharacterWidth = ui.characterSize->value();
	Character::CharacterHeight = ui.characterSize->value();
	Character::MaxHealthy = ui.characterHealthy->value();
	Character::BaseAttack = ui.characterAttack->value();
	Character::BaseSpeed = ui.moveSpeed->value();
	Item::ItemWidth = ui.itemSize->value();
	Item::ItemHeight = ui.itemSize->value();
	KnifeRoll::maxRadius = ui.knifeRollMaxRadius->value();
	GameView::grassNumber = ui.grassNum->value();
}

void GameStartWindow::on_btnStart_clicked()
{
	this->acceptSetting();
	GameEngine::instance()->start();
	this->hide();
}

void GameStartWindow::on_btnSetting_clicked()
{
	ui.stackedWidget->setCurrentIndex(1);
}

void GameStartWindow::on_btnBack_clicked()
{
	ui.stackedWidget->setCurrentIndex(0);
}

void GameStartWindow::on_btnAccept_clicked()
{
	this->acceptSetting();

	this->saveSetting();

	QMessageBox::information(this, "设置", "应用成功");
}

void GameStartWindow::on_btnRestore_clicked()
{
	QSettings settings("settings.ini", QSettings::IniFormat);

	int FPS = settings.value("defaultFPS").toInt();
	ui.FPS->setCurrentIndex(FPS);
	ui.botNumber->setValue(settings.value("defaultBotNumber").toInt());
	ui.activeRadius->setValue(settings.value("defaultActiveRadius").toInt());
	ui.characterSize->setValue(settings.value("defaultCharacterSize").toInt());
	ui.characterHealthy->setValue(settings.value("defaultCharacterHealthy").toInt());
	ui.characterAttack->setValue(settings.value("defaultCharacterAttack").toInt());
	ui.moveSpeed->setValue(settings.value("defaultMoveSpeed").toInt());
	ui.itemSize->setValue(settings.value("defaultItemSize").toInt());
	ui.knifeRollMaxRadius->setValue(settings.value("defaultKnifeRollMaxRadius").toInt());
	ui.grassNum->setValue(settings.value("defaultGrassNum").toInt());

	QMessageBox::information(this, "设置", "重置成功");
}

void GameStartWindow::on_btnExit_clicked()
{
	this->close();
}

void GameStartWindow::gameOver()
{
	this->show();
	//QMessageBox::information(this, "", "游戏结束");
}
