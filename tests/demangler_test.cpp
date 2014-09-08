#include <QtTest/qtest.h>
#include <QObject>

#include "../demangle/demangler.cpp"

#define VB QVector<QByteArray>()

class DemanglerTest : public QObject
{
    Q_OBJECT
private slots:
    void testDemangler_data()
    {
        QTest::addColumn<QString>("mangled"); // should be QByteArray, but then we have to explicitly cast it below
        QTest::addColumn<QVector<QByteArray>>("expectedDemangled");

        QTest::newRow("empty") << "" << (VB << "");
        QTest::newRow("C 1") << "malloc" << (VB << "malloc");
        QTest::newRow("member func 1") << "_ZN10QArrayData4dataEv" << (VB << "QArrayData" << "data()");
        QTest::newRow("member func 2") << "_ZN10QByteArray6appendERKS_" << (VB << "QByteArray" << "append(QByteArray const&)");
        QTest::newRow("member func 3") << "_ZN10QByteArray6numberEii" << (VB << "QByteArray" << "number(int, int)");
        QTest::newRow("ctor 1") << "_ZN10QByteArrayC1EPKci" << (VB << "QByteArray" << "QByteArray(char const*, int)");
        QTest::newRow("copy ctor 1") << "_ZN10QByteArrayC1ERKS_" << (VB << "QByteArray" << "QByteArray(QByteArray const&)");
        QTest::newRow("copy ctor 2") << "_ZN10QByteArrayC2ERKS_" << (VB << "QByteArray" << "QByteArray(QByteArray const&)");
        QTest::newRow("dtor 1") << "_ZN10QByteArrayD1Ev" << (VB << "QByteArray" << "~QByteArray()");
        QTest::newRow("dtor 2") << "_ZN10QByteArrayD2Ev" << (VB << "QByteArray" << "~QByteArray()");
        QTest::newRow("operator 1") << "_ZN10QByteArraypLERKS_" << (VB << "QByteArray" << "operator+=(QByteArray const&)");

        QTest::newRow("template func 1") <<  "_Z13qGetPtrHelperI14QScopedPointerI11QObjectData21QScopedPointerDeleterIS1_EEENT_7pointerERKS5_" << (VB << "qGetPtrHelper" << "qGetPtrHelper<QScopedPointer<QObjectData, QScopedPointerDeleter<QObjectData>>>(QScopedPointer<QObjectData, QScopedPointerDeleter<QObjectData>> const&)");

        QTest::newRow("function pointer template") << "_ZN20QGlobalStaticDeleterI5QListIPFP7QObjectvEEED1Ev" << (VB << "QGlobalStaticDeleter" << "QGlobalStaticDeleter<QList<QObject* (*)()>>" << "~QGlobalStaticDeleter()");
    }

    void testDemangler()
    {
        QFETCH(QString, mangled);
        QFETCH(QVector<QByteArray>, expectedDemangled);

        Demangler d;
        auto actualDemangled = d.demangle(mangled.toLatin1());
        if (actualDemangled != expectedDemangled)
            qDebug() << actualDemangled << expectedDemangled;
        QCOMPARE(actualDemangled, expectedDemangled);
    }
};

QTEST_MAIN(DemanglerTest)

#include "demangler_test.moc"