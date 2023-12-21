#include "lc_global.h"
#include "lc_qpropertiestree.h"
#include "lc_colorpicker.h"
#include "lc_application.h"
#include "lc_model.h"
#include "lc_mainwindow.h"
#include "object.h"
#include "piece.h"
#include "camera.h"
#include "light.h"
#include "pieceinf.h"
#include "lc_library.h"
#include "lc_qutils.h"
#include "lc_viewwidget.h"
#include "lc_previewwidget.h"

// Draw an icon indicating opened/closing branches
static QIcon drawIndicatorIcon(const QPalette &palette, QStyle *style)
{
	QPixmap pix(14, 14);
	pix.fill(Qt::transparent);
	QStyleOption branchOption;
	branchOption.rect = QRect(2, 2, 9, 9); // ### hardcoded in qcommonstyle.cpp
	branchOption.palette = palette;
	branchOption.state = QStyle::State_Children;

	QPainter p;
	// Draw closed state
	p.begin(&pix);
	style->drawPrimitive(QStyle::PE_IndicatorBranch, &branchOption, &p);
	p.end();
	QIcon rc = pix;
	rc.addPixmap(pix, QIcon::Selected, QIcon::Off);
	// Draw opened state
	branchOption.state |= QStyle::State_Open;
	pix.fill(Qt::transparent);
	p.begin(&pix);
	style->drawPrimitive(QStyle::PE_IndicatorBranch, &branchOption, &p);
	p.end();

	rc.addPixmap(pix, QIcon::Normal, QIcon::On);
	rc.addPixmap(pix, QIcon::Selected, QIcon::On);
	return rc;
}

static QIcon drawCheckBox(bool value)
{
	QStyleOptionButton opt;
	opt.state |= value ? QStyle::State_On : QStyle::State_Off;
	opt.state |= QStyle::State_Enabled;
	const QStyle *style = QApplication::style();
	// Figure out size of an indicator and make sure it is not scaled down in a list view item
	// by making the pixmap as big as a list view icon and centering the indicator in it.
	// (if it is smaller, it can't be helped)
	const int indicatorWidth = style->pixelMetric(QStyle::PM_IndicatorWidth, &opt);
	const int indicatorHeight = style->pixelMetric(QStyle::PM_IndicatorHeight, &opt);
	const int listViewIconSize = indicatorWidth;
	const int pixmapWidth = indicatorWidth;
	const int pixmapHeight = qMax(indicatorHeight, listViewIconSize);

	opt.rect = QRect(0, 0, indicatorWidth, indicatorHeight);
	QPixmap pixmap = QPixmap(pixmapWidth, pixmapHeight);
	pixmap.fill(Qt::transparent);
	{
		// Center?
		const int xoff = (pixmapWidth  > indicatorWidth)  ? (pixmapWidth  - indicatorWidth)  / 2 : 0;
		const int yoff = (pixmapHeight > indicatorHeight) ? (pixmapHeight - indicatorHeight) / 2 : 0;
		QPainter painter(&pixmap);
		painter.translate(xoff, yoff);
		style->drawPrimitive(QStyle::PE_IndicatorCheckBox, &opt, &painter);
	}
	return QIcon(pixmap);
}

int lcQPropertiesTreeDelegate::indentation(const QModelIndex &index) const
{
	if (!m_treeWidget)
		return 0;

	QTreeWidgetItem *item = m_treeWidget->indexToItem(index);
	int indent = 0;
	while (item->parent())
	{
		item = item->parent();
		++indent;
	}

	if (m_treeWidget->rootIsDecorated())
		++indent;

	return indent * m_treeWidget->indentation();
}

void lcQPropertiesTreeDelegate::slotEditorDestroyed(QObject *object)
{
	if (m_editedWidget == object)
	{
		m_editedWidget = nullptr;
		m_editedItem = nullptr;
	}
}

QWidget *lcQPropertiesTreeDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &style, const QModelIndex &index) const
{
	Q_UNUSED(style);

	if (index.column() == 1 && m_treeWidget)
	{
		QTreeWidgetItem *item = m_treeWidget->indexToItem(index);

		if (item && (item->flags() & Qt::ItemIsEnabled))
		{
			QWidget *editor = m_treeWidget->createEditor(parent, item);
			if (editor)
			{
				editor->setAutoFillBackground(true);
				editor->installEventFilter(const_cast<lcQPropertiesTreeDelegate *>(this));

				m_editedItem = item;
				m_editedWidget = editor;

				connect(editor, SIGNAL(destroyed(QObject *)), this, SLOT(slotEditorDestroyed(QObject *)));

				return editor;
			}
		}
	}

	return nullptr;
}

void lcQPropertiesTreeDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED(index)
	editor->setGeometry(option.rect.adjusted(0, 0, 0, -1));
}

void lcQPropertiesTreeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	bool hasValue = true;
	if (m_treeWidget)
		hasValue = m_treeWidget->indexToItem(index)->data(0, lcQPropertiesTree::PropertyTypeRole).toInt() != lcQPropertiesTree::PropertyGroup;

	QStyleOptionViewItem opt = option;

	opt.state &= ~QStyle::State_HasFocus;

	if (index.column() == 1 && m_treeWidget)
	{
		QTreeWidgetItem *item = m_treeWidget->indexToItem(index);
		if (m_editedItem && m_editedItem == item)
			m_disablePainting = true;
	}

	QItemDelegate::paint(painter, opt, index);
	if (option.type)
		m_disablePainting = false;

	opt.palette.setCurrentColorGroup(QPalette::Active);
	QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &opt));
	painter->save();
	painter->setPen(QPen(color));

	if (!m_treeWidget || (!m_treeWidget->lastColumn(index.column()) && hasValue))
	{
		int right = (option.direction == Qt::LeftToRight) ? option.rect.right() : option.rect.left();
		painter->drawLine(right, option.rect.y(), right, option.rect.bottom());
	}

	painter->restore();
}

void lcQPropertiesTreeDelegate::drawDecoration(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QPixmap &pixmap) const
{
	if (m_disablePainting)
		return;

	QItemDelegate::drawDecoration(painter, option, rect, pixmap);
}

void lcQPropertiesTreeDelegate::drawDisplay(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QString &text) const
{
	if (m_disablePainting)
		return;

	QItemDelegate::drawDisplay(painter, option, rect, text);
}

QSize lcQPropertiesTreeDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	return QItemDelegate::sizeHint(option, index) + QSize(3, 4);
}

bool lcQPropertiesTreeDelegate::eventFilter(QObject *object, QEvent *event)
{
	if (event->type() == QEvent::FocusOut)
	{
		QFocusEvent *fe = static_cast<QFocusEvent *>(event);
		if (fe->reason() == Qt::ActiveWindowFocusReason)
			return false;
	}
	return QItemDelegate::eventFilter(object, event);
}

