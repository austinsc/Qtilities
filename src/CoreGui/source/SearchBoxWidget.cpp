/****************************************************************************
**
** Copyright (c) 2009-2012, Jaco Naude
**
** This file is part of Qtilities which is released under the following
** licensing options.
**
** Option 1: Open Source
** Under this license Qtilities is free software: you can
** redistribute it and/or modify it under the terms of the GNU General
** Public License as published by the Free Software Foundation, either
** version 3 of the License, or (at your option) any later version.
**
** Qtilities is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Qtilities. If not, see http://www.gnu.org/licenses/.
**
** Option 2: Commercial
** Alternatively, this library is also released under a commercial license
** that allows the development of closed source proprietary applications
** without restrictions on licensing. For more information on this option,
** please see the project website's licensing page:
** http://www.qtilities.org/licensing.html
**
** If you are unsure which license is appropriate for your use, please
** contact support@qtilities.org.
**
****************************************************************************/

#include "SearchBoxWidget.h"
#include "ui_SearchBoxWidget.h"
#include "QtilitiesCoreGuiConstants.h"

#include <QMenu>
#include <QAction>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::CoreGui::Icons;

struct Qtilities::CoreGui::SearchBoxWidgetPrivateData {
    SearchBoxWidgetPrivateData() : searchOptionRegEx(0),
        searchOptionFixedString(0),
        searchOptionWildcard(0),
        searchPatternGroup(0),
        textEdit(0),
        plainTextEdit(0) {}

    QMenu*                          searchOptionsMenu;
    QAction*                        searchOptionCaseSensitive;
    QAction*                        searchOptionWholeWordsOnly;

    QAction*                        searchOptionRegEx;
    QAction*                        searchOptionFixedString;
    QAction*                        searchOptionWildcard;
    QActionGroup*                   searchPatternGroup;

    SearchBoxWidget::WidgetTarget   widget_target;
    QTextEdit*                      textEdit;
    QPlainTextEdit*                 plainTextEdit;

    SearchBoxWidget::ButtonFlags    button_flags;
    SearchBoxWidget::SearchOptions  search_options;
    SearchBoxWidget::WidgetMode     widget_mode;
    SearchBoxWidget::SearchStringChangedNotificationMode search_string_notification_mode;
};

