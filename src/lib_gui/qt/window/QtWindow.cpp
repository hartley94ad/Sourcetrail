#include "qt/window/QtWindow.h"

#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QSysInfo>

#include "qt/utility/QtDeviceScaledPixmap.h"
#include "qt/utility/utilityQt.h"
#include "settings/ApplicationSettings.h"
#include "utility/ResourcePaths.h"

QtWindow::QtWindow(QWidget* parent)
	: QtWindowStackElement(parent)
	, m_window(nullptr)
	, m_title(nullptr)
	, m_nextButton(nullptr)
	, m_previousButton(nullptr)
	, m_closeButton(nullptr)
	, m_cancelAble(true)
	, m_scrollAble(false)
	, m_hasLogo(false)
	, m_mousePressedInWindow(false)
{
	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground, true);

	m_window = new QWidget(this);

	std::string frameStyle =
		"#window {"
		"	border: 1px solid lightgray;"
		"	border-radius: 15px;"
		"	background: white;"
		"}";
	m_window->setStyleSheet(frameStyle.c_str());
	m_window->setObjectName("window");

	// window shadow
	if (QSysInfo::macVersion() == QSysInfo::MV_None)
	{
		QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
		effect->setBlurRadius(5);
		effect->setXOffset(2);
		effect->setYOffset(2);
		effect->setColor(Qt::darkGray);
		m_window->setGraphicsEffect(effect);
	}

	QVBoxLayout* layout = new QVBoxLayout(m_window);
	layout->setSpacing(1);

	m_content = new QWidget();
	layout->addWidget(m_content);

	QHBoxLayout* gripLayout = new QHBoxLayout();
	QSizeGrip* sizeGrip = new QSizeGrip(m_window);
	sizeGrip->setStyleSheet(QString::fromStdString(
		"QSizeGrip {"
		"	max-height: 12px;"
		"	max-width: 12px;"
		"	border-image: url(" + ResourcePaths::getGuiPath() + "window/size_grip.png);"
		"}"
	));
	gripLayout->addWidget(new QWidget());
	gripLayout->addWidget(sizeGrip);
	layout->addLayout(gripLayout);

	resize(sizeHint());

	this->raise();
}

QSize QtWindow::sizeHint() const
{
	return QSize(
		ApplicationSettings::getInstance()->getWindowBaseWidth(),
		ApplicationSettings::getInstance()->getWindowBaseHeight()
	);
}

void QtWindow::setup()
{
	m_content->setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath() + "window/window.css").c_str());

	QVBoxLayout* layout = new QVBoxLayout();
	layout->setContentsMargins(25, 30, 25, 0);

	m_title = new QLabel();
	m_title->setObjectName("title");
	layout->addWidget(m_title);
	layout->addSpacing(10);

	QWidget* contentWidget = new QWidget();
	contentWidget->setObjectName("form");

	populateWindow(contentWidget);

	QScrollArea* scrollArea = new QScrollArea();
	scrollArea->setFrameShadow(QFrame::Plain);
	scrollArea->setObjectName("formArea");
	scrollArea->setWidgetResizable(true);

	scrollArea->setWidget(contentWidget);
	layout->addWidget(scrollArea);

	if (!m_scrollAble)
	{
		scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}
	else
	{
		scrollArea->setObjectName("scrollArea");
	}

	{
		m_nextButton = new QPushButton("Next");
		m_nextButton->setObjectName("windowButton");
		connect(m_nextButton, SIGNAL(clicked()), this, SLOT(handleNextPress()));

		m_previousButton = new QPushButton("Previous");
		m_previousButton->setObjectName("windowButton");
		connect(m_previousButton, SIGNAL(clicked()), this, SLOT(handlePreviousPress()));

		m_closeButton = new QPushButton("Cancel");
		m_closeButton->setObjectName("windowButton");
		connect(m_closeButton, SIGNAL(clicked()), this, SLOT(handleClosePress()));

		QHBoxLayout* buttons = new QHBoxLayout();
		buttons->addWidget(m_closeButton);
		buttons->addStretch();
		buttons->addWidget(m_previousButton);
		buttons->addSpacing(3);
		buttons->addWidget(m_nextButton);

		layout->addLayout(buttons);
	}

	m_content->setLayout(layout);

	setupDone();
}

void QtWindow::setCancelAble(bool cancelable)
{
	setCloseEnabled(cancelable);

	m_cancelAble = cancelable;
}

void QtWindow::setScrollAble(bool scrollAble)
{
	m_scrollAble = scrollAble;
}

bool QtWindow::isScrollAble() const
{
	return m_scrollAble;
}

void QtWindow::updateTitle(QString title)
{
	if (m_title)
	{
		m_title->setText(title);
	}
}

void QtWindow::updateNextButton(QString text)
{
	if (m_nextButton)
	{
		m_nextButton->setText(text);
	}
}

void QtWindow::updateCloseButton(QString text)
{
	if (m_closeButton)
	{
		m_closeButton->setText(text);
	}
}

void QtWindow::setNextEnabled(bool enabled)
{
	if (m_nextButton)
	{
		m_nextButton->setEnabled(enabled);
	}
}