lcQPropertiesTree::lcQPropertiesTree(QWidget *parent) :
	QTreeWidget(parent)
{
	setIconSize(QSize(18, 18));
	setColumnCount(2);
	QStringList labels;
	labels.append(tr("Property"));
	labels.append(tr("Value"));
	setHeaderLabels(labels);
	header()->setSectionsMovable(false);
	header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	header()->setVisible(false);
	setAlternatingRowColors(true);
	setRootIsDecorated(false);
	setEditTriggers(QAbstractItemView::EditKeyPressed);

	m_expandIcon = drawIndicatorIcon(palette(), style());
	m_checkedIcon = drawCheckBox(true);
	m_uncheckedIcon = drawCheckBox(false);

	mDelegate = new lcQPropertiesTreeDelegate(parent);
	mDelegate->setTreeWidget(this);
	setItemDelegate(mDelegate);

	SetEmpty();

	connect(header(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(resizeColumnToContents(int)));
}

QSize lcQPropertiesTree::sizeHint() const
{
	return QSize(200, -1);
}

void lcQPropertiesTree::drawRow(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItem opt = option;

	QTreeWidgetItem *item = itemFromIndex(index);

	if (item->data(0, lcQPropertiesTree::PropertyTypeRole).toInt() == lcQPropertiesTree::PropertyGroup)
	{
		const QColor c = option.palette.color(QPalette::Dark);
		painter->fillRect(option.rect, c);
		opt.palette.setColor(QPalette::AlternateBase, c);
	}

	QTreeWidget::drawRow(painter, opt, index);
	QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &opt));
	painter->save();
	painter->setPen(QPen(color));
	painter->drawLine(opt.rect.x(), opt.rect.bottom(), opt.rect.right(), opt.rect.bottom());
	painter->restore();
}

void lcQPropertiesTree::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_Return:
	case Qt::Key_Enter:
	case Qt::Key_Space: // Trigger Edit
		if (!mDelegate->editedItem())
		{
			if (const QTreeWidgetItem *item = currentItem())
			{
				if (item->columnCount() >= 2 && ((item->flags() & (Qt::ItemIsEditable | Qt::ItemIsEnabled)) == (Qt::ItemIsEditable | Qt::ItemIsEnabled)))
				{
					event->accept();
					// If the current position is at column 0, move to 1.
					QModelIndex index = currentIndex();
					if (index.column() == 0)
					{
						index = index.sibling(index.row(), 1);
						setCurrentIndex(index);
					}
					edit(index);
					return;
				}
			}
		}
		break;

	default:
		break;
	}

	QTreeWidget::keyPressEvent(event);
}

void lcQPropertiesTree::mousePressEvent(QMouseEvent *event)
{
	QTreeWidget::mousePressEvent(event);
	QTreeWidgetItem *item = itemAt(event->pos());

	if (item)
	{
		if ((item != mDelegate->editedItem()) && (event->button() == Qt::LeftButton) && (header()->logicalIndexAt(event->pos().x()) == 1) &&
			((item->flags() & (Qt::ItemIsEditable | Qt::ItemIsEnabled)) == (Qt::ItemIsEditable | Qt::ItemIsEnabled)))
			editItem(item, 1);
	}
}

void lcQPropertiesTree::Update(const lcArray<lcObject*>& Selection, lcObject* Focus)
{
	lcPropertyWidgetMode Mode = LC_PROPERTY_WIDGET_EMPTY;

	if (Focus)
	{
		switch (Focus->GetType())
		{
		case lcObjectType::Piece:
			Mode = LC_PROPERTY_WIDGET_PIECE;
			break;

		case lcObjectType::Camera:
			Mode = LC_PROPERTY_WIDGET_CAMERA;
			break;

		case lcObjectType::Light:
			Mode = LC_PROPERTY_WIDGET_LIGHT;
			break;
		}
	}
	else
	{
		for (int ObjectIdx = 0; ObjectIdx < Selection.GetSize(); ObjectIdx++)
		{
			switch (Selection[ObjectIdx]->GetType())
			{
			case lcObjectType::Piece:
				if (Mode == LC_PROPERTY_WIDGET_EMPTY)
					Mode = LC_PROPERTY_WIDGET_PIECE;
				else if (Mode != LC_PROPERTY_WIDGET_PIECE)
				{
					Mode = LC_PROPERTY_WIDGET_MULTIPLE;
					ObjectIdx = Selection.GetSize();
				}
				break;

			case lcObjectType::Camera:
				if (Mode != LC_PROPERTY_WIDGET_EMPTY)
				{
					Mode = LC_PROPERTY_WIDGET_MULTIPLE;
					ObjectIdx = Selection.GetSize();
				}
				else
				{
					Mode = LC_PROPERTY_WIDGET_CAMERA;
					Focus = Selection[ObjectIdx];
				}
				break;

			case lcObjectType::Light:
				if (Mode != LC_PROPERTY_WIDGET_EMPTY)
				{
					Mode = LC_PROPERTY_WIDGET_MULTIPLE;
					ObjectIdx = Selection.GetSize();
				}
				else
				{
					Mode = LC_PROPERTY_WIDGET_LIGHT;
					Focus = Selection[ObjectIdx];
				}
				break;
			}
		}
	}

	switch (Mode)
	{
	case LC_PROPERTY_WIDGET_EMPTY:
		SetEmpty();
		break;

	case LC_PROPERTY_WIDGET_PIECE:
		SetPiece(Selection, Focus);
		break;

	case LC_PROPERTY_WIDGET_CAMERA:
		SetCamera(Focus);
		break;

	case LC_PROPERTY_WIDGET_LIGHT:
		SetLight(Focus);
		break;

	case LC_PROPERTY_WIDGET_MULTIPLE:
		SetMultiple();
		break;
	}
}

class lcStepValidator : public QIntValidator
{
public:
	lcStepValidator(lcStep Min, lcStep Max, bool AllowEmpty)
		: QIntValidator(1, INT_MAX), mMin(Min), mMax(Max), mAllowEmpty(AllowEmpty)
	{
	}

	QValidator::State validate(QString& Input, int& Pos) const override
	{
		if (mAllowEmpty && Input.isEmpty())
			return Acceptable;

		bool Ok;
		lcStep Step = Input.toUInt(&Ok);

		if (Ok)
			return (Step >= mMin && Step <= mMax) ? Acceptable : Invalid;

		return QIntValidator::validate(Input, Pos);
	}

protected:
	lcStep mMin;
	lcStep mMax;
	bool mAllowEmpty;
};