Qtilities::CoreGui::SearchBoxWidget::SearchBoxWidget(SearchOptions search_options, WidgetMode mode, ButtonFlags buttons, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchBoxWidget)
{
    ui->setupUi(this);
    d = new SearchBoxWidgetPrivateData;
    d->widget_target = SearchBoxWidget::ExternalTarget;
    d->search_string_notification_mode = SearchBoxWidget::NotifyOnChange;

    setWindowTitle(tr("Search Box Widget"));

    // Connect ui signals to own signals which will be monitored by this widget's parent
    connect(ui->btnFindPrevious,SIGNAL(clicked()),SLOT(handleFindPrevious()));
    connect(ui->btnFindNext,SIGNAL(clicked()),SLOT(handleFindNext()));
    connect(ui->btnReplacePrevious,SIGNAL(clicked()),SLOT(handleReplacePrevious()));
    connect(ui->btnReplaceNext,SIGNAL(clicked()),SLOT(handleReplaceNext()));
    connect(ui->btnReplaceAll,SIGNAL(clicked()),SLOT(handleReplaceAll()));
    connect(ui->btnClose,SIGNAL(clicked()),SLOT(handleClose()));
    connect(ui->txtSearchString,SIGNAL(textChanged(QString)),SLOT(handleSearchStringChanged(QString)));
    connect(ui->txtReplaceString,SIGNAL(textChanged(QString)),SLOT(handleReplaceStringChanged(QString)));

    // Construct the search options pop-up menu
    d->searchOptionsMenu = new QMenu(tr("Search Options"));
    d->searchOptionsMenu->setObjectName(objectName());
    ui->btnSearchOptions->setIcon(QIcon(qti_icon_SEARCH_OPTIONS_22x22));
    ui->btnSearchOptions->setIconSize(QSize(16,16));

    d->searchOptionCaseSensitive = new QAction(tr("Case Sensitive"),0);
    d->searchOptionCaseSensitive->setCheckable(true);
    d->searchOptionCaseSensitive->setObjectName(objectName());
    d->searchOptionsMenu->addAction(d->searchOptionCaseSensitive);
    connect(d->searchOptionCaseSensitive,SIGNAL(triggered()),SLOT(handleOptionsChanged()));

    d->searchOptionWholeWordsOnly = new QAction(tr("Whole Words Only"),0);
    d->searchOptionWholeWordsOnly->setObjectName(objectName());
    d->searchOptionsMenu->addAction(d->searchOptionWholeWordsOnly);
    d->searchOptionWholeWordsOnly->setCheckable(true);
    connect(d->searchOptionWholeWordsOnly,SIGNAL(triggered()),SLOT(handleOptionsChanged()));

    d->searchOptionsMenu->addSeparator();

    d->searchOptionRegEx = new QAction(tr("Regular Expression"),0);
    d->searchOptionRegEx->setObjectName(objectName());
    d->searchOptionsMenu->addAction(d->searchOptionRegEx);
    d->searchOptionRegEx->setCheckable(true);

    d->searchOptionWildcard = new QAction(tr("Wildcard Expression"),0);
    d->searchOptionWildcard->setObjectName(objectName());
    d->searchOptionsMenu->addAction(d->searchOptionWildcard);
    d->searchOptionWildcard->setCheckable(true);

    d->searchOptionFixedString = new QAction(tr("Fixed String"),this);
    d->searchOptionFixedString->setObjectName(objectName());
    d->searchOptionsMenu->addAction(d->searchOptionFixedString);
    d->searchOptionFixedString->setCheckable(true);

    d->searchPatternGroup = new QActionGroup(this);
    d->searchPatternGroup->addAction(d->searchOptionRegEx);
    d->searchPatternGroup->addAction(d->searchOptionWildcard);
    d->searchPatternGroup->addAction(d->searchOptionFixedString);
    d->searchOptionWildcard->setChecked(true);

    connect(d->searchPatternGroup,SIGNAL(triggered(QAction*)),SLOT(handleOptionsChanged()));

    ui->btnSearchOptions->setPopupMode(QToolButton::InstantPopup);
    ui->btnSearchOptions->setMenu(d->searchOptionsMenu);

    // Inspect the search options and button flags:
    setButtonFlags(buttons);
    setSearchOptions(search_options);
    setWidgetMode(mode);

    QFont normal_font = ui->txtSearchString->font();
    normal_font.setWeight(QFont::Normal);
    ui->txtSearchString->setFont(normal_font);

    normal_font = ui->txtReplaceString->font();
    normal_font.setWeight(QFont::Normal);
    ui->txtReplaceString->setFont(normal_font);
}

Qtilities::CoreGui::SearchBoxWidget::~SearchBoxWidget()
{
    delete d;
    delete ui;
}

QString Qtilities::CoreGui::SearchBoxWidget::currentSearchString() const {
    return ui->txtSearchString->text();
}

void Qtilities::CoreGui::SearchBoxWidget::setCurrentSearchString(const QString& search_string) {
    ui->txtSearchString->setText(search_string);
}

QString Qtilities::CoreGui::SearchBoxWidget::currentReplaceString() const {
    return ui->txtReplaceString->text();
}

void Qtilities::CoreGui::SearchBoxWidget::setCurrentReplaceString(const QString& replace_string) {
    ui->txtReplaceString->setText(replace_string);
}

void Qtilities::CoreGui::SearchBoxWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

