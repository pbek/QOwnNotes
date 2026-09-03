#pragma once

#include <QObject>

#ifdef LANGUAGETOOL_ENABLED

class TestLanguageTool : public QObject {
    Q_OBJECT

   private Q_SLOTS:
    void initTestCase();

    // Self-hosted / free LanguageTool server (e.g. run locally on
    // http://localhost:8081), which requires no credentials.
    void testFreeLocalServerCheck();
    void testFreeServerDoesNotSendCredentials();

    // LanguageTool Premium, authenticated via "username" + "apiKey".
    void testPremiumServerSendsUsernameAndApiKey();
    void testPremiumServerParsesMatches();

    void testWhitespaceRuleIsFiltered();
    void testInvalidServerUrlEmitsError();
    void testServerUrlWithExistingV2CheckPathIsNotDuplicated();
    void testServerUrlWithTrailingSlash();
    void testConnectionTimeoutEmitsError();
};

#endif