QWidget* lcQPropertiesTree::createEditor(QWidget* Parent, QTreeWidgetItem* Item) const
{
	lcQPropertiesTree::PropertyType PropertyType = (lcQPropertiesTree::PropertyType)Item->data(0, lcQPropertiesTree::PropertyTypeRole).toInt();

	switch (PropertyType)
	{
	case PropertyGroup:
	case PropertyBool:
		return nullptr;

	case PropertyFloat:
		{
			QLineEdit* Editor = new QLineEdit(Parent);
			float Value = Item->data(0, PropertyValueRole).toFloat();
			QPointF Range = Item->data(0, PropertyRangeRole).toPointF();

			Editor->setValidator(Range.isNull() ? new QDoubleValidator(Editor) : new QDoubleValidator(Range.x(), Range.y(), 1, Editor));
			Editor->setText(lcFormatValueLocalized(Value));

			connect(Editor, &QLineEdit::returnPressed, this, &lcQPropertiesTree::slotReturnPressed);

			return Editor;
		}

	case PropertyInteger:
	{
		QLineEdit* Editor = new QLineEdit(Parent);
		int Value = Item->data(0, PropertyValueRole).toInt();
		QPoint Range = Item->data(0, PropertyRangeRole).toPoint();

		Editor->setValidator(Range.isNull() ? new QIntValidator(Editor) : new QIntValidator(Range.x(), Range.y(), Editor));
		Editor->setText(QString::number(Value));

		connect(Editor, &QLineEdit::returnPressed, this, &lcQPropertiesTree::slotReturnPressed);

		return Editor;
	}

	case PropertyStep:
		{
			QLineEdit* Editor = new QLineEdit(Parent);

			lcStep Value = Item->data(0, PropertyValueRole).toUInt();
			lcStep Show = partShow->data(0, PropertyValueRole).toUInt();
			lcStep Hide = partHide->data(0, PropertyValueRole).toUInt();

			if (Show && Hide)
			{
				if (Item == partShow)
					Editor->setValidator(new lcStepValidator(1, Hide - 1, false));
				else
					Editor->setValidator(new lcStepValidator(Show + 1, LC_STEP_MAX, true));
			}
			else
				Editor->setValidator(new lcStepValidator(1, LC_STEP_MAX, Item == partHide));

			if (Item != partHide || Value != LC_STEP_MAX)
				Editor->setText(QString::number(Value));

			connect(Editor, SIGNAL(returnPressed()), this, SLOT(slotReturnPressed()));

			return Editor;
		}

	case PropertyString:
		{
			QLineEdit *editor = new QLineEdit(Parent);
			QString value = Item->data(0, PropertyValueRole).toString();

			editor->setText(value);

			connect(editor, SIGNAL(returnPressed()), this, SLOT(slotReturnPressed()));

			return editor;
		}

	case PropertyStringList:
	{
		QComboBox* editor = new QComboBox(Parent);

		if (Item == mCameraProjectionItem)
		{
			editor->addItems( { tr("Perspective"), tr("Orthographic") } );
		}
		else if (Item == mLightTypeItem)
		{
			for (int LightTypeIndex = 0; LightTypeIndex < static_cast<int>(lcLightType::Count); LightTypeIndex++)
				editor->addItem(lcLight::GetLightTypeString(static_cast<lcLightType>(LightTypeIndex)));
		}
		else if (Item == mLightAreaShapeItem)
		{
			for (int LightAreaShapeIndex = 0; LightAreaShapeIndex < static_cast<int>(lcLightAreaShape::Count); LightAreaShapeIndex++)
				editor->addItem(lcLight::GetAreaShapeString(static_cast<lcLightAreaShape>(LightAreaShapeIndex)));
		}

		int value = Item->data(0, PropertyValueRole).toInt();
		editor->setCurrentIndex(value);

		connect(editor, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSetValue(int)));

		return editor;
	}

	case PropertyColor:
		{
			QPushButton *Editor = new QPushButton(Parent);
			QColor Value = Item->data(0, PropertyValueRole).value<QColor>();

			UpdateLightColorEditor(Editor, Value);

			connect(Editor, &QPushButton::clicked, this, &lcQPropertiesTree::LightColorButtonClicked);

			return Editor;
		}

	case PropertyPieceColor:
		{
			QPushButton *editor = new QPushButton(Parent);
			int value = Item->data(0, PropertyValueRole).toInt();

			updateColorEditor(editor, value);

			connect(editor, SIGNAL(clicked()), this, SLOT(slotColorButtonClicked()));

			return editor;
		}

	case PropertyPart:
		{
			QComboBox *editor = new QComboBox(Parent);

			editor->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
			editor->setMinimumContentsLength(1);

			lcPiecesLibrary* Library = lcGetPiecesLibrary();
			std::vector<PieceInfo*> SortedPieces;
			SortedPieces.reserve(Library->mPieces.size());
			const lcModel* ActiveModel = gMainWindow->GetActiveModel();

			for (const auto& PartIt : Library->mPieces)
			{
				PieceInfo* Info = PartIt.second;

				if (!Info->IsModel() || !Info->GetModel()->IncludesModel(ActiveModel))
					SortedPieces.push_back(PartIt.second);
			}

			auto PieceCompare = [](PieceInfo* Info1, PieceInfo* Info2)
			{
				return strcmp(Info1->m_strDescription, Info2->m_strDescription) < 0;
			};

			std::sort(SortedPieces.begin(), SortedPieces.end(), PieceCompare);

			for (PieceInfo* Info : SortedPieces)
				editor->addItem(Info->m_strDescription, QVariant::fromValue((void*)Info));

			PieceInfo *info = (PieceInfo*)Item->data(0, PropertyValueRole).value<void*>();
			editor->setCurrentIndex(editor->findData(QVariant::fromValue((void*)info)));

			connect(editor, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSetValue(int)));

			return editor;
		}
	}

	return nullptr;
}

void lcQPropertiesTree::updateColorEditor(QPushButton *editor, int value) const
{
	QImage img(12, 12, QImage::Format_ARGB32);
	img.fill(0);

	lcColor* color = &gColorList[value];
	QPainter painter(&img);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.setPen(Qt::darkGray);
	painter.setBrush(QColor::fromRgbF(color->Value[0], color->Value[1], color->Value[2]));
	painter.drawRect(0, 0, img.width() - 1, img.height() - 1);
	painter.end();

	editor->setStyleSheet("Text-align:left");
	editor->setIcon(QPixmap::fromImage(img));
	editor->setText(color->Name);
}

void lcQPropertiesTree::UpdateLightColorEditor(QPushButton* Editor, QColor Color) const
{
	QImage Image(12, 12, QImage::Format_ARGB32);
	Image.fill(0);

	QPainter Painter(&Image);
	Painter.setCompositionMode(QPainter::CompositionMode_Source);
	Painter.setPen(Qt::darkGray);
	Painter.setBrush(Color);
	Painter.drawRect(0, 0, Image.width() - 1, Image.height() - 1);
	Painter.end();

	Editor->setStyleSheet("Text-align:left");
	Editor->setIcon(QPixmap::fromImage(Image));
	Editor->setText(Color.name().toUpper());
}

void lcQPropertiesTree::slotToggled(bool Value)
{
	QTreeWidgetItem* Item = mDelegate->editedItem();
	lcModel* Model = gMainWindow->GetActiveModel();
	lcObject* Focus = Model->GetFocusObject();

	if (mWidgetMode == LC_PROPERTY_WIDGET_LIGHT)
	{
		lcLight* Light = (Focus && Focus->IsLight()) ? (lcLight*)Focus : nullptr;

		if (Light)
		{
			if (Item == mLightCastShadowItem)
			{
				Model->SetLightCastShadow(Light, Value);
			}
		}
	}
}