bool Qtilities::CoreGui::SearchBoxWidget::caseSensitive() const {
    if (d->searchOptionCaseSensitive)
        return d->searchOptionCaseSensitive->isChecked();
    else
        return false;
}

bool Qtilities::CoreGui::SearchBoxWidget::wholeWordsOnly() const {
    if (d->searchOptionWholeWordsOnly)
        return d->searchOptionWholeWordsOnly->isChecked();
    else
        return false;
}

QRegExp::PatternSyntax Qtilities::CoreGui::SearchBoxWidget::patternSyntax() const {
    if (!d->searchOptionFixedString || !d->searchOptionRegEx || !d->searchOptionWildcard)
        return QRegExp::Wildcard;

    if (d->searchOptionRegEx->isChecked())
        return QRegExp::RegExp;
    if (d->searchOptionFixedString->isChecked())
        return QRegExp::FixedString;
    if (d->searchOptionWildcard->isChecked())
        return QRegExp::Wildcard;
    else
        return QRegExp::Wildcard;
}

void Qtilities::CoreGui::SearchBoxWidget::setPatternSyntax(QRegExp::PatternSyntax pattern_syntax) {
    if (!d->searchOptionFixedString || !d->searchOptionRegEx || !d->searchOptionWildcard)
        return;

    if (pattern_syntax == QRegExp::RegExp) {
        d->searchOptionRegEx->setChecked(true);
        d->searchOptionWildcard->setChecked(false);
        d->searchOptionFixedString->setChecked(false);
    } else if (pattern_syntax == QRegExp::FixedString) {
        d->searchOptionRegEx->setChecked(false);
        d->searchOptionWildcard->setChecked(false);
        d->searchOptionFixedString->setChecked(true);
    } else if (pattern_syntax == QRegExp::Wildcard) {
        d->searchOptionRegEx->setChecked(false);
        d->searchOptionWildcard->setChecked(true);
        d->searchOptionFixedString->setChecked(false);
    }
}

void Qtilities::CoreGui::SearchBoxWidget::setCaseSensitive(bool toggle) {
    if (d->searchOptionCaseSensitive)
        return d->searchOptionCaseSensitive->setChecked(toggle);
}

void Qtilities::CoreGui::SearchBoxWidget::setWholeWordsOnly(bool toggle) {
    if (d->searchOptionWholeWordsOnly)
        return d->searchOptionWholeWordsOnly->setChecked(toggle);
}

void Qtilities::CoreGui::SearchBoxWidget::setEditorFocus() {
    ui->txtSearchString->setFocus();
}

void Qtilities::CoreGui::SearchBoxWidget::setButtonFlags(ButtonFlags button_flags) {
    d->button_flags = button_flags;
    // Shows buttons according to button flags
    if (d->button_flags & NextButtons) {
        ui->btnFindNext->show();
        ui->btnReplaceNext->show();
    } else {
        ui->btnFindNext->hide();
        ui->btnReplaceNext->hide();
    }
    if (d->button_flags & PreviousButtons) {
        ui->btnFindPrevious->show();
        ui->btnReplacePrevious->show();
    } else {
        ui->btnFindPrevious->hide();
        ui->btnReplacePrevious->hide();
    }
    if ((d->button_flags & HideButtonDown) || (d->button_flags & HideButtonUp)) {
        if (d->button_flags & HideButtonDown)
            ui->btnClose->setArrowType(Qt::DownArrow);
        else
            ui->btnClose->setArrowType(Qt::UpArrow);
        ui->btnClose->show();
    } else {
        ui->btnClose->hide();
    }
}

Qtilities::CoreGui::SearchBoxWidget::ButtonFlags Qtilities::CoreGui::SearchBoxWidget::buttonFlags() const {
    return d->button_flags;
}

