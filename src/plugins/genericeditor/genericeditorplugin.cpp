/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
**************************************************************************/

#include "genericeditorplugin.h"
#include "highlightdefinition.h"
#include "highlightdefinitionhandler.h"
#include "highlighter.h"
#include "highlighterexception.h"
#include "genericeditorconstants.h"
#include "editor.h"
#include "editorfactory.h"

#include <coreplugin/icore.h>
#include <coreplugin/mimedatabase.h>
#include <utils/qtcassert.h>

#include <QtCore/QtPlugin>
#include <QtCore/QLatin1String>
#include <QtCore/QFile>
#include <QtXml/QXmlSimpleReader>
#include <QtXml/QXmlInputSource>

using namespace GenericEditor;
using namespace Internal;

// Todo: Temp.
const QLatin1String GenericEditorPlugin::kAlertDefinitionId(":/genericeditor/XML/alert.xml");
const QLatin1String GenericEditorPlugin::kCDefinitionId(":/genericeditor/XML/c.xml");
const QLatin1String GenericEditorPlugin::kCppDefinitionId(":/genericeditor/XML/cpp.xml");
const QLatin1String GenericEditorPlugin::kCssDefinitionId(":/genericeditor/XML/css.xml");
const QLatin1String GenericEditorPlugin::kDoxygenDefinitionId(
        ":/genericeditor/XML/doxygen.xml");
const QLatin1String GenericEditorPlugin::kFortranDefinitionId(
        ":/genericeditor/XML/fortran.xml");
const QLatin1String GenericEditorPlugin::kHtmlDefinitionId(":/genericeditor/XML/html.xml");
const QLatin1String GenericEditorPlugin::kJavaDefinitionId(":/genericeditor/XML/java.xml");
const QLatin1String GenericEditorPlugin::kJavadocDefinitionId(
        ":/genericeditor/XML/javadoc.xml");
const QLatin1String GenericEditorPlugin::kJavascriptDefinitionId(
        ":/genericeditor/XML/javascript.xml");
const QLatin1String GenericEditorPlugin::kObjectiveCDefinitionId(
        ":/genericeditor/XML/objectivec.xml");
const QLatin1String GenericEditorPlugin::kPerlDefinitionId(":/genericeditor/XML/perl.xml");
const QLatin1String GenericEditorPlugin::kPhpDefinitionId(":/genericeditor/XML/php.xml");
const QLatin1String GenericEditorPlugin::kPythonDefinitionId(":/genericeditor/XML/python.xml");
const QLatin1String GenericEditorPlugin::kRubyDefinitionId(":/genericeditor/XML/ruby.xml");
const QLatin1String GenericEditorPlugin::kSqlDefinitionId(":/genericeditor/XML/sql.xml");
const QLatin1String GenericEditorPlugin::kTclDefinitionId(":/genericeditor/XML/tcl.xml");

GenericEditorPlugin *GenericEditorPlugin::m_instance = 0;

