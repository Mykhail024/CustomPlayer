#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>
#include <QHBoxLayout>

#include "ListControlsPanel.h"

ListControlsPanel::ListControlsPanel(QWidget *parrent) : QWidget(parrent)
{
	this->setObjectName("ListControlsPanel");

	int buttonSize = this->width() * 0.045;
	int iconSize = buttonSize * 0.8;

	auto *layout = new QHBoxLayout(this);
	layout->setSpacing(0);
	layout->setContentsMargins(3, 3, 3, 3);

	openFolderBtn = new QPushButton();
	openFolderBtn->setObjectName("OpenFolderButton");
	openFolderBtn->setFixedSize(buttonSize, buttonSize);
	openFolderBtn->setIconSize(QSize(iconSize, iconSize));

	findLineEdit = new QLineEdit();
	findLineEdit->setObjectName("FindLineEdit");
	findLineEdit->setPlaceholderText("Find");
	findLineEdit->setClearButtonEnabled(true);
	findLineEdit->setFixedWidth(this->width() * 0.8);
	findLineEdit->setContentsMargins(0, 0, 0, 0);

	layout->addWidget(findLineEdit, 0, Qt::AlignCenter);
	layout->addWidget(openFolderBtn);

	this->setLayout(layout);

	connect(openFolderBtn, &QPushButton::clicked, this, &ListControlsPanel::openFolderButtonClick);
	connect(findLineEdit, &QLineEdit::textChanged, this, &ListControlsPanel::onFindLineEditTextChange);
}

void ListControlsPanel::paintEvent(QPaintEvent *pe)
{
	QStyleOption o;
	o.initFrom(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
};