void lcQPropertiesTree::slotReturnPressed()
{
	QLineEdit* Editor = (QLineEdit*)sender();
	QTreeWidgetItem* Item = mDelegate->editedItem();
	lcModel* Model = gMainWindow->GetActiveModel();

	lcPiece* Piece = (mFocus && mFocus->IsPiece()) ? (lcPiece*)mFocus : nullptr;
	lcLight* Light = (mFocus && mFocus->IsLight()) ? (lcLight*)mFocus : nullptr;

	if (Item == mPositionXItem || Item == mPositionYItem || Item == mPositionZItem)
	{
		lcVector3 Center;
		lcMatrix33 RelativeRotation;
		Model->GetMoveRotateTransform(Center, RelativeRotation);
		lcVector3 Position = Center;
		float Value = lcParseValueLocalized(Editor->text());

		if (Item == mPositionXItem)
			Position[0] = Value;
		else if (Item == mPositionYItem)
			Position[1] = Value;
		else if (Item == mPositionZItem)
			Position[2] = Value;

		lcVector3 Distance = Position - Center;

		Model->MoveSelectedObjects(Distance, Distance, false, true, true, true);
	}
	else if (Item == mRotationXItem || Item == mRotationYItem || Item == mRotationZItem)
	{
		lcVector3 InitialRotation(0.0f, 0.0f, 0.0f);

		if (Piece)
			InitialRotation = lcMatrix44ToEulerAngles(Piece->mModelWorld) * LC_RTOD;
		else if (Light)
			InitialRotation = lcMatrix44ToEulerAngles(Light->mWorldMatrix) * LC_RTOD;

		lcVector3 Rotation = InitialRotation;

		float Value = lcParseValueLocalized(Editor->text());

		if (Item == mRotationXItem)
			Rotation[0] = Value;
		else if (Item == mRotationYItem)
			Rotation[1] = Value;
		else if (Item == mRotationZItem)
			Rotation[2] = Value;

		Model->RotateSelectedObjects(Rotation - InitialRotation, true, false, true, true);
	}

	if (mWidgetMode == LC_PROPERTY_WIDGET_PIECE)
	{
		if (Item == partShow)
		{
			bool Ok = false;
			lcStep Step = Editor->text().toUInt(&Ok);

			if (Ok)
				Model->SetSelectedPiecesStepShow(Step);
		}
		else if (Item == partHide)
		{
			QString Text = Editor->text();

			if (Text.isEmpty())
				Model->SetSelectedPiecesStepHide(LC_STEP_MAX);
			else
			{
				bool Ok = false;
				lcStep Step = Text.toUInt(&Ok);

				if (Ok)
					Model->SetSelectedPiecesStepHide(Step);
			}
		}
	}
	else if (mWidgetMode == LC_PROPERTY_WIDGET_CAMERA)
	{
		lcCamera* Camera = (mFocus && mFocus->IsCamera()) ? (lcCamera*)mFocus : nullptr;

		if (Camera)
		{
			if (Item == cameraPositionX || Item == cameraPositionY || Item == cameraPositionZ)
			{
				lcVector3 Center = Camera->mPosition;
				lcVector3 Position = Center;
				float Value = lcParseValueLocalized(Editor->text());

				if (Item == cameraPositionX)
					Position[0] = Value;
				else if (Item == cameraPositionY)
					Position[1] = Value;
				else if (Item == cameraPositionZ)
					Position[2] = Value;

				lcVector3 Distance = Position - Center;

				Model->MoveSelectedObjects(Distance, Distance, false, false, true, true);
			}
			else if (Item == cameraTargetX || Item == cameraTargetY || Item == cameraTargetZ)
			{
				lcVector3 Center = Camera->mTargetPosition;
				lcVector3 Position = Center;
				float Value = lcParseValueLocalized(Editor->text());

				if (Item == cameraTargetX)
					Position[0] = Value;
				else if (Item == cameraTargetY)
					Position[1] = Value;
				else if (Item == cameraTargetZ)
					Position[2] = Value;

				lcVector3 Distance = Position - Center;

				Model->MoveSelectedObjects(Distance, Distance, false, false, true, true);
			}
			else if (Item == cameraUpX || Item == cameraUpY || Item == cameraUpZ)
			{
				lcVector3 Center = Camera->mUpVector;
				lcVector3 Position = Center;
				float Value = lcParseValueLocalized(Editor->text());

				if (Item == cameraUpX)
					Position[0] = Value;
				else if (Item == cameraUpY)
					Position[1] = Value;
				else if (Item == cameraUpZ)
					Position[2] = Value;

				lcVector3 Distance = Position - Center;

				Model->MoveSelectedObjects(Distance, Distance, false, false, true, true);
			}
			else if (Item == cameraFOV)
			{
				float Value = lcParseValueLocalized(Editor->text());

				Model->SetCameraFOV(Camera, Value);
			}
			else if (Item == cameraNear)
			{
				float Value = lcParseValueLocalized(Editor->text());

				Model->SetCameraZNear(Camera, Value);
			}
			else if (Item == cameraFar)
			{
				float Value = lcParseValueLocalized(Editor->text());

				Model->SetCameraZFar(Camera, Value);
			}
			else if (Item == mCameraNameItem)
			{
				QString Value = Editor->text();

				Model->SetCameraName(Camera, Value);
			}
		}
	}
	else if (mWidgetMode == LC_PROPERTY_WIDGET_LIGHT)
	{
		if (Light)
		{
			if (Item == mLightAttenuationDistanceItem)
			{
				float Value = lcParseValueLocalized(Editor->text());

				Model->SetLightAttenuationDistance(Light, Value);
			}
			else if (Item == mLightAttenuationPowerItem)
			{
				float Value = lcParseValueLocalized(Editor->text());

				Model->SetLightAttenuationPower(Light, Value);
			}
			else if (Item == mLightSpotConeAngleItem)
			{
				float Value = lcParseValueLocalized(Editor->text());

				Model->SetSpotLightConeAngle(Light, Value);
			}
			else if (Item == mLightSpotPenumbraAngleItem)
			{
				float Value = lcParseValueLocalized(Editor->text());

				Model->SetSpotLightPenumbraAngle(Light, Value);
			}
			else if (Item == mLightSpotTightnessItem)
			{
				float Value = lcParseValueLocalized(Editor->text());

				Model->SetSpotLightTightness(Light, Value);
			}
			else if (Item == mLightAreaGridXItem)
			{
				lcVector2i AreaGrid = Light->GetAreaGrid();
				AreaGrid.x = Editor->text().toInt();

				Model->SetLightAreaGrid(Light, AreaGrid);
			}
			else if (Item == mLightAreaGridYItem)
			{
				lcVector2i AreaGrid = Light->GetAreaGrid();
				AreaGrid.y = Editor->text().toInt();

				Model->SetLightAreaGrid(Light, AreaGrid);
			}
			else if (Item == mLightSizeXItem)
			{
				lcVector2 Value = Light->GetSize();
				Value[0] = lcParseValueLocalized(Editor->text());

				Model->SetLightSize(Light, Value);
			}
			else if (Item == mLightSizeYItem)
			{
				lcVector2 Value = Light->GetSize();
				Value[1] = lcParseValueLocalized(Editor->text());

				Model->SetLightSize(Light, Value);
			}
			else if (Item == mLightPowerItem)
			{
				float Value = lcParseValueLocalized(Editor->text());

				Model->SetLightPower(Light, Value);
			}
			else if (Item == mLightNameItem)
			{
				QString Value = Editor->text();

				Model->SetLightName(Light, Value);
			}
		}
	}
}

void lcQPropertiesTree::slotSetValue(int Value)
{
	QTreeWidgetItem* Item = mDelegate->editedItem();
	lcModel* Model = gMainWindow->GetActiveModel();

	if (mWidgetMode == LC_PROPERTY_WIDGET_PIECE)
	{
		if (Item == mPieceColorItem)
		{
			Model->SetSelectedPiecesColorIndex(Value);

			QPushButton *editor = (QPushButton*)mDelegate->editor();
			updateColorEditor(editor, Value);
		}
		else if (Item == mPieceIdItem)
		{
			QComboBox *editor = (QComboBox*)sender();

			PieceInfo* Info = (PieceInfo*)editor->itemData(Value).value<void*>();
			Model->SetSelectedPiecesPieceInfo(Info);

			int ColorIndex = gDefaultColor;
			lcObject* Focus = gMainWindow->GetActiveModel()->GetFocusObject();
			if (Focus && Focus->IsPiece())
				ColorIndex = ((lcPiece*)Focus)->GetColorIndex();
			quint32 ColorCode = lcGetColorCode(ColorIndex);
			gMainWindow->PreviewPiece(Info->mFileName, ColorCode, false);
		}
	}
	else if (mWidgetMode == LC_PROPERTY_WIDGET_CAMERA)
	{
		lcObject* Focus = Model->GetFocusObject();

		if (Focus && Focus->IsCamera())
		{
			lcCamera* Camera = (lcCamera*)Focus;

			if (Item == mCameraProjectionItem)
			{
				Model->SetCameraOrthographic(Camera, Value == 1);
			}
		}
	}
	else if (mWidgetMode == LC_PROPERTY_WIDGET_LIGHT)
	{
		lcObject* Focus = Model->GetFocusObject();
		lcLight* Light = (Focus && Focus->IsLight()) ? (lcLight*)Focus : nullptr;

		if (Light)
		{
			if (Item == mLightTypeItem)
			{
				Model->SetLightType(Light, static_cast<lcLightType>(Value));
			}
			else if (Item == mLightAreaShapeItem)
			{
				Model->SetLightAreaShape(Light, static_cast<lcLightAreaShape>(Value));
			}
		}
	}
}

