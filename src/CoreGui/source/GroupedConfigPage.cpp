/****************************************************************************
**
** Copyright 2009, Jaco Naude
**
** This library is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License.
**  
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**  
** You should have received a copy of the GNU General Public License
** along with this library.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "GroupedConfigPage.h"
#include "ui_GroupedConfigPage.h"

#include "QtilitiesApplication.h"
#include "QtilitiesCoreGuiConstants.h"
#include "QMainWindow"

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::CoreGui::Icons;
using namespace Qtilities::CoreGui::Interfaces;

struct Qtilities::CoreGui::GroupedConfigPageData {
    GroupedConfigPageData() : apply_all(false),
      use_tab_icons(false) {}

    QMap<QWidget*,IConfigPage*>     pages;
    QtilitiesCategory               category;
    bool                            apply_all;
    bool                            use_tab_icons;
    QIcon                           grouping_icon;
};

Qtilities::CoreGui::GroupedConfigPage::GroupedConfigPage(const QtilitiesCategory &category, QWidget *parent, Qt::WindowFlags f) :
    QWidget(parent,f),
    ui(new Ui::GroupedConfigPage)
{
    ui->setupUi(this);
    d = new GroupedConfigPageData;
    d->category = category;
    setObjectName(d->category.toString());

    connect(ui->groupedTab,SIGNAL(currentChanged(int)),SLOT(handleCurrentPageChanged(int)));
}

Qtilities::CoreGui::GroupedConfigPage::~GroupedConfigPage() {
    delete ui;
}

QIcon Qtilities::CoreGui::GroupedConfigPage::configPageIcon() const {
    if (!d->grouping_icon.isNull())
        return d->grouping_icon;

    // For now use the first IConfigPage icon.
    for (int i = 0; i < d->pages.count(); i++) {
        if (!d->pages.values().at(i)->configPageIcon().isNull())
            return d->pages.values().at(i)->configPageIcon();
    }

    QWidget* main_window = QtilitiesApplication::mainWindow();
    if (main_window)
        return main_window->windowIcon();

    return QIcon();
}

void Qtilities::CoreGui::GroupedConfigPage::setConfigPageIcon(const QIcon &icon) {
    if (icon.isNull())
        return;

    d->grouping_icon = icon;
}

QWidget* Qtilities::CoreGui::GroupedConfigPage::configPageWidget() {
    return this;
}

QString Qtilities::CoreGui::GroupedConfigPage::configPageTitle() const {
    return d->category.toString();
}

QtilitiesCategory Qtilities::CoreGui::GroupedConfigPage::configPageCategory() const {
    return QtilitiesCategory();
}

void Qtilities::CoreGui::GroupedConfigPage::configPageApply() {
    if (d->apply_all) {
        for (int i = 0; i < d->pages.count(); i++) {
            d->pages.values().at(i)->configPageApply();
            emit appliedPage(d->pages.values().at(i));
        }
    } else {
        if (activePage()) {
            emit appliedPage(activePage());
            activePage()->supportsApply();
        }
    }
}

bool Qtilities::CoreGui::GroupedConfigPage::supportsApply() const {
    if (d->apply_all)
        return true;
    else {
        if (activePage())
            return activePage()->supportsApply();
        else
            return false;
    }
}

void Qtilities::CoreGui::GroupedConfigPage::addConfigPage(IConfigPage *page) {
    if (hasConfigPage(page))
        return;

    if (!page->configPageWidget())
        return;

    addPageTab(page);
    d->pages[page->configPageWidget()] = page;
}

void Qtilities::CoreGui::GroupedConfigPage::removeConfigPage(IConfigPage *page) {
    if (!hasConfigPage(page))
        return;

    removePageTab(page);
    d->pages.remove(page->configPageWidget());
}

bool Qtilities::CoreGui::GroupedConfigPage::hasConfigPage(IConfigPage *page) const {
    return d->pages.values().contains(page);
}

bool Qtilities::CoreGui::GroupedConfigPage::hasConfigPage(const QString &page_title) const {
    for (int i = 0; i < d->pages.count(); i++) {
        if (d->pages.values().at(i)->configPageTitle() == page_title)
            return true;
    }
    return false;
}

IConfigPage* Qtilities::CoreGui::GroupedConfigPage::getConfigPage(const QString &page_title) const {
    for (int i = 0; i < d->pages.count(); i++) {
        if (d->pages.values().at(i)->configPageTitle() == page_title)
            return d->pages.values().at(i);
    }
    return 0;
}

IConfigPage *Qtilities::CoreGui::GroupedConfigPage::activePage() const {
    QWidget* current_widget = ui->groupedTab->currentWidget();
    return d->pages[current_widget];
}

void Qtilities::CoreGui::GroupedConfigPage::setActivePage(IConfigPage *page) {
    if (!hasConfigPage(page))
        return;

    if (page) {
        if (page->configPageWidget())
            ui->groupedTab->setCurrentWidget(page->configPageWidget());
    }
}

void Qtilities::CoreGui::GroupedConfigPage::setActivePage(const QString& page_title) {
    if (!hasConfigPage(page_title))
        return;

    IConfigPage* page = getConfigPage(page_title);
    if (page) {
        if (page->configPageWidget())
            ui->groupedTab->setCurrentWidget(page->configPageWidget());
    }
}

QList<IConfigPage *> Qtilities::CoreGui::GroupedConfigPage::configPages() const {
    return d->pages.values();
}

QStringList Qtilities::CoreGui::GroupedConfigPage::configPageNames() const {
    QStringList names;
    for (int i = 0; i < d->pages.count(); i++) {
        names << d->pages.values().at(i)->configPageTitle();
    }
    return names;
}

QtilitiesCategory Qtilities::CoreGui::GroupedConfigPage::category() const {
    return d->category;
}

void Qtilities::CoreGui::GroupedConfigPage::setApplyAll(bool apply_all) {
    d->apply_all = apply_all;
}

void Qtilities::CoreGui::GroupedConfigPage::setUseTabIcons(bool use_tab_icons) {
    d->use_tab_icons = use_tab_icons;
}

void Qtilities::CoreGui::GroupedConfigPage::addPageTab(IConfigPage *page) {
    if (d->use_tab_icons)
        ui->groupedTab->addTab(page->configPageWidget(),page->configPageIcon(),page->configPageTitle());
    else
        ui->groupedTab->addTab(page->configPageWidget(),QIcon(),page->configPageTitle());
}

void Qtilities::CoreGui::GroupedConfigPage::removePageTab(IConfigPage *page) {
    // Find the tab inx
    ui->groupedTab->removeTab(findTabIndex(page));
}

int Qtilities::CoreGui::GroupedConfigPage::findTabIndex(IConfigPage *page) {
    if (!hasConfigPage(page))
        return -1;

    if (page)
        return -1;

    if (page->configPageWidget())
        return -1;

    for (int i = 0; i < ui->groupedTab->count(); i++) {
        if (ui->groupedTab->widget(i) == page->configPageWidget())
            return i;
    }

    return -1;
}

bool Qtilities::CoreGui::GroupedConfigPage::useTabIcons() const {
    return d->use_tab_icons;
}

void Qtilities::CoreGui::GroupedConfigPage::handleCurrentPageChanged(int new_index) {
    QWidget* widget = ui->groupedTab->widget(new_index);

    if (d->pages.contains(widget))
        emit activeGroupedPageChanged(d->pages[widget]);
}
