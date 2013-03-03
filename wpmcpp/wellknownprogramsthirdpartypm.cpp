#include "wellknownprogramsthirdpartypm.h"
#include "wpmutils.h"

void WellKnownProgramsThirdPartyPM::scanDotNet(
        QList<InstalledPackageVersion *> *installed, Repository *rep) const
{
    // http://stackoverflow.com/questions/199080/how-to-detect-what-net-framework-versions-and-service-packs-are-installed

    WindowsRegistry wr;
    QString err = wr.open(HKEY_LOCAL_MACHINE,
            "Software\\Microsoft\\NET Framework Setup\\NDP", false, KEY_READ);
    if (err.isEmpty()) {
        QStringList entries = wr.list(&err);
        if (err.isEmpty()) {
            for (int i = 0; i < entries.count(); i++) {
                QString v_ = entries.at(i);
                Version v;
                if (v_.startsWith("v") && v.setVersion(
                        v_.right(v_.length() - 1))) {
                    WindowsRegistry r;
                    err = r.open(wr, v_, KEY_READ);
                    if (err.isEmpty())
                        detectOneDotNet(installed, rep, r, v_);
                }
            }
        }
    }
}

void WellKnownProgramsThirdPartyPM::detectOneDotNet(
        QList<InstalledPackageVersion *> *installed, Repository *rep,
        const WindowsRegistry& wr,
        const QString& keyName) const
{
    QString package("com.microsoft.DotNetRedistributable");
    Version keyVersion;

    Version oneOne(1, 1);
    Version four(4, 0);
    Version two(2, 0);

    Version v;
    bool found = false;
    if (keyName.startsWith("v") && keyVersion.setVersion(
            keyName.right(keyName.length() - 1))) {
        if (keyVersion.compare(oneOne) < 0) {
            // not yet implemented
        } else if (keyVersion.compare(two) < 0) {
            v = keyVersion;
            found = true;
        } else if (keyVersion.compare(four) < 0) {
            QString err;
            QString value_ = wr.get("Version", &err);
            if (err.isEmpty() && v.setVersion(value_)) {
                found = true;
            }
        } else {
            WindowsRegistry r;
            QString err = r.open(wr, "Full", KEY_READ);
            if (err.isEmpty()) {
                QString value_ = r.get("Version", &err);
                if (err.isEmpty() && v.setVersion(value_)) {
                    found = true;
                }
            }
        }
    }

    if (found) {
        PackageVersion* pv = new PackageVersion(package);
        pv->version = v;
        rep->savePackageVersion(pv);
        delete pv;

        Package* p = new Package(package, package);
        rep->savePackage(p);
        delete p;

        InstalledPackageVersion* ipv = new InstalledPackageVersion(package, v,
                WPMUtils::getWindowsDir());
        installed->append(ipv);
    }
}

void WellKnownProgramsThirdPartyPM::detectMSXML(
        QList<InstalledPackageVersion *> *installed, Repository *rep) const
{
    QScopedPointer<Package> p(
            new Package("com.microsoft.MSXML", "Microsoft XML library"));
    rep->savePackage(p.data());

    Version v = WPMUtils::getDLLVersion("msxml.dll");
    Version nullNull(0, 0);
    if (v.compare(nullNull) > 0) {
        QScopedPointer<PackageVersion> pv(new PackageVersion(p->name, v));
        rep->savePackageVersion(pv.data());
        installed->append(new InstalledPackageVersion(p->name, v,
                WPMUtils::getWindowsDir()));
    }
    v = WPMUtils::getDLLVersion("msxml2.dll");
    if (v.compare(nullNull) > 0) {
        QScopedPointer<PackageVersion> pv(new PackageVersion(p->name, v));
        rep->savePackageVersion(pv.data());
        installed->append(new InstalledPackageVersion(p->name, v,
                WPMUtils::getWindowsDir()));
    }
    v = WPMUtils::getDLLVersion("msxml3.dll");
    if (v.compare(nullNull) > 0) {
        v.prepend(3);
        QScopedPointer<PackageVersion> pv(new PackageVersion(p->name, v));
        rep->savePackageVersion(pv.data());
        installed->append(new InstalledPackageVersion(p->name, v,
                WPMUtils::getWindowsDir()));
    }
    v = WPMUtils::getDLLVersion("msxml4.dll");
    if (v.compare(nullNull) > 0) {
        QScopedPointer<PackageVersion> pv(new PackageVersion(p->name, v));
        rep->savePackageVersion(pv.data());
        installed->append(new InstalledPackageVersion(p->name, v,
                WPMUtils::getWindowsDir()));
    }
    v = WPMUtils::getDLLVersion("msxml5.dll");
    if (v.compare(nullNull) > 0) {
        QScopedPointer<PackageVersion> pv(new PackageVersion(p->name, v));
        rep->savePackageVersion(pv.data());
        installed->append(new InstalledPackageVersion(p->name, v,
                WPMUtils::getWindowsDir()));
    }
    v = WPMUtils::getDLLVersion("msxml6.dll");
    if (v.compare(nullNull) > 0) {
        QScopedPointer<PackageVersion> pv(new PackageVersion(p->name, v));
        rep->savePackageVersion(pv.data());
        installed->append(new InstalledPackageVersion(p->name, v,
                WPMUtils::getWindowsDir()));
    }
}