void lcQPropertiesTree::slotColorButtonClicked()
{
	int ColorIndex = gDefaultColor;
	lcObject* Focus = gMainWindow->GetActiveModel()->GetFocusObject();

	if (Focus && Focus->IsPiece())
		ColorIndex = ((lcPiece*)Focus)->GetColorIndex();

	QWidget* Button = (QWidget*)sender();

	if (!Button)
		return;

	lcColorPickerPopup* Popup = new lcColorPickerPopup(Button, ColorIndex);
	connect(Popup, &lcColorPickerPopup::Selected, this, &lcQPropertiesTree::slotSetValue);
	Popup->setMinimumSize(qMax(300, width()), qMax(200, static_cast<int>(width() * 2 / 3)));

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	QScreen* Screen = Button->screen();
	const QRect ScreenRect = Screen ? Screen->geometry() : QRect();
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
	QScreen* Screen = QGuiApplication::screenAt(Button->mapToGlobal(Button->rect().bottomLeft()));
	const QRect ScreenRect = Screen ? Screen->geometry() : QApplication::desktop()->geometry();
#else
	const QRect ScreenRect = QApplication::desktop()->geometry();
#endif

	int x = mapToGlobal(QPoint(0, 0)).x();
	int y = Button->mapToGlobal(Button->rect().bottomLeft()).y();

	if (x < ScreenRect.left())
		x = ScreenRect.left();
	if (y < ScreenRect.top())
		y = ScreenRect.top();

	if (x + Popup->width() > ScreenRect.right())
		x = ScreenRect.right() - Popup->width();
	if (y + Popup->height() > ScreenRect.bottom())
		y = ScreenRect.bottom() - Popup->height();

	Popup->move(QPoint(x, y));
	Popup->setFocus();
	Popup->show();
}

void lcQPropertiesTree::LightColorButtonClicked()
{
	lcModel* Model = gMainWindow->GetActiveModel();
	lcObject* Focus = Model->GetFocusObject();
	lcLight* Light = (Focus && Focus->IsLight()) ? (lcLight*)Focus : nullptr;

	if (!Light)
		return;

	QColor Color = QColorDialog::getColor(lcQColorFromVector3(Light->GetColor()), this, tr("Select Light Color"));

	if (!Color.isValid())
		return;

	Model->SetLightColor(Light, lcVector3FromQColor(Color));

	QPushButton* Editor = qobject_cast<QPushButton*>(mDelegate->editor());

	if (Editor)
		UpdateLightColorEditor(Editor, Color);
}

QTreeWidgetItem *lcQPropertiesTree::addProperty(QTreeWidgetItem *parent, const QString& label, PropertyType propertyType)
{
	QTreeWidgetItem *newItem;

	if (parent)
		newItem = new QTreeWidgetItem(parent, QStringList(label));
	else
		newItem = new QTreeWidgetItem(this, QStringList(label));

	newItem->setData(0, PropertyTypeRole, QVariant(propertyType));
	newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
	newItem->setExpanded(true);

	if (propertyType == PropertyGroup)
	{
		newItem->setFirstColumnSpanned(true);
		newItem->setIcon(0, m_expandIcon);
	}

	return newItem;
}

void lcQPropertiesTree::SetEmpty()
{
	clear();

	mPieceAttributesItem = nullptr;
	partVisibility = nullptr;
	partShow = nullptr;
	partHide = nullptr;
	partAppearance = nullptr;
	mPieceColorItem = nullptr;
	mPieceIdItem = nullptr;

	cameraPosition = nullptr;
	cameraPositionX = nullptr;
	cameraPositionY = nullptr;
	cameraPositionZ = nullptr;
	cameraTarget = nullptr;
	cameraTargetX = nullptr;
	cameraTargetY = nullptr;
	cameraTargetZ = nullptr;
	cameraUp = nullptr;
	cameraUpX = nullptr;
	cameraUpY = nullptr;
	cameraUpZ = nullptr;
	mCameraAttributesItem = nullptr;
	mCameraProjectionItem = nullptr;
	cameraFOV = nullptr;
	cameraNear = nullptr;
	cameraFar = nullptr;
	mCameraNameItem = nullptr;

	mLightColorItem = nullptr;
	mLightPowerItem = nullptr;
	mLightAttributesItem = nullptr;
	mLightTypeItem = nullptr;
	mLightNameItem = nullptr;
	mLightAttenuationDistanceItem = nullptr;
	mLightAttenuationPowerItem = nullptr;
	mLightSpotConeAngleItem = nullptr;
	mLightSpotPenumbraAngleItem = nullptr;
	mLightSpotTightnessItem = nullptr;
	mLightAreaShapeItem = nullptr;
	mLightAreaGridXItem = nullptr;
	mLightAreaGridYItem = nullptr;
	mLightSizeXItem = nullptr;
	mLightSizeYItem = nullptr;
	mLightCastShadowItem = nullptr;
	
	mPositionItem = nullptr;
	mPositionXItem = nullptr;
	mPositionYItem = nullptr;
	mPositionZItem = nullptr;
	mRotationItem = nullptr;
	mRotationXItem = nullptr;
	mRotationYItem = nullptr;
	mRotationZItem = nullptr;

	mWidgetMode = LC_PROPERTY_WIDGET_EMPTY;
	mFocus = nullptr;
}

