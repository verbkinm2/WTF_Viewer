#include "toolspanel.h"
#include "ui_toolspanel.h"


ToolsPanel::ToolsPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ToolsPanel)
{
    ui->setupUi(this);

    m_color.setRgb(0,0,0);
    m_value = 0;

    QPixmap pix(16, 16);
    pix.fill(m_color);
    ui->color_button->setIcon(pix);

    connect(ui->edit_button, SIGNAL(toggled(bool)),
            this, SLOT(slotPenToggle(bool)));
    connect(ui->selection_button, SIGNAL(toggled(bool)),
            this, SLOT(slotSelectionToggle(bool)));
    connect(ui->cut_button, SIGNAL(clicked(bool)),
            this, SIGNAL(signalCutClicked(bool)));

    connect(ui->color_button, SIGNAL(clicked()),
            this, SLOT(slotSetColor()));

    connect(ui->pen_value_Spin, SIGNAL(valueChanged(int)),
            this, SLOT(slotPenValueChanged(int)));

    connect(ui->repaint, SIGNAL(clicked(bool)),
            this, SIGNAL(signalRepaint()));

    buttonCutDisable();
    ui->penGroupBox->setVisible(false);
}

ToolsPanel::~ToolsPanel()
{
    delete ui;
}

void ToolsPanel::buttonSelectionDisable(bool value)
{
    ui->selection_button->setDisabled(value);
}

void ToolsPanel::buttonEditDisable(bool value)
{
    ui->edit_button->setDisabled(value);
}

void ToolsPanel::buttonCutDisable(bool value)
{
    ui->cut_button->setDisabled(value);
}

void ToolsPanel::setToggleButtons(bool value)
{
    ui->edit_button->setChecked(value);
    ui->selection_button->setChecked(value);
}

void ToolsPanel::finishSelection()
{
    ui->selection_button->setChecked(false);
}

QColor ToolsPanel::getPenColor()
{
    return m_color;
}

int ToolsPanel::getPenValue()
{
    return ui->pen_value_Spin->value();
}

void ToolsPanel::slotSetColor()
{
    QColor color = QColorDialog::getColor(this->m_color);
    if(color.spec())
    {
        QPixmap pix(16, 16);
        pix.fill(color);
        ui->color_button->setIcon(QIcon(pix));
        m_color = color;
    }
}

void ToolsPanel::slotSelectionToggle(bool value)
{
    buttonEditDisable(value);
    emit signalSelectionToggle(value);
}

void ToolsPanel::slotPenToggle(bool value)
{
    ui->penGroupBox->setVisible(value);
    emit signalPenToggle(value);

    buttonSelectionDisable(value);

}

void ToolsPanel::slotPenValueChanged(int value)
{
    m_value = value;
}