void WellKnownProgramsThirdPartyPM::detectWindows(
        QList<InstalledPackageVersion *> *installed, Repository *rep) const
{
    OSVERSIONINFO osvi;
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osvi);
    Version v;
    v.setVersion(osvi.dwMajorVersion, osvi.dwMinorVersion,
            osvi.dwBuildNumber);

    QScopedPointer<Package> p(new Package("com.microsoft.Windows",
            "Windows"));
    rep->savePackage(p.data());
    QScopedPointer<PackageVersion> pv(new PackageVersion(p->name, v));
    rep->savePackageVersion(pv.data());
    installed->append(new InstalledPackageVersion(p->name, v,
            WPMUtils::getWindowsDir()));

    if (!WPMUtils::is64BitWindows()) {
        QScopedPointer<Package> p32(new Package("com.microsoft.Windows32",
                "Windows 32 bit"));
        QScopedPointer<PackageVersion> pv32(new PackageVersion(p32->name, v));
        rep->savePackage(p32.data());
        rep->savePackageVersion(pv32.data());
        installed->append(new InstalledPackageVersion(p32->name, v,
                WPMUtils::getWindowsDir()));
    } else {
        QScopedPointer<Package> p64(new Package("com.microsoft.Windows64",
                "Windows 64 bit"));
        QScopedPointer<PackageVersion> pv64(new PackageVersion(p64->name, v));
        rep->savePackage(p64.data());
        rep->savePackageVersion(pv64.data());
        installed->append(new InstalledPackageVersion(p64->name, v,
                WPMUtils::getWindowsDir()));
    }
}

void WellKnownProgramsThirdPartyPM::detectJRE(
        QList<InstalledPackageVersion *> *installed, Repository *rep,
        bool w64bit) const
{
    if (w64bit && !WPMUtils::is64BitWindows())
        return;

    QString package = w64bit ? "com.oracle.JRE64" :
            "com.oracle.JRE";

    QScopedPointer<Package> p(new Package(package, package));
    rep->savePackage(p.data());

    WindowsRegistry jreWR;
    QString err = jreWR.open(HKEY_LOCAL_MACHINE,
            "Software\\JavaSoft\\Java Runtime Environment", !w64bit, KEY_READ);
    if (err.isEmpty()) {
        QStringList entries = jreWR.list(&err);
        for (int i = 0; i < entries.count(); i++) {
            QString v_ = entries.at(i);
            v_ = v_.replace('_', '.');
            Version v;
            if (!v.setVersion(v_) || v.getNParts() <= 2)
                continue;

            WindowsRegistry wr;
            err = wr.open(jreWR, entries.at(i), KEY_READ);
            if (!err.isEmpty())
                continue;

            QString path = wr.get("JavaHome", &err);
            if (!err.isEmpty())
                continue;

            QDir d(path);
            if (!d.exists())
                continue;

            QScopedPointer<PackageVersion> pv(new PackageVersion(package, v));
            rep->savePackageVersion(pv.data());

            installed->append(new InstalledPackageVersion(package, v, path));
        }
    }
}

void WellKnownProgramsThirdPartyPM::detectJDK(
        QList<InstalledPackageVersion *> *installed, Repository *rep,
        bool w64bit) const
{
    QString package = w64bit ? "com.oracle.JDK64" : "com.oracle.JDK";

    if (w64bit && !WPMUtils::is64BitWindows())
        return;

    QScopedPointer<Package> p(new Package(package, package));
    rep->savePackage(p.data());

    WindowsRegistry wr;
    QString err = wr.open(HKEY_LOCAL_MACHINE,
            "Software\\JavaSoft\\Java Development Kit",
            !w64bit, KEY_READ);
    if (err.isEmpty()) {
        QStringList entries = wr.list(&err);
        if (err.isEmpty()) {
            for (int i = 0; i < entries.count(); i++) {
                QString v_ = entries.at(i);
                WindowsRegistry r;
                err = r.open(wr, v_, KEY_READ);
                if (!err.isEmpty())
                    continue;

                v_.replace('_', '.');
                Version v;
                if (!v.setVersion(v_) || v.getNParts() <= 2)
                    continue;

                QString path = r.get("JavaHome", &err);
                if (!err.isEmpty())
                    continue;

                QDir d(path);
                if (!d.exists())
                    continue;

                QScopedPointer<PackageVersion> pv(
                        new PackageVersion(package, v));
                rep->savePackageVersion(pv.data());

                installed->append(new InstalledPackageVersion(package, v, path));
            }
        }
    }
}

void WellKnownProgramsThirdPartyPM::detectMicrosoftInstaller(
        QList<InstalledPackageVersion *> *installed, Repository *rep) const
{
    Version v = WPMUtils::getDLLVersion("MSI.dll");
    Version nullNull(0, 0);
    if (v.compare(nullNull) > 0) {
        QScopedPointer<Package> p(new Package("com.microsoft.WindowsInstaller",
                "Windows Installer"));
        rep->savePackage(p.data());

        QScopedPointer<PackageVersion> pv(new PackageVersion(p->name, v));
        rep->savePackageVersion(pv.data());

        installed->append(new InstalledPackageVersion(p->name, v,
                WPMUtils::getWindowsDir()));
    }
}

void WellKnownProgramsThirdPartyPM::scan(
        QList<InstalledPackageVersion *> *installed, Repository *rep) const
{
    detectWindows(installed, rep);
    scanDotNet(installed, rep);
    detectMSXML(installed, rep);
    detectMicrosoftInstaller(installed, rep);

    detectJRE(installed, rep, false);
    if (WPMUtils::is64BitWindows())
        detectJRE(installed, rep, true);
    detectJDK(installed, rep, false);
    if (WPMUtils::is64BitWindows())
        detectJDK(installed, rep, true);
}