void lcQPropertiesTree::SetPiece(const lcArray<lcObject*>& Selection, lcObject* Focus)
{
	if (mWidgetMode != LC_PROPERTY_WIDGET_PIECE)
	{
		SetEmpty();

		mPieceAttributesItem = addProperty(nullptr, tr("Piece Attributes"), PropertyGroup);
		mPieceIdItem = addProperty(mPieceAttributesItem, tr("Part"), PropertyPart);
		mPieceColorItem = addProperty(mPieceAttributesItem, tr("Color"), PropertyPieceColor);

		partVisibility = addProperty(nullptr, tr("Visible Steps"), PropertyGroup);
		partShow = addProperty(partVisibility, tr("Show"), PropertyStep);
		partHide = addProperty(partVisibility, tr("Hide"), PropertyStep);

		mPositionItem = addProperty(nullptr, tr("Position"), PropertyGroup);
		mPositionXItem = addProperty(mPositionItem, tr("X"), PropertyFloat);
		mPositionYItem = addProperty(mPositionItem, tr("Y"), PropertyFloat);
		mPositionZItem = addProperty(mPositionItem, tr("Z"), PropertyFloat);

		mRotationItem = addProperty(nullptr, tr("Rotation"), PropertyGroup);
		mRotationXItem = addProperty(mRotationItem, tr("X"), PropertyFloat);
		mRotationYItem = addProperty(mRotationItem, tr("Y"), PropertyFloat);
		mRotationZItem = addProperty(mRotationItem, tr("Z"), PropertyFloat);

		mWidgetMode = LC_PROPERTY_WIDGET_PIECE;
	}

	lcModel* Model = gMainWindow->GetActiveModel();
	lcPiece* Piece = (Focus && Focus->IsPiece()) ? (lcPiece*)Focus : nullptr;
	mFocus = Piece;

	lcVector3 Position;
	lcMatrix33 RelativeRotation;

	Model->GetMoveRotateTransform(Position, RelativeRotation);

	mPositionXItem->setText(1, lcFormatValueLocalized(Position[0]));
	mPositionXItem->setData(0, PropertyValueRole, Position[0]);
	mPositionYItem->setText(1, lcFormatValueLocalized(Position[1]));
	mPositionYItem->setData(0, PropertyValueRole, Position[1]);
	mPositionZItem->setText(1, lcFormatValueLocalized(Position[2]));
	mPositionZItem->setData(0, PropertyValueRole, Position[2]);

	lcVector3 Rotation;

	if (Piece)
		Rotation = lcMatrix44ToEulerAngles(Piece->mModelWorld) * LC_RTOD;
	else
		Rotation = lcVector3(0.0f, 0.0f, 0.0f);

	mRotationXItem->setText(1, lcFormatValueLocalized(Rotation[0]));
	mRotationXItem->setData(0, PropertyValueRole, Rotation[0]);
	mRotationYItem->setText(1, lcFormatValueLocalized(Rotation[1]));
	mRotationYItem->setData(0, PropertyValueRole, Rotation[1]);
	mRotationZItem->setText(1, lcFormatValueLocalized(Rotation[2]));
	mRotationZItem->setData(0, PropertyValueRole, Rotation[2]);

	lcStep Show = 0;
	lcStep Hide = 0;
	int ColorIndex = gDefaultColor;
	PieceInfo* Info = nullptr;

	if (Piece)
	{
		Show = Piece->GetStepShow();
		Hide = Piece->GetStepHide();
		ColorIndex = Piece->GetColorIndex();
		Info = Piece->mPieceInfo;
		quint32 ColorCode = lcGetColorCode(ColorIndex);
		gMainWindow->PreviewPiece(Info->mFileName, ColorCode, false);
	}
	else
	{
		bool FirstPiece = true;

		for (int ObjectIdx = 0; ObjectIdx < Selection.GetSize(); ObjectIdx++)
		{
			lcObject* Object = Selection[ObjectIdx];

			if (!Object->IsPiece())
				continue;

			lcPiece* SelectedPiece = (lcPiece*)Object;

			if (FirstPiece)
			{
				Show = SelectedPiece->GetStepShow();
				Hide = SelectedPiece->GetStepHide();
				ColorIndex = SelectedPiece->GetColorIndex();
				Info = SelectedPiece->mPieceInfo;

				FirstPiece = false;
			}
			else
			{
				if (SelectedPiece->GetStepShow() != Show)
					Show = 0;

				if (SelectedPiece->GetStepHide() != Hide)
					Hide = 0;

				if (SelectedPiece->GetColorIndex() != ColorIndex)
					ColorIndex = gDefaultColor;

				if (SelectedPiece->mPieceInfo != Info)
					Info = nullptr;
			}
		}
	}

	partShow->setText(1, QString::number(Show));
	partShow->setData(0, PropertyValueRole, Show);
	partHide->setText(1, Hide == LC_STEP_MAX ? QString() : QString::number(Hide));
	partHide->setData(0, PropertyValueRole, Hide);

	QImage img(16, 16, QImage::Format_ARGB32);
	img.fill(0);

	lcColor* color = &gColorList[ColorIndex];
	QPainter painter(&img);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.setPen(Qt::darkGray);
	painter.setBrush(QColor::fromRgbF(color->Value[0], color->Value[1], color->Value[2]));
	painter.drawRect(0, 0, img.width() - 1, img.height() - 1);
	painter.end();

	mPieceColorItem->setIcon(1, QIcon(QPixmap::fromImage(img)));
	mPieceColorItem->setText(1, color->Name);
	mPieceColorItem->setData(0, PropertyValueRole, ColorIndex);

	QString text = Info ? Info->m_strDescription : QString();
	mPieceIdItem->setText(1, text);
	mPieceIdItem->setToolTip(1, text);
	mPieceIdItem->setData(0, PropertyValueRole, QVariant::fromValue((void*)Info));
}

void lcQPropertiesTree::SetCamera(lcObject* Focus)
{
	if (mWidgetMode != LC_PROPERTY_WIDGET_CAMERA)
	{
		SetEmpty();

		mCameraAttributesItem = addProperty(nullptr, tr("Camera Attributes"), PropertyGroup);
		mCameraNameItem = addProperty(mCameraAttributesItem, tr("Name"), PropertyString);
		mCameraProjectionItem = addProperty(mCameraAttributesItem, tr("Projection"), PropertyStringList);
		cameraFOV = addProperty(mCameraAttributesItem, tr("FOV"), PropertyFloat);
		cameraNear = addProperty(mCameraAttributesItem, tr("Near"), PropertyFloat);
		cameraFar = addProperty(mCameraAttributesItem, tr("Far"), PropertyFloat);

		cameraPosition = addProperty(nullptr, tr("Position"), PropertyGroup);
		cameraPositionX = addProperty(cameraPosition, tr("X"), PropertyFloat);
		cameraPositionY = addProperty(cameraPosition, tr("Y"), PropertyFloat);
		cameraPositionZ = addProperty(cameraPosition, tr("Z"), PropertyFloat);

		cameraTarget = addProperty(nullptr, tr("Target"), PropertyGroup);
		cameraTargetX = addProperty(cameraTarget, tr("X"), PropertyFloat);
		cameraTargetY = addProperty(cameraTarget, tr("Y"), PropertyFloat);
		cameraTargetZ = addProperty(cameraTarget, tr("Z"), PropertyFloat);

		cameraUp = addProperty(nullptr, tr("Up"), PropertyGroup);
		cameraUpX = addProperty(cameraUp, tr("X"), PropertyFloat);
		cameraUpY = addProperty(cameraUp, tr("Y"), PropertyFloat);
		cameraUpZ = addProperty(cameraUp, tr("Z"), PropertyFloat);

		mWidgetMode = LC_PROPERTY_WIDGET_CAMERA;
	}

	lcCamera* Camera = (Focus && Focus->IsCamera()) ? (lcCamera*)Focus : nullptr;
	mFocus = Camera;

	lcVector3 Position(0.0f, 0.0f, 0.0f);
	lcVector3 Target(0.0f, 0.0f, 0.0f);
	lcVector3 UpVector(0.0f, 0.0f, 0.0f);
	bool Ortho = false;
	float FoV = 60.0f;
	float ZNear = 1.0f;
	float ZFar = 100.0f;
	QString Name;

	if (Camera)
	{
		Position = Camera->mPosition;
		Target = Camera->mTargetPosition;
		UpVector = Camera->mUpVector;

		Ortho = Camera->IsOrtho();
		FoV = Camera->m_fovy;
		ZNear = Camera->m_zNear;
		ZFar = Camera->m_zFar;
		Name = Camera->GetName();
	}

	cameraPositionX->setText(1, lcFormatValueLocalized(Position[0]));
	cameraPositionX->setData(0, PropertyValueRole, Position[0]);
	cameraPositionY->setText(1, lcFormatValueLocalized(Position[1]));
	cameraPositionY->setData(0, PropertyValueRole, Position[1]);
	cameraPositionZ->setText(1, lcFormatValueLocalized(Position[2]));
	cameraPositionZ->setData(0, PropertyValueRole, Position[2]);

	cameraTargetX->setText(1, lcFormatValueLocalized(Target[0]));
	cameraTargetX->setData(0, PropertyValueRole, Target[0]);
	cameraTargetY->setText(1, lcFormatValueLocalized(Target[1]));
	cameraTargetY->setData(0, PropertyValueRole, Target[1]);
	cameraTargetZ->setText(1, lcFormatValueLocalized(Target[2]));
	cameraTargetZ->setData(0, PropertyValueRole, Target[2]);

	cameraUpX->setText(1, lcFormatValueLocalized(UpVector[0]));
	cameraUpX->setData(0, PropertyValueRole, UpVector[0]);
	cameraUpY->setText(1, lcFormatValueLocalized(UpVector[1]));
	cameraUpY->setData(0, PropertyValueRole, UpVector[1]);
	cameraUpZ->setText(1, lcFormatValueLocalized(UpVector[2]));
	cameraUpZ->setData(0, PropertyValueRole, UpVector[2]);

	mCameraProjectionItem->setText(1, Ortho ? tr("Orthographic") : tr("Perspective"));
	mCameraProjectionItem->setData(0, PropertyValueRole, Ortho);
	cameraFOV->setText(1, lcFormatValueLocalized(FoV));
	cameraFOV->setData(0, PropertyValueRole, FoV);
	cameraNear->setText(1, lcFormatValueLocalized(ZNear));
	cameraNear->setData(0, PropertyValueRole, ZNear);
	cameraFar->setText(1, lcFormatValueLocalized(ZFar));
	cameraFar->setData(0, PropertyValueRole, ZFar);

	mCameraNameItem->setText(1, Name);
	mCameraNameItem->setData(0, PropertyValueRole, Name);
}