void Qtilities::CoreGui::SearchBoxWidget::setSearchOptions(SearchOptions search_options) {
    d->search_options = search_options;
    if (d->search_options & CaseSensitive) {
        d->searchOptionCaseSensitive->setVisible(true);
        d->searchOptionCaseSensitive->setChecked(true);
    } else {
        d->searchOptionCaseSensitive->setVisible(false);
    }

    if (d->search_options & WholeWordsOnly) {
        d->searchOptionWholeWordsOnly->setVisible(true);
        d->searchOptionWholeWordsOnly->setChecked(true);
    } else {
        d->searchOptionWholeWordsOnly->setVisible(false);
    }

    if (d->search_options & RegEx) {
        d->searchOptionRegEx->setVisible(true);
    }
    if (d->search_options & RegWildcard) {
        d->searchOptionWildcard->setVisible(true);
    }
    if (d->search_options & RegFixedString) {
        d->searchOptionFixedString->setVisible(true);
    }
}

Qtilities::CoreGui::SearchBoxWidget::SearchOptions Qtilities::CoreGui::SearchBoxWidget::searchOptions() const {
    return d->search_options;
}

void Qtilities::CoreGui::SearchBoxWidget::setWidgetMode(WidgetMode widget_mode) {
    d->widget_mode = widget_mode;
    // Show contents according to selected mode
    if (d->widget_mode == SearchOnly) {
        ui->widgetReplace->hide();
        setFixedHeight(24);
    } else if (d->widget_mode == SearchAndReplace) {
        ui->widgetReplace->show();
        setFixedHeight(52);
    }
}

Qtilities::CoreGui::SearchBoxWidget::WidgetMode Qtilities::CoreGui::SearchBoxWidget::widgetMode() const {
    return d->widget_mode;
}

QMenu* Qtilities::CoreGui::SearchBoxWidget::searchOptionsMenu() {
    return d->searchOptionsMenu;
}

void Qtilities::CoreGui::SearchBoxWidget::setTextEditor(QTextEdit* textEdit) {
    d->textEdit = textEdit;

    if (d->textEdit)
        d->widget_target = SearchBoxWidget::TextEdit;
}

QTextEdit* Qtilities::CoreGui::SearchBoxWidget::textEditor() const {
    return d->textEdit;
}

void Qtilities::CoreGui::SearchBoxWidget::setPlainTextEditor(QPlainTextEdit* plainTextEdit) {
    d->plainTextEdit = plainTextEdit;

    if (d->plainTextEdit) {
        d->widget_target = SearchBoxWidget::PlainTextEdit;
    }
}

QPlainTextEdit* Qtilities::CoreGui::SearchBoxWidget::plainTextEditor() const {
    return d->plainTextEdit;
}

void Qtilities::CoreGui::SearchBoxWidget::handleOptionsChanged() {
    handleFindPrevious();
    handleFindNext();
    emit searchOptionsChanged();
}

void Qtilities::CoreGui::SearchBoxWidget::handleReplaceStringChanged(const QString& string) {
    emit replaceStringChanged(string);
}

void Qtilities::CoreGui::SearchBoxWidget::handleClose() {
    if (d->widget_target == ExternalTarget)
        emit btnClose_clicked();
    else if (d->widget_target == TextEdit || d->widget_target == PlainTextEdit) {
        emit btnClose_clicked();
        hide();
    }
}

void Qtilities::CoreGui::SearchBoxWidget::handleFindNext() {
    if (d->widget_target == ExternalTarget)
        emit btnFindNext_clicked();
    else if (d->widget_target == TextEdit && d->textEdit) {
        QTextDocument::FindFlags find_flags = findFlags();

        if (!d->textEdit->find(currentSearchString(),find_flags)) {
            QTextCursor cursor = d->textEdit->textCursor();
            cursor.movePosition(QTextCursor::Start);
            d->textEdit->setTextCursor(cursor);
            d->textEdit->find(currentSearchString(),find_flags);
        }
    } else if (d->widget_target == PlainTextEdit && d->plainTextEdit) {
        QTextDocument::FindFlags find_flags = findFlags();

        if (d->plainTextEdit->find(currentSearchString(),find_flags)) {
            d->plainTextEdit->centerCursor();
        } else {
            QTextCursor cursor = d->plainTextEdit->textCursor();
            cursor.movePosition(QTextCursor::Start);
            d->plainTextEdit->setTextCursor(cursor);
            d->plainTextEdit->find(currentSearchString(),find_flags);
        }
    }
}