void QtWindow::setPreviousEnabled(bool enabled)
{
	if (m_previousButton)
	{
		m_previousButton->setEnabled(enabled);
	}
}

void QtWindow::setCloseEnabled(bool enabled)
{
	if (m_closeButton)
	{
		m_closeButton->setEnabled(enabled);
	}
}

void QtWindow::setNextVisible(bool visible)
{
	if (m_nextButton)
	{
		m_nextButton->setVisible(visible);
	}
}

void QtWindow::setPreviousVisible(bool visible)
{
	if (m_previousButton)
	{
		m_previousButton->setVisible(visible);
	}
}

void QtWindow::setCloseVisible(bool visible)
{
	if (m_closeButton)
	{
		m_closeButton->setVisible(visible);
	}
}

void QtWindow::setNextDefault(bool isDefault)
{
	if (m_nextButton)
	{
		m_nextButton->setDefault(isDefault);
	}
}

void QtWindow::setPreviousDefault(bool isDefault)
{
	if (m_previousButton)
	{
		m_previousButton->setDefault(isDefault);
	}
}

void QtWindow::setCloseDefault(bool isDefault)
{
	if (m_closeButton)
	{
		m_closeButton->setDefault(isDefault);
	}
}

void QtWindow::showWindow()
{
	show();
}

void QtWindow::hideWindow()
{
	hide();
}

void QtWindow::resizeEvent(QResizeEvent *event)
{
	QSize size = event->size();

	if (size.width() < 300)
	{
		size.setWidth(300);
		resize(size);
		return;
	}

	if (size.height() < 200)
	{
		size.setHeight(200);
		resize(size);
		return;
	}

	int displacement = 0;
	if (m_hasLogo)
	{
		displacement = 58;
	}

	QSize windowSize = size - QSize(10, 20 + displacement);

	m_window->resize(windowSize);
	m_window->move(5, displacement + 10);
}

void QtWindow::keyPressEvent(QKeyEvent* event)
{
	if (m_cancelAble && event->key() == Qt::Key_Escape)
	{
		handleClose();
		return;
	}

	std::vector<QPushButton*> buttons;
	if (m_nextButton && m_nextButton->isVisible()) buttons.push_back(m_nextButton);
	if (m_closeButton && m_closeButton->isVisible()) buttons.push_back(m_closeButton);
	if (m_previousButton && m_previousButton->isVisible()) buttons.push_back(m_previousButton);

	if (event->key() == Qt::Key_Return)
	{
		for (QPushButton* button : buttons)
		{
			if (button->isDefault())
			{
				button->animateClick();
				return;
			}
		}
	}

	if (event->key() == Qt::Key_Tab)
	{
		for (size_t i = 0; i < buttons.size(); i++)
		{
			if (buttons[i]->isDefault())
			{
				buttons[i]->setDefault(false);
				buttons[(i + 1) % buttons.size()]->setDefault(true);
				return;
			}
		}

		if (buttons.size())
		{
			buttons[0]->setDefault(true);
			return;
		}
	}

	QWidget::keyPressEvent(event);
}

void QtWindow::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton && m_mousePressedInWindow)
	{
		move(event->globalPos() - m_dragPosition);
		event->accept();
	}
}

void QtWindow::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_dragPosition = event->globalPos() - frameGeometry().topLeft();
		event->accept();
		m_mousePressedInWindow = true;
	}
}

void QtWindow::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_dragPosition = event->globalPos() - frameGeometry().topLeft();
		event->accept();
		m_mousePressedInWindow = false;
	}
}

void QtWindow::populateWindow(QWidget* widget)
{
}

void QtWindow::windowReady()
{
}

void QtWindow::handleNext()
{
	emit finished();
}

void QtWindow::handlePrevious()
{
	emit previous();
}

void QtWindow::handleClose()
{
	emit canceled();
}

void QtWindow::setupDone()
{
	windowReady();

	QSize actualSize = m_window->sizeHint() + QSize(50, 50);
	QSize preferredSize = sizeHint();

	QSize size(qMax(actualSize.width(), preferredSize.width()), qMax(actualSize.height(), preferredSize.height()));
	resize(size);

	if (parentWidget())
	{
		move(
			parentWidget()->pos().x() + parentWidget()->width() / 2 - size.width() / 2,
			parentWidget()->pos().y() + parentWidget()->height() / 2 - size.height() / 2
		);
	}
}

void QtWindow::addLogo()
{
	QtDeviceScaledPixmap coatiLogo((ResourcePaths::getGuiPath() + "window/logo.png").c_str());
	coatiLogo.scaleToWidth(150);

	QLabel* coatiLogoLabel = new QLabel(this);
	coatiLogoLabel->setPixmap(coatiLogo.pixmap());
	coatiLogoLabel->resize(coatiLogo.width(), coatiLogo.height());
	coatiLogoLabel->move(30, 25);
	coatiLogoLabel->show();

	m_hasLogo = true;
	resize(sizeHint());
}

void QtWindow::handleNextPress()
{
	handleNext();
}

void QtWindow::handlePreviousPress()
{
	handlePrevious();
}

void QtWindow::handleClosePress()
{
	handleClose();
}