void lcQPropertiesTree::SetLight(lcObject* Focus)
{
	lcLight* Light = (Focus && Focus->IsLight()) ? (lcLight*)Focus : nullptr;

	QString Name = tr("Light");
	lcLightType LightType = lcLightType::Point;
	lcLightAreaShape LightAreaShape = lcLightAreaShape::Rectangle;
	lcVector2 LightSize(0.0f, 0.0f);
	lcVector2i AreaGrid(2, 2);
	float Power = 0.0f;
	float AttenuationDistance = 0.0f;
	float AttenuationPower = 0.0f;
	bool CastShadow = true;
	lcVector3 Position(0.0f, 0.0f, 0.0f);
	QColor Color(Qt::white);
	float SpotConeAngle = 0.0f, SpotPenumbraAngle = 0.0f, SpotTightness = 0.0f;

	if (Light)
	{
		Name = Light->GetName();

		CastShadow = Light->GetCastShadow();
		Position = Light->GetPosition();
		Color = lcQColorFromVector3(Light->GetColor());
		Power = Light->GetPower();
		AttenuationDistance = Light->GetAttenuationDistance();
		AttenuationPower = Light->GetAttenuationPower();
		SpotConeAngle = Light->GetSpotConeAngle();
		SpotPenumbraAngle = Light->GetSpotPenumbraAngle();
		SpotTightness = Light->GetSpotTightness();

		LightType = Light->GetLightType();
		LightAreaShape = Light->GetAreaShape();
		LightSize = Light->GetSize();
		AreaGrid = Light->GetAreaGrid();
	}

	if (mWidgetMode != LC_PROPERTY_WIDGET_LIGHT || mLightType != LightType)
	{
		SetEmpty();

		// Attributes
		mLightAttributesItem = addProperty(nullptr, tr("Light Attributes"), PropertyGroup);
		mLightNameItem = addProperty(mLightAttributesItem, tr("Name"), PropertyString);
		mLightTypeItem = addProperty(mLightAttributesItem, tr("Type"), PropertyStringList);

		mLightColorItem = addProperty(mLightAttributesItem, tr("Color"), PropertyColor);
		mLightColorItem->setToolTip(1, tr("Color of the emitted light."));

		mLightPowerItem = addProperty(mLightAttributesItem, tr("Power"), PropertyFloat);
		mLightPowerItem->setToolTip(1, tr("Power of the light (Watts in Blender)."));

		mLightCastShadowItem = addProperty(mLightAttributesItem, tr("Cast Shadows"), PropertyBool);

		mLightAttenuationDistanceItem = addProperty(mLightAttributesItem, tr("Attenuation Distance"), PropertyFloat);
		mLightAttenuationDistanceItem->setToolTip(1, tr("The distance at which the full light intensity arrives (POV-Ray only)."));

		mLightAttenuationPowerItem = addProperty(mLightAttributesItem, tr("Attenuation Power"), PropertyFloat);
		mLightAttenuationPowerItem->setToolTip(1, tr("Light falloff rate (POV-Ray only)."));

		switch (LightType)
		{
		case lcLightType::Point:
			mLightSizeXItem = addProperty(mLightAttributesItem, tr("Radius"), PropertyFloat);
			mLightSizeXItem->setToolTip(1, tr("Shadow soft size (Blender only)."));
			break;

		case lcLightType::Spot:
			mLightSpotConeAngleItem = addProperty(mLightAttributesItem, tr("Spot Cone Angle"), PropertyFloat);
			mLightSpotConeAngleItem->setToolTip(1, tr("The angle (in degrees) of the spot light's beam."));

			mLightSpotPenumbraAngleItem = addProperty(mLightAttributesItem, tr("Spot Penumbra Angle"), PropertyFloat);
			mLightSpotPenumbraAngleItem->setToolTip(1, tr("The angle (in degrees) over which the intensity of the spot light falls off to zero."));

			mLightSpotTightnessItem = addProperty(mLightAttributesItem, tr("Spot Tightness"), PropertyFloat);
			mLightSpotTightnessItem->setToolTip(1, tr("Additional exponential spot light edge softening (POV-Ray only)."));

			mLightSizeXItem = addProperty(mLightAttributesItem, tr("Radius"), PropertyFloat);
			mLightSizeXItem->setToolTip(1, tr("Shadow soft size (Blender only)."));
			break;

		case lcLightType::Directional:
			mLightSizeXItem = addProperty(mLightAttributesItem, tr("Angle"), PropertyFloat);
			mLightSizeXItem->setToolTip(1, tr("Angular diameter of the light (Blender only)."));
			break;

		case lcLightType::Area:
			mLightAreaShapeItem = addProperty(mLightAttributesItem, tr("Area Shape"), PropertyStringList);
			mLightAreaShapeItem->setToolTip(1, tr("The shape of the area light."));

			switch (LightAreaShape)
			{
			case lcLightAreaShape::Rectangle:
			case lcLightAreaShape::Ellipse:
				mLightSizeXItem = addProperty(mLightAttributesItem, tr("Area Width"), PropertyFloat);
				mLightSizeXItem->setToolTip(1, tr("The width (X direction) of the area light."));

				mLightSizeYItem = addProperty(mLightAttributesItem, tr("Area Height"), PropertyFloat);
				mLightSizeYItem->setToolTip(1, tr("The height (Y direction) of the area light."));
				break;

			case lcLightAreaShape::Square:
			case lcLightAreaShape::Disk:
				mLightSizeXItem = addProperty(mLightAttributesItem, tr("Area Size"), PropertyFloat);
				mLightSizeXItem->setToolTip(1, tr("The size of the area light."));

				mLightSizeYItem = nullptr;
				break;

			case lcLightAreaShape::Count:
				break;
			}

			mLightAreaGridXItem = addProperty(mLightAttributesItem, tr("Area Grid X"), PropertyInteger);
			mLightAreaGridXItem->setToolTip(1, tr("Number of point sources along the X axis (POV-Ray only)."));
			mLightAreaGridXItem->setData(0, PropertyRangeRole, QPointF(1, INT_MAX));

			mLightAreaGridYItem = addProperty(mLightAttributesItem, tr("Area Grid Y"), PropertyInteger);
			mLightAreaGridYItem->setToolTip(1, tr("Number of point sources along the Y axis (POV-Ray only)."));
			mLightAreaGridYItem->setData(0, PropertyRangeRole, QPointF(1, INT_MAX));

			break;

		case lcLightType::Count:
			break;
		}

		mPositionItem = addProperty(nullptr, tr("Position"), PropertyGroup);
		mPositionXItem = addProperty(mPositionItem, tr("X"), PropertyFloat);
		mPositionYItem = addProperty(mPositionItem, tr("Y"), PropertyFloat);
		mPositionZItem = addProperty(mPositionItem, tr("Z"), PropertyFloat);

		if (LightType != lcLightType::Point)
		{
			mRotationItem = addProperty(nullptr, tr("Rotation"), PropertyGroup);
			mRotationXItem = addProperty(mRotationItem, tr("X"), PropertyFloat);
			mRotationYItem = addProperty(mRotationItem, tr("Y"), PropertyFloat);
			mRotationZItem = addProperty(mRotationItem, tr("Z"), PropertyFloat);
		}

		mWidgetMode = LC_PROPERTY_WIDGET_LIGHT;
		mLightType = LightType;
	}

	mFocus = Light;

	mPositionXItem->setText(1, lcFormatValueLocalized(Position[0]));
	mPositionXItem->setData(0, PropertyValueRole, Position[0]);
	mPositionYItem->setText(1, lcFormatValueLocalized(Position[1]));
	mPositionYItem->setData(0, PropertyValueRole, Position[1]);
	mPositionZItem->setText(1, lcFormatValueLocalized(Position[2]));
	mPositionZItem->setData(0, PropertyValueRole, Position[2]);

	if (LightType != lcLightType::Point)
	{
		lcVector3 Rotation;

		if (Light)
			Rotation = lcMatrix44ToEulerAngles(Light->mWorldMatrix) * LC_RTOD;
		else
			Rotation = lcVector3(0.0f, 0.0f, 0.0f);

		mRotationXItem->setText(1, lcFormatValueLocalized(Rotation[0]));
		mRotationXItem->setData(0, PropertyValueRole, Rotation[0]);
		mRotationYItem->setText(1, lcFormatValueLocalized(Rotation[1]));
		mRotationYItem->setData(0, PropertyValueRole, Rotation[1]);
		mRotationZItem->setText(1, lcFormatValueLocalized(Rotation[2]));
		mRotationZItem->setData(0, PropertyValueRole, Rotation[2]);
	}

	QImage ColorImage(16, 16, QImage::Format_ARGB32);
	ColorImage.fill(0);

	QPainter painter(&ColorImage);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.setPen(Qt::darkGray);
	painter.setBrush(Color);
	painter.drawRect(0, 0, ColorImage.width() - 1, ColorImage.height() - 1);
	painter.end();

	mLightColorItem->setIcon(1, QIcon(QPixmap::fromImage(ColorImage)));
	mLightColorItem->setText(1, Color.name().toUpper());
	mLightColorItem->setData(0, PropertyValueRole, Color);

	mLightPowerItem->setText(1, lcFormatValueLocalized(Power));
	mLightPowerItem->setData(0, PropertyValueRole, Power);

	mLightAttenuationDistanceItem->setText(1, lcFormatValueLocalized(AttenuationDistance));
	mLightAttenuationDistanceItem->setData(0, PropertyValueRole, AttenuationDistance);
	mLightAttenuationDistanceItem->setData(0, PropertyRangeRole, QPointF(0.0, FLT_MAX));

	mLightAttenuationPowerItem->setText(1, lcFormatValueLocalized(AttenuationPower));
	mLightAttenuationPowerItem->setData(0, PropertyValueRole, AttenuationPower);
	mLightAttenuationPowerItem->setData(0, PropertyRangeRole, QPointF(0.0, FLT_MAX));

	mLightTypeItem->setText(1, lcLight::GetLightTypeString(LightType));
	mLightTypeItem->setData(0, PropertyValueRole, static_cast<int>(LightType));

	mLightCastShadowItem->setCheckState(1, CastShadow ? Qt::Checked : Qt::Unchecked);
	mLightCastShadowItem->setData(0, PropertyValueRole, CastShadow);

	mLightSizeXItem->setText(1, lcFormatValueLocalized(LightSize[0]));
	mLightSizeXItem->setData(0, PropertyValueRole, LightSize[0]);

	if (mLightSizeYItem)
	{
		mLightSizeYItem->setText(1, lcFormatValueLocalized(LightSize[1]));
		mLightSizeYItem->setData(0, PropertyValueRole, LightSize[1]);
	}

	switch (LightType)
	{
	case lcLightType::Point:
		break;

	case lcLightType::Spot:
		mLightSpotConeAngleItem->setText(1, lcFormatValueLocalized(SpotConeAngle));
		mLightSpotConeAngleItem->setData(0, PropertyValueRole, SpotConeAngle);
		mLightSpotConeAngleItem->setData(0, PropertyRangeRole, QPointF(1.0, 180.0));

		mLightSpotPenumbraAngleItem->setText(1, lcFormatValueLocalized(SpotPenumbraAngle));
		mLightSpotPenumbraAngleItem->setData(0, PropertyValueRole, SpotPenumbraAngle);
		mLightSpotPenumbraAngleItem->setData(0, PropertyRangeRole, QPointF(0.0, 180.0));

		mLightSpotTightnessItem->setText(1, lcFormatValueLocalized(SpotTightness));
		mLightSpotTightnessItem->setData(0, PropertyValueRole, SpotTightness);
		mLightSpotTightnessItem->setData(0, PropertyRangeRole, QPointF(0.0, 100.0));
		break;

	case lcLightType::Directional:
		break;

	case lcLightType::Area:
		mLightAreaShapeItem->setText(1, lcLight::GetAreaShapeString(LightAreaShape));
		mLightAreaShapeItem->setData(0, PropertyValueRole, static_cast<int>(LightAreaShape));

		mLightAreaGridXItem->setText(1, QString::number(AreaGrid.x));
		mLightAreaGridXItem->setData(0, PropertyValueRole, AreaGrid.x);

		mLightAreaGridYItem->setText(1, QString::number(AreaGrid.y));
		mLightAreaGridYItem->setData(0, PropertyValueRole, AreaGrid.y);
		break;

	case lcLightType::Count:
		break;
	}

	mLightNameItem->setText(1, Name);
	mLightNameItem->setData(0, PropertyValueRole, QVariant::fromValue(Name));
}

void lcQPropertiesTree::SetMultiple()
{
	if (mWidgetMode != LC_PROPERTY_WIDGET_MULTIPLE)
	{
		SetEmpty();

		addProperty(nullptr, tr("Multiple Objects Selected"), PropertyGroup);

		mWidgetMode = LC_PROPERTY_WIDGET_MULTIPLE;
	}

	mFocus = nullptr;
}

bool lcQPropertiesTree::lastColumn(int column) const
{
	return header()->visualIndex(column) == columnCount() - 1;
}