void Qtilities::CoreGui::SearchBoxWidget::handleFindPrevious() {
    if (d->widget_target == ExternalTarget)
        emit btnFindPrevious_clicked();
    else if (d->widget_target == TextEdit && d->textEdit) {
        if (!d->textEdit->find(currentSearchString(), findFlags() | QTextDocument::FindBackward)) {
            QTextCursor cursor = d->textEdit->textCursor();
            cursor.movePosition(QTextCursor::End);
            d->textEdit->setTextCursor(cursor);
            d->textEdit->find(currentSearchString(), findFlags() | QTextDocument::FindBackward);
        }
    } else if (d->widget_target == PlainTextEdit && d->plainTextEdit) {
        if (d->plainTextEdit->find(currentSearchString(), findFlags() | QTextDocument::FindBackward))
            d->plainTextEdit->centerCursor();
        else {
            QTextCursor cursor = d->plainTextEdit->textCursor();
            cursor.movePosition(QTextCursor::End);
            d->plainTextEdit->setTextCursor(cursor);
            d->plainTextEdit->find(currentSearchString(), findFlags() | QTextDocument::FindBackward);
        }
    }
}

void Qtilities::CoreGui::SearchBoxWidget::handleReplaceNext() {
    if (d->widget_target == ExternalTarget)
        emit btnReplaceNext_clicked();
    else if (d->widget_target == TextEdit && d->textEdit) {
        if (!d->textEdit->textCursor().hasSelection())
            handleFindNext();
        else {
            d->textEdit->textCursor().insertText(ui->txtReplaceString->text());
            handleFindNext();
        }
    } else if (d->widget_target == PlainTextEdit && d->plainTextEdit) {
        if (!d->plainTextEdit->textCursor().hasSelection())
            handleFindNext();
        else {
            d->plainTextEdit->textCursor().insertText(ui->txtReplaceString->text());
            handleFindNext();
        }
    }
}

void Qtilities::CoreGui::SearchBoxWidget::handleReplacePrevious() {
    if (d->widget_target == ExternalTarget)
        emit btnReplacePrevious_clicked();
    else if (d->widget_target == TextEdit && d->textEdit) {
        if (!d->textEdit->textCursor().hasSelection())
            handleFindPrevious();
        else {
            d->textEdit->textCursor().insertText(ui->txtReplaceString->text());
            handleFindPrevious();
        }
    } else if (d->widget_target == PlainTextEdit && d->plainTextEdit) {
        if (!d->plainTextEdit->textCursor().hasSelection())
            handleFindPrevious();
        else {
            d->plainTextEdit->textCursor().insertText(ui->txtReplaceString->text());
            handleFindPrevious();
        }
    }
}

