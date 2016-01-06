/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company.  For licensing terms and
** conditions see http://www.qt.io/terms-conditions.  For further information
** use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file.  Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, The Qt Company gives you certain additional
** rights.  These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock-matchers.h"
#include "gmock/gmock.h"
#include "gtest-qt-printing.h"

#include <clangfixitoperation.h>
#include <fixitcontainer.h>

#include <utils/changeset.h>

#include <QFile>
#include <QVector>

using ClangBackEnd::FixItContainer;
using ClangCodeModel::ClangFixItOperation;

using ::testing::PrintToString;

namespace {

QString unsavedFileContent(const QString &unsavedFilePath)
{
    QFile unsavedFileContentFile(unsavedFilePath);
    const bool isOpen = unsavedFileContentFile.open(QFile::ReadOnly | QFile::Text);
    if (!isOpen)
        ADD_FAILURE() << "File with the unsaved content cannot be opened!";

    return QString::fromUtf8(unsavedFileContentFile.readAll());
}

MATCHER_P(MatchText, expectedText,
          std::string(negation ? "hasn't" : "has")
          + " expected text:\n" + PrintToString(expectedText))
{
    const ::ClangFixItOperation &operation = arg;
    QString resultText = operation.refactoringFileContent_forTestOnly();

    if (resultText != expectedText) {
        *result_listener << "\n" << resultText.toUtf8().constData();
        return false;
    }

    return true;
}

class ClangFixItOperation : public ::testing::Test
{
protected:
    Utf8String semicolonFilePath{TESTDATA_DIR"/diagnostic_semicolon_fixit.cpp", -1};
    Utf8String compareFilePath{TESTDATA_DIR"/diagnostic_comparison_fixit.cpp", -1};
    Utf8String diagnosticText{Utf8StringLiteral("expected ';' at end of declaration")};
    FixItContainer semicolonFixItContainer{Utf8StringLiteral(";"),
                                  {{semicolonFilePath, 3u, 13u},
                                   {semicolonFilePath, 3u, 13u}}};
    QString semicolonErrorFile{semicolonFilePath.toString()};
    QString semicolonExpectedFile{QString::fromUtf8(TESTDATA_DIR"/diagnostic_semicolon_fixit_expected.cpp")};
    QString compareWarningFile{compareFilePath.toString()};
    QString compareExpected1File{QString::fromUtf8(TESTDATA_DIR"/diagnostic_comparison_fixit_expected1.cpp")};
    QString compareExpected2File{QString::fromUtf8(TESTDATA_DIR"/diagnostic_comparison_fixit_expected2.cpp")};
    FixItContainer compareFixItContainer{Utf8StringLiteral("=="),
                                  {{compareFilePath, 4u, 11u},
                                   {compareFilePath, 4u, 12u}}};
    FixItContainer assignmentFixItContainerParenLeft{Utf8StringLiteral("("),
                                  {{compareFilePath, 4u, 9u},
                                   {compareFilePath, 4u, 9u}}};
    FixItContainer assignmentFixItContainerParenRight{Utf8StringLiteral(")"),
                                  {{compareFilePath, 4u, 14u},
                                   {compareFilePath, 4u, 14u}}};
};

TEST_F(ClangFixItOperation, Description)
{
    ::ClangFixItOperation operation(semicolonFilePath, diagnosticText, {semicolonFixItContainer});

    ASSERT_THAT(operation.description(),
                QStringLiteral("Apply Fix: expected ';' at end of declaration"));
}

TEST_F(ClangFixItOperation, AppendSemicolon)
{
    ::ClangFixItOperation operation(semicolonFilePath, diagnosticText, {semicolonFixItContainer});

    operation.perform();

    ASSERT_THAT(operation, MatchText(unsavedFileContent(semicolonExpectedFile)));
}

TEST_F(ClangFixItOperation, ComparisonVersusAssignmentChooseComparison)
{
    ::ClangFixItOperation operation(compareFilePath, diagnosticText, {compareFixItContainer});

    operation.perform();

    ASSERT_THAT(operation, MatchText(unsavedFileContent(compareExpected1File)));
}

TEST_F(ClangFixItOperation, ComparisonVersusAssignmentChooseParentheses)
{
    ::ClangFixItOperation operation(compareFilePath,
                                    diagnosticText,
                                    {assignmentFixItContainerParenLeft,
                                     assignmentFixItContainerParenRight});

    operation.perform();

    ASSERT_THAT(operation, MatchText(unsavedFileContent(compareExpected2File)));
}

}