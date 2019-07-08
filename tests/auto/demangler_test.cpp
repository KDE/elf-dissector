/*
    Copyright (C) 2014 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <config-elf-dissector.h>

#include <QtTest/qtest.h>
#include <QObject>
#include <QDebug>

#include <demangle/demangler.h>

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
        QTest::newRow("operator 2") << "_ZNK10QByteArraycvPKcEv" << (VB << "QByteArray" << "operator char const*() const");
        QTest::newRow("operator 3") << "_ZN7QStringaSEOS_" << (VB << "QString" << "operator=(QString&&)");

#if BINUTILS_VERSION >= BINUTILS_VERSION_CHECK(2, 24)
        QTest::newRow("rvalue ref on this") << "_ZNO7QString11toLocal8BitEv" << (VB << "QString" << "toLocal8Bit() &&");
#endif

        QTest::newRow("template func 1") <<  "_Z13qGetPtrHelperI14QScopedPointerI11QObjectData21QScopedPointerDeleterIS1_EEENT_7pointerERKS5_" << (VB << "qGetPtrHelper" << "qGetPtrHelper<QScopedPointer<QObjectData, QScopedPointerDeleter<QObjectData>>>(QScopedPointer<QObjectData, QScopedPointerDeleter<QObjectData>> const&)");
        QTest::newRow("template func 2") << "_ZN23QXmlStreamWriterPrivate5writeILi4EEEvRAT__Kc" << (VB << "QXmlStreamWriterPrivate" << "write" << "write<4>(char const (&) [4])");
        QTest::newRow("template func 3") << "_ZSt4moveIRP11TreeMapItemEONSt16remove_referenceIT_E4typeEOS4_" << (VB << "std" << "move" << "move<TreeMapItem*&>(TreeMapItem*&)");

        QTest::newRow("function pointer template") << "_ZN20QGlobalStaticDeleterI5QListIPFP7QObjectvEEED1Ev" << (VB << "QGlobalStaticDeleter" << "QGlobalStaticDeleter<QList<QObject* (*)()>>" << "~QGlobalStaticDeleter()");

        QTest::newRow("lambda 1") << "_ZSt7find_ifIPKSt10shared_ptrI7ElfFileEZN10ElfFileSet7addFileERK7QStringEUlRS3_E_ET_SB_SB_T0_" << (VB << "std" << "find_if" << "find_if<std::shared_ptr<ElfFile> const*, ElfFileSet::addFile(QString const&)::{lambda(std::shared_ptr<ElfFile> const&)#1}>(ElfFileSet::addFile(QString const&)::{lambda(std::shared_ptr<ElfFile> const&)#1}, ElfFileSet::addFile(QString const&)::{lambda(std::shared_ptr<ElfFile> const&)#1}, ElfFileSet::addFile(QString const&)::{lambda(std::shared_ptr<ElfFile> const&)#1})");
        QTest::newRow("lambda 2") << "_ZZN13Ui_MainWindow7setupUiEP11QMainWindowENKUlvE5_clEv" << (VB << "Ui_MainWindow" << "setupUi(QMainWindow*)" << "{lambda()#7}" << "operator()() const");

        QTest::newRow("tmp 1") << "_Z27qRegisterNormalizedMetaTypeI7QVectorI10QByteArrayEEiRKS1_PT_N9QtPrivate21MetaTypeDefinedHelperIS5_Xaasr12QMetaTypeId2IS5_E7DefinedntsrSA_9IsBuiltInEE11DefinedTypeE" << (VB << "qRegisterNormalizedMetaType" << /*"qRegisterNormalizedMetaType<QVector<QByteArray>>(QByteArray const&, QVector<QByteArray>*, QtPrivate::MetaTypeDefinedHelper<QVector<QByteArray>, QMetaTypeId2<QVector<QByteArray>>::Defined&&(!QMetaTypeId2<QVector<QByteArray>>::IsBuiltIn)>::DefinedType)"*/ "qRegisterNormalizedMetaType<QVector<QByteArray>>(QByteArray const&, QVector<QByteArray>*, QtPrivate::MetaTypeDefinedHelper<QVector<QByteArray>, QMetaTypeId2<QVector<QByteArray>>::Defined&&!QMetaTypeId2<QVector<QByteArray>>::IsBuiltIn>::DefinedType)"); // TODO find a way to fix the parenthesis in unary/binary expressions
        QTest::newRow("tmp 2") << "_ZN7QObject7connectIM9QLineEditFvRK7QStringEZN10MainWindowC1EP7QWidgetEUlS4_E_EEN9QtPrivate9QEnableIfIXeqsrNSB_15FunctionPointerIT0_EE13ArgumentCountngLi1EEN11QMetaObject10ConnectionEE4TypeEPKNSD_IT_E6ObjectESK_PKS_SE_N2Qt14ConnectionTypeE" << (VB << "QObject" << "connect" << /*"connect<void (QLineEdit::*)(QString const&), MainWindow::MainWindow(QWidget*)::{lambda(QString const&)#1}>(QtPrivate::QEnableIf<void (QLineEdit::*)(QString const&)>::Object const*, QtPrivate::QEnableIf<MainWindow::MainWindow(QWidget*)::{lambda(QString const&)#1}::FunctionPointer<MainWindow::MainWindow(QWidget*)::{lambda(QString const&)#1}>::ArgumentCount==(-(1)), QMetaObject::Connection>::Type, QObject const*, MainWindow::MainWindow(QWidget*)::{lambda(QString const&)#1}::FunctionPointer, Qt::ConnectionType)"*/ "connect<void (QLineEdit::*)(QString const&), MainWindow::MainWindow(QWidget*)::{lambda(QString const&)#1}>(QtPrivate::QEnableIf<void (QLineEdit::*)(QString const&)>::Object const*, QtPrivate::QEnableIf<MainWindow::MainWindow(QWidget*)::{lambda(QString const&)#1}::FunctionPointer<MainWindow::MainWindow(QWidget*)::{lambda(QString const&)#1}>::ArgumentCount==-1, QMetaObject::Connection>::Type, QObject const*, MainWindow::MainWindow(QWidget*)::{lambda(QString const&)#1}::FunctionPointer, Qt::ConnectionType)"); // TODO fix parenthesis on -1

        QTest::newRow("pack 1") << "_ZSt12__get_helperILm0EPN2Ui10MainWindowEISt14default_deleteIS1_EEENSt9__add_refIT0_E4typeERSt11_Tuple_implIXT_EIS6_DpT1_EE" << (VB << "std" << "__get_helper" << "__get_helper<0ul, Ui::MainWindow*, std::default_delete<Ui::MainWindow>>(std::_Tuple_impl<0ul, Ui::MainWindow*, std::default_delete<Ui::MainWindow>>&)");
        QTest::newRow("pack 2") << "_ZN3WTF14NeverDestroyedINS_12_GLOBAL__N_125ARC4RandomNumberGeneratorEEC1IIEEEDpOT_" << (VB << "WTF" << "NeverDestroyed" << "NeverDestroyed<WTF::(anonymous namespace)::ARC4RandomNumberGenerator>" << "NeverDestroyed" << "NeverDestroyed<>()");

        QTest::newRow("vendor 1") << "_ZL18mergeQuestionMarksU8__vectorx" << (VB << "mergeQuestionMarks(long long __vector)");

        QTest::newRow("array no size") << "_ZN5boost6detail21sp_assert_convertibleIA_NS_18default_color_typeES3_EEvv" << (VB << "boost" << "detail" << "sp_assert_convertible" << "sp_assert_convertible<boost::default_color_type [], boost::default_color_type []>()");
        QTest::newRow("vector type") << "_ZN9QSimdSse25v_mulEDv4_fS0_" << (VB << "QSimdSse2" << "v_mul(float __vector(4), float __vector(4))");

        QTest::newRow("decltype in return") << "_ZNSt16allocator_traitsISaIiEE9constructIiJRKiEEEDTcl12_S_constructfp_fp0_spcl7forwardIT0_Efp1_EEERS0_PT_DpOS5_" << (VB << "std" << "allocator_traits" << "allocator_traits<std::allocator<int>>" << "construct" << "construct<int, int const&>(std::allocator<int>&, int*, int const&)");
        QTest::newRow("decltype with binary op") << "_ZSt5beginI7QVectorIP7ElfFileEEDTcldtfp_5beginEERT_" << (VB << "std" << "begin" << "begin<QVector<ElfFile*>>(QVector<ElfFile*>&)");

        QTest::newRow("typeinfo") << "_ZTI14ElfNodeVisitorIiE" << (VB << "ElfNodeVisitor" << "ElfNodeVisitor<int>" << "typeinfo");
        QTest::newRow("typeinfo name") << "_ZTS26KRecursiveFilterProxyModel" << (VB << "KRecursiveFilterProxyModel" << "typeinfo name");
        QTest::newRow("vtable") << "_ZTV17ElfDynamicSection" << (VB << "ElfDynamicSection" << "vtable");
        QTest::newRow("thunk") << "_ZThn16_N13TreeMapWidgetD0Ev" << (VB << "TreeMapWidget" << "~TreeMapWidget()" << "thunk");
        QTest::newRow("virtual thunk") << "_ZTv0_n24_N5Solid6Ifaces11OpticalDiscD1Ev" << (VB << "Solid" << "Ifaces" << "OpticalDisc" << "~OpticalDisc()" << "virtual thunk");
        QTest::newRow("covariant thunk") << "_ZTch0_h16_NK12ThreadWeaver15WeaverImplState6weaverEv" << (VB << "ThreadWeaver" << "WeaverImplState" << "weaver() const" << "covariant return thunk");
        QTest::newRow("vtt") << "_ZTTN5Solid6Ifaces13StorageVolumeE" << (VB << "Solid" << "Ifaces" << "StorageVolume" << "vtt");
        QTest::newRow("construction vtable") << "_ZTCN5Solid8Backends7UDisks211OpticalDiscE0_NS1_5BlockE" << (VB << "Solid" << "Backends" << "UDisks2" << "Block" << "construction vtable in Solid::Backends::UDisks2::OpticalDisc");

        QTest::newRow("guard variable") << "_ZGVZN12_GLOBAL__N_119Q_QGS_s_parsingData13innerFunctionEvE6holder" << (VB << "(anonymous namespace)" << "Q_QGS_s_parsingData" << "innerFunction()" << "holder" << "guard variable");
        QTest::newRow("reference temporary") << "_ZGRZNK16KateHighlighting10canBreakAtE5QChariE2sq0" << (VB << "KateHighlighting" << "canBreakAt(QChar, int) const" << "sq" << "reference temporary #0");
        QTest::newRow("unnamed type") << "_ZN24QVariantAnimationPrivateUt_D1Ev" << (VB << "QVariantAnimationPrivate" <<  "{unnamed type#1}" << "~QVariantAnimationPrivate()");

        QTest::newRow("literal bool") << "_ZNSt10_Iter_baseIPN3QV45ValueELb0EE7_S_baseES2_" << (VB << "std" << "_Iter_base" << "_Iter_base<QV4::Value*, false>" << "_S_base(QV4::Value*)");
        QTest::newRow("literal custom") << "_ZSt10_ConstructIN3JSC4Yarr13YarrGeneratorILNS1_18YarrJITCompileModeE0EE6YarrOpEIS5_EEvPT_DpOT0_" << (VB << "std" << "_Construct" << "_Construct<JSC::Yarr::YarrGenerator<(JSC::Yarr::YarrJITCompileMode)0>::YarrOp, JSC::Yarr::YarrGenerator<(JSC::Yarr::YarrJITCompileMode)0>::YarrOp>(JSC::Yarr::YarrGenerator<(JSC::Yarr::YarrJITCompileMode)0>::YarrOp*, JSC::Yarr::YarrGenerator<(JSC::Yarr::YarrJITCompileMode)0>::YarrOp&&)");
        QTest::newRow("literal neg int") << "_ZNK7WebCore17CSSPrimitiveValue15convertToLengthILin1EEENS_6LengthEPKNS_11RenderStyleES5_db" << (VB << "WebCore" << "CSSPrimitiveValue" << "convertToLength" << "convertToLength<-1>(WebCore::RenderStyle const*, WebCore::RenderStyle const*, double, bool) const");

        QTest::newRow("cloned component") << "_ZN7QVectorIjE16defaultConstructEPjS1_.isra.2" << (VB << "QVector" << "QVector<unsigned int>" << "defaultConstruct(unsigned int*, unsigned int*) [clone .isra.2]");

        QTest::newRow("nested types") << "_ZN7QVectorIPZN10MainWindow8loadFileERK7QStringE10SymbolNodeE4dataEv" << (VB << "QVector" << "QVector<MainWindow::loadFile(QString const&)::SymbolNode*>" << "data()");

        QTest::newRow("default argument") << "_ZZN8KDevelop18SourceFileTemplate22setTemplateDescriptionERK7QStringS3_Ed_NKUlvE_clEv" << (VB << "KDevelop" << "SourceFileTemplate" << "setTemplateDescription(QString const&, QString const&)" << "{default arg#1}" << "{lambda()#1}" << "operator()() const");

        QTest::newRow("abi tag") << "_ZNK5ZXing6Result4textB5cxx11Ev" << (VB << "ZXing" << "Result" << "text[abi:cxx11]() const");

        QTest::newRow("transaction clone") << "_ZGTtNSt11logic_errorC1EPKc.cold.14" << (VB << "std" << "logic_error" << "transaction clone for logic_error(char const*) [clone .cold.14]");

	QTest::newRow("initializer list") << "_Z1fP1BIXtl1ALi1EEEE" << (VB << "f(B<A{1}>*)");
	QTest::newRow("template parameter object") << "_ZTAXtl1ALi1EEE" << (VB << "template parameter object for A{1}");
    }

    void testDemangler()
    {
        QFETCH(QString, mangled);
        QFETCH(QVector<QByteArray>, expectedDemangled);

        Demangler d;
        auto actualDemangled = d.demangle(mangled.toLatin1());
        if (actualDemangled != expectedDemangled) {
            qDebug() << actualDemangled;
            qDebug() << expectedDemangled;
        }
        QEXPECT_FAIL("nested types", "bug in pointer handling", Continue);
        QCOMPARE(actualDemangled, expectedDemangled);
    }

    void testSymbolType_data()
    {
        QTest::addColumn<QByteArray>("symbol");
        QTest::addColumn<Demangler::SymbolType>("type");

        QTest::newRow("empty") << QByteArray("") << Demangler::SymbolType::Normal;
        QTest::newRow("C") << QByteArray("malloc") << Demangler::SymbolType::Normal;
        QTest::newRow("C++") << QByteArray("_ZN10QArrayData4dataEv") << Demangler::SymbolType::Normal;

        QTest::newRow("vtable") << QByteArray("_ZTV17ElfDynamicSection") << Demangler::SymbolType::VTable;
        QTest::newRow("typeinfo") << QByteArray("_ZTI14ElfNodeVisitorIiE") << Demangler::SymbolType::TypeInfo;
        QTest::newRow("typeinfo name") << QByteArray("_ZTS26KRecursiveFilterProxyModel") << Demangler::SymbolType::TypeInfoName;
//         QTest::newRow("thunk") << QByteArray("_ZThn16_N13TreeMapWidgetD0Ev") << Demangler::SymbolType::Thunk;
//         QTest::newRow("virtual thunk") << QByteArray("_ZTv0_n24_N5Solid6Ifaces11OpticalDiscD1Ev") << Demangler::SymbolType::VirtualThunk;
//         QTest::newRow("covariant thunk") << QByteArray("_ZTch0_h16_NK12ThreadWeaver15WeaverImplState6weaverEv") << Demangler::SymbolType::CovariantThunk;
        QTest::newRow("vtt") << QByteArray("_ZTTN5Solid6Ifaces13StorageVolumeE") << Demangler::SymbolType::VTT;
        QTest::newRow("construction vtable") << QByteArray("_ZTCN5Solid8Backends7UDisks211OpticalDiscE0_NS1_5BlockE") << Demangler::SymbolType::ConstructionVTable;
    }

    void testSymbolType()
    {
        QFETCH(QByteArray, symbol);
        QFETCH(Demangler::SymbolType, type);

        QCOMPARE(Demangler::symbolType(symbol), type);
    }
};

QTEST_MAIN(DemanglerTest)

#include "demangler_test.moc"
