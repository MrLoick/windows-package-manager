#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <windows.h>

#include <QMainWindow>
#include <qprogressdialog.h>
#include <qtimer.h>
#include "qmap.h"
#include <QModelIndex>

#include "packageversion.h"
#include "job.h"
#include "progressdialog.h"
#include "fileloader.h"

namespace Ui {
    class MainWindow;
}

const UINT WM_ICONTRAY = WM_USER + 1;

const UINT NIN_BALLOONSHOW = WM_USER + 2;
const UINT NIN_BALLOONHIDE = WM_USER + 3;
const UINT NIN_BALLOONTIMEOUT = WM_USER + 4;
const UINT NIN_BALLOONUSERCLICK = WM_USER + 5;
const UINT NIN_SELECT = WM_USER + 0;
const UINT NINF_KEY = 1;
const UINT NIN_KEYSELECT = NIN_SELECT or NINF_KEY;

/**
 * Main window.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
private:
    Ui::MainWindow *ui;

    FileLoader fileLoader;

    void addTextTab(const QString& title, const QString& text);
    void addJobsTab();
    void showDetails();
    void updateIcons();
    void updateActions();
    bool isUpdateEnabled(PackageVersion* pv);

    /**
     * Fills the table with known package versions.
     */
    void fillList();

    /**
     * @return selected package version or 0.
     */
    PackageVersion* getSelectedPackageVersion();

    /**
     * @param pv a version or 0
     */
    void selectPackageVersion(PackageVersion* pv);

    void updateStatusInDetailTabs();
public:
    static QMap<QString, QIcon> icons;

    /**
     * @param pv a package versioin
     * @return icon for the specified package
     */
    static QIcon getPackageVersionIcon(PackageVersion* pv);

    /**
     * This icon is used if a package does not define an icon.
     */
    static QIcon genericAppIcon;

    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool winEvent(MSG* message, long* result);

    /**
     * Prepares the UI after the constructor was called.
     */
    void prepare();

    /**
     * Blocks until the job is completed. Shows an error
     * message dialog if the job was completed with an
     * error.
     *
     * @param job a job
     * @return true if the job has completed successfully
     *     (no error and not cancelled)
     * @param title dialog title
     */
    bool waitFor(Job* job, const QString& title);

    void closeDetailTabs();
    void recognizeAndLoadRepositories();
protected:
    void changeEvent(QEvent *e);
    void process(QList<InstallOperation*>& install);
private slots:
    void on_actionScan_Hard_Drives_triggered();
    void on_actionShow_Details_triggered();
    void on_actionDownload_All_Files_triggered();
    void on_actionList_Installed_MSI_Products_triggered();
    void on_tabWidget_currentChanged(int index);
    void on_tabWidget_tabCloseRequested(int index);
    void on_tableWidget_doubleClicked(QModelIndex index);
    void on_actionTest_Repositories_triggered();
    void on_actionAbout_triggered();
    void on_actionCompute_SHA1_triggered();
    void on_actionTest_Download_Site_triggered();
    void on_actionUpdate_triggered();
    void on_actionSettings_triggered();
    void on_lineEditText_textChanged(QString );
    void on_comboBoxStatus_currentIndexChanged(int index);
    void on_actionGotoPackageURL_triggered();
    void onShow();
    void on_actionInstall_activated();
    void on_tableWidget_itemSelectionChanged();
    void on_actionUninstall_activated();
    void on_actionExit_triggered();
    void iconDownloaded(const FileLoaderItem& it);
    void on_actionReload_Repositories_triggered();
    void on_actionClose_Tab_triggered();
};

#endif // MAINWINDOW_H
