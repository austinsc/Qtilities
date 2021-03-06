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

#ifndef GROUPED_CONFIG_PAGE
#define GROUPED_CONFIG_PAGE

#include "QtilitiesCoreGui_global.h"
#include "IConfigPage.h"

#include <QWidget>

namespace Ui {
    class GroupedConfigPage;
}

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces; 
        /*!
          \struct GroupedConfigPageData
          \brief The GroupedConfigPageData struct stores private data in GroupedConfigPage.
          */
        struct GroupedConfigPageData;

        /*!
        \class GroupedConfigPage
        \brief A config page which groups other IConfigPage pages using tabs.

        <i>This class was added in %Qtilities v1.1.</i>
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT GroupedConfigPage : public QWidget, public IConfigPage
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::CoreGui::Interfaces::IConfigPage)

        public:
            GroupedConfigPage(const QtilitiesCategory& category, QWidget* parent = 0, Qt::WindowFlags f = 0);
            ~GroupedConfigPage();

            // --------------------------------------------
            // IConfigPage Implementation
            // --------------------------------------------
            QIcon configPageIcon() const;
            void setConfigPageIcon(const QIcon& icon);
            QWidget* configPageWidget();
            QtilitiesCategory configPageCategory() const;
            QString configPageTitle() const;
            void configPageApply();
            bool supportsApply() const;

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

            // --------------------------------------------
            // Group Page Specific
            // --------------------------------------------
            //! Adds a IConfigPage to this grouping.
            void addConfigPage(IConfigPage* page);
            //! Removes a IConfigPage from this grouping.
            void removeConfigPage(IConfigPage* page);
            //! Checks if a page exists in this grouping.
            bool hasConfigPage(IConfigPage* page) const;
            //! Checks if a page exists in this grouping, specified by the title of the page.
            bool hasConfigPage(const QString& page_title) const;
            //! Gets a IConfigPage with the given title.
            IConfigPage* getConfigPage(const QString& page_title) const;

            //! Returns the current active config page.
            IConfigPage* activePage() const;
            //! Sets the current active page.
            void setActivePage(IConfigPage* page);
            //! Sets the current active page, specified by the title of the page.
            void setActivePage(const QString& page_title);

            //! Returns a list of all config pages in this grouping.
            QList<IConfigPage*> configPages() const;
            //! Returns a list of names of all config pages in this grouping.
            QStringList configPageNames() const;

            //! Returns the category of this grouped page.
            QtilitiesCategory category() const;

            //! Sets that this grouped widget must apply all its tabbed pages on an apply.
            void setApplyAll(bool apply_all);

            //! Sets if tabs should use icons of individual config pages.
            void setUseTabIcons(bool use_tab_icons);
            //! Gets if tabs should use icons of individual config pages.
            /*!
              False by default.
              */
            bool useTabIcons() const;

        signals:
             //! Signal emitted whenever a config page is applied.
             void appliedPage(IConfigPage* conig_page);
             //! Signal emitted whenever the active config page in the group changes.
             void activeGroupedPageChanged(IConfigPage* config_page);

        private slots:
             //! Slots which responds to changes to the active tab in the grouped tab widget.
             void handleCurrentPageChanged(int new_index);

        private:
            //! Creates a tab for a given page.
            void addPageTab(IConfigPage* page);
            //! Removes a tab for a given page.
            void removePageTab(IConfigPage* page);
            //! Gets the index in the tab widget for the specified page.
            int findTabIndex(IConfigPage* page);

            Ui::GroupedConfigPage *ui;
            GroupedConfigPageData* d;
        };
    }
}

#endif // GROUPED_CONFIG_PAGE