void Qtilities::CoreGui::SearchBoxWidget::handleReplaceAll() {
    if (d->widget_target == ExternalTarget)
        emit btnReplaceAll_clicked();
    else if (d->widget_target == TextEdit && d->textEdit) {
        int count = 0;

        QTextCursor cursor = d->textEdit->textCursor();
        int position = cursor.position();
        cursor.setPosition(0);
        cursor.beginEditBlock();

        d->textEdit->setTextCursor(cursor);
        d->textEdit->find(currentSearchString(),findFlags());
        while (d->textEdit->textCursor().hasSelection()) {
            d->textEdit->textCursor().insertText(ui->txtReplaceString->text());
            ++count;
            d->textEdit->find(currentSearchString(),findFlags());
        }

        cursor.endEditBlock();
        cursor.setPosition(position);
        d->textEdit->setTextCursor(cursor);

        if (count == 1)
            setMessage(QString("<font color='green'>Replaced 1 occurance.</font>"));
        else if (count > 1)
            setMessage(QString("<font color='green'>Replaced %1 occurances.</font>").arg(count));
        else
            setMessage(QString("<font color='orange'>No occurance of your search string was found.</font>"));
    } else if (d->widget_target == PlainTextEdit && d->plainTextEdit) {
        int count = 0;

        QTextCursor cursor = d->plainTextEdit->textCursor();
        int position = cursor.position();
        cursor.setPosition(0);
        cursor.beginEditBlock();

        d->plainTextEdit->setTextCursor(cursor);
        d->plainTextEdit->find(currentSearchString(),findFlags());
        while (d->plainTextEdit->textCursor().hasSelection()) {
            d->plainTextEdit->textCursor().insertText(ui->txtReplaceString->text());
            ++count;
            d->plainTextEdit->find(currentSearchString(),findFlags());
        }

        cursor.endEditBlock();
        cursor.setPosition(position);
        d->plainTextEdit->setTextCursor(cursor);

        if (count == 1)
            setMessage(QString("<font color='green'>Replaced 1 occurance.</font>"));
        else if (count > 1)
            setMessage(QString("<font color='green'>Replaced %1 occurances.</font>").arg(count));
        else
            setMessage(QString("<font color='orange'>No occurance of your search string was found.</font>"));
    }
}

QTextDocument::FindFlags Qtilities::CoreGui::SearchBoxWidget::findFlags() const {
    QTextDocument::FindFlags find_flags = 0;
    if (wholeWordsOnly())
        find_flags |= QTextDocument::FindWholeWords;
    if (caseSensitive())
        find_flags |= QTextDocument::FindCaseSensitively;
    return find_flags;
}

void Qtilities::CoreGui::SearchBoxWidget::setInfoText(const QString& info_text) {
    QFont live_preview_label_font = ui->txtInfoMessage->font();
    live_preview_label_font.setWeight(QFont::Normal);
    ui->txtInfoMessage->setFont(live_preview_label_font);

    ui->txtInfoMessage->setText(info_text);
}

QString Qtilities::CoreGui::SearchBoxWidget::infoText() const {
    return ui->txtInfoMessage->text();
}

void Qtilities::CoreGui::SearchBoxWidget::clearInfoText() {
    ui->txtInfoMessage->clear();
}

void Qtilities::CoreGui::SearchBoxWidget::setSearchStringNotificationMode(Qtilities::CoreGui::SearchBoxWidget::SearchStringChangedNotificationMode notification_mode) {
    d->search_string_notification_mode = notification_mode;
}

Qtilities::CoreGui::SearchBoxWidget::SearchStringChangedNotificationMode Qtilities::CoreGui::SearchBoxWidget::searchStringNotificationMode() const {
    return d->search_string_notification_mode;
}

void Qtilities::CoreGui::SearchBoxWidget::setMessage(const QString& message) {
    ui->lblMessage->setText(message);
}

Qtilities::CoreGui::SearchBoxWidget::WidgetTarget Qtilities::CoreGui::SearchBoxWidget::widgetTarget() const {
    return d->widget_target;
}

void Qtilities::CoreGui::SearchBoxWidget::on_txtSearchString_returnPressed() {
    if (d->search_string_notification_mode == NotifyOnReturn) {
        handleFindPrevious();
        handleFindNext();
        emit searchStringChanged(ui->txtSearchString->text());
    }
}

void Qtilities::CoreGui::SearchBoxWidget::handleSearchStringChanged(const QString& string) {
    if (d->search_string_notification_mode == NotifyOnChange) {
        handleFindPrevious();
        handleFindNext();
        emit searchStringChanged(string);
    }
}