GenericEditorPlugin::GenericEditorPlugin() :
    m_actionHandler(0)
{
    QTC_ASSERT(!m_instance, return);
    m_instance = this;

    //Todo

    m_idByMimeType.insert(GenericEditor::Constants::C_HEADER_MIMETYPE, kCDefinitionId);
    m_idByMimeType.insert(GenericEditor::Constants::C_SOURCE_MIMETYPE, kCDefinitionId);
    m_idByMimeType.insert(GenericEditor::Constants::CPP_HEADER_MIMETYPE, kCppDefinitionId);
    m_idByMimeType.insert(GenericEditor::Constants::CPP_SOURCE_MIMETYPE, kCppDefinitionId);
    m_idByMimeType.insert(GenericEditor::Constants::CSS_MIMETYPE, kCssDefinitionId);
    m_idByMimeType.insert(GenericEditor::Constants::FORTRAN_MIMETYPE, kFortranDefinitionId);
    m_idByMimeType.insert(GenericEditor::Constants::HTML_MIMETYPE, kHtmlDefinitionId);
    m_idByMimeType.insert(GenericEditor::Constants::JAVA_MIMETYPE, kJavaDefinitionId);
    m_idByMimeType.insert(GenericEditor::Constants::JAVASCRIPT_MIMETYPE, kJavascriptDefinitionId);
    m_idByMimeType.insert(GenericEditor::Constants::OBJECTIVEC_MIMETYPE, kObjectiveCDefinitionId);
    m_idByMimeType.insert(GenericEditor::Constants::PERL_MIMETYPE, kPerlDefinitionId);
    m_idByMimeType.insert(GenericEditor::Constants::PHP_MIMETYPE, kPhpDefinitionId);
    m_idByMimeType.insert(GenericEditor::Constants::PYTHON_MIMETYPE, kPythonDefinitionId);
    m_idByMimeType.insert(GenericEditor::Constants::RUBY_MIMETYPE, kRubyDefinitionId);
    m_idByMimeType.insert(GenericEditor::Constants::SQL_MIMETYPE, kSqlDefinitionId);
    m_idByMimeType.insert(GenericEditor::Constants::TCL_MIMETYPE, kTclDefinitionId);

    m_idByName.insert(QLatin1String("alerts"), kAlertDefinitionId);
    m_idByName.insert(QLatin1String("c"), kCDefinitionId);
    m_idByName.insert(QLatin1String("cpp"), kCppDefinitionId);
    m_idByName.insert(QLatin1String("css"), kCssDefinitionId);
    m_idByName.insert(QLatin1String("doxygen"), kDoxygenDefinitionId);
    m_idByName.insert(QLatin1String("fortran"), kFortranDefinitionId);
    m_idByName.insert(QLatin1String("html"), kHtmlDefinitionId);
    m_idByName.insert(QLatin1String("java"), kJavaDefinitionId);
    m_idByName.insert(QLatin1String("javadoc"), kJavadocDefinitionId);
    m_idByName.insert(QLatin1String("javascript"), kJavascriptDefinitionId);
    m_idByName.insert(QLatin1String("objectivec"), kObjectiveCDefinitionId);
    m_idByName.insert(QLatin1String("perl"), kPerlDefinitionId);
    m_idByName.insert(QLatin1String("php"), kPhpDefinitionId);
    m_idByName.insert(QLatin1String("python"), kPythonDefinitionId);
    m_idByName.insert(QLatin1String("ruby"), kRubyDefinitionId);
    m_idByName.insert(QLatin1String("sql"), kSqlDefinitionId);
    m_idByName.insert(QLatin1String("tcl"), kTclDefinitionId);
}

GenericEditorPlugin::~GenericEditorPlugin()
{
    delete m_actionHandler;
    m_instance = 0;
}

GenericEditorPlugin *GenericEditorPlugin::instance()
{ return m_instance; }

bool GenericEditorPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments)

    Core::ICore *core = Core::ICore::instance();
    if (!core->mimeDatabase()->addMimeTypes(
            QLatin1String(":/genericeditor/GenericEditor.mimetypes.xml"), errorString)) {
        return false;
    }

    addAutoReleasedObject(new EditorFactory(this));

    m_actionHandler = new TextEditor::TextEditorActionHandler(
        GenericEditor::Constants::GENERIC_EDITOR,
        TextEditor::TextEditorActionHandler::UnCommentSelection);
    m_actionHandler->initializeActions();

    return true;
}

void GenericEditorPlugin::extensionsInitialized()
{}

void GenericEditorPlugin::initializeEditor(Editor *editor)
{
    m_actionHandler->setupActions(editor);
}

QString GenericEditorPlugin::definitionIdByName(const QString &name) const
{ return m_idByName.value(name.toLower()); }

QString GenericEditorPlugin::definitionIdByMimeType(const QString &mimeType) const
{ return m_idByMimeType.value(mimeType.toLower()); }

bool GenericEditorPlugin::isBuildingDefinition(const QString &id) const
{ return m_isBuilding.contains(id); }

const QSharedPointer<HighlightDefinition> &GenericEditorPlugin::definition(const QString &id)
{
    if (!m_definitions.contains(id)) {
        m_isBuilding.insert(id);

        QFile definitionFile(id);
        if (!definitionFile.open(QIODevice::ReadOnly | QIODevice::Text))
            throw HighlighterException();
        QXmlInputSource source(&definitionFile);

        QSharedPointer<HighlightDefinition> definition(new HighlightDefinition);
        HighlightDefinitionHandler handler(definition);

        QXmlSimpleReader reader;
        reader.setContentHandler(&handler);
        reader.parse(source);

        m_definitions.insert(id, definition);
        definitionFile.close();
        m_isBuilding.remove(id);
    }

    return *m_definitions.constFind(id);
}

Q_EXPORT_PLUGIN(GenericEditorPlugin)
