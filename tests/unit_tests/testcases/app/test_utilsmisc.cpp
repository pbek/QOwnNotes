#include "test_utilsmisc.h"

#include <QTest>

#include "utils/misc.h"

using namespace Utils::Misc;

void TestUtilsMisc::initTestCase() {}

void TestUtilsMisc::testRemoveIfStartsWith() {
    QString s = QStringLiteral("--*--testString");
    QString result = removeIfStartsWith(s, QStringLiteral("--*--"));
    QString expected = QStringLiteral("testString");
    QVERIFY(result == expected);
}

void TestUtilsMisc::testRemoveIfEndsWith() {
    QString s = QStringLiteral("testString--*--");
    QString result = removeIfEndsWith(s, QStringLiteral("--*--"));
    QString expected = QStringLiteral("testString");
    QVERIFY(result == expected);
}

void TestUtilsMisc::testPrependIfDoesNotStartWith() {
    QString s = QStringLiteral("testString");
    QString result = prependIfDoesNotStartWith(s, QStringLiteral("--*--"));
    QString expected = QStringLiteral("--*--testString");
    QVERIFY(result == expected);
}

void TestUtilsMisc::testAppendIfDoesNotEndWith() {
    QString s = QStringLiteral("testString");
    QString result = appendIfDoesNotEndWith(s, QStringLiteral("--*--"));
    QString expected = QStringLiteral("testString--*--");
    QVERIFY(result == expected);
}

void TestUtilsMisc::testStartDetachedProcess() {
#ifdef Q_OS_UNIX
    auto result = startDetachedProcess("pwd", {});
    QVERIFY2(result == true, "Failed to start detached process");
#else
    QSKIP("This test is skipped on non unix OS");
#endif
}

void TestUtilsMisc::testShorten() {
    QString s = QStringLiteral("A Long test string with lots of words");
    const auto result = shorten(s, 10);
    QString expected = QStringLiteral("A Long ...");
    QVERIFY(result == expected);
}

void TestUtilsMisc::testCycleTextCase() {
    // test empty string
    QString s = "";
    QString result = cycleTextCase(s);
    QString expected = "";
    QVERIFY(result == expected);

    // lower
    s = "lower case sentence";
    result = cycleTextCase(s);
    expected = "LOWER CASE SENTENCE";
    QVERIFY(result == expected);

    // upper
    s = "LOWER CASE SENTENCE";
    result = cycleTextCase(s);
    expected = "Lower Case Sentence";
    QVERIFY(result == expected);

    // start case
    s = "Lower Case Sentence";
    result = cycleTextCase(s);
    expected = "Lower case sentence";
    QVERIFY(result == expected);

    // sentence case
    s = "Lower case sentence";
    result = cycleTextCase(s);
    expected = "lower case sentence";
    QVERIFY(result == expected);

    // random 1
    s = "LOWER CASE Sentence";
    result = cycleTextCase(s);
    expected = "lower case sentence";
    QVERIFY(result == expected);

    // random 2
    s = "lower CASE Sentence";
    result = cycleTextCase(s);
    expected = "lower case sentence";
    QVERIFY(result == expected);
}

void TestUtilsMisc::testHtmlToMarkdown() {
    QString html = "<head><meta charset=\"utf-8\"></head>";
    html += "<script>script</script>";
    html += "<style>style</style>";
    html += "<h1>Heading <em>italic</em></h1>";
    html += "<h2>Heading <strong>italic</strong></h2>";
    html += "<h3>Heading <em>italic</em></h3>";
    html += "<h4>Heading <em>italic</em></h4>";
    html += "<h5>Heading <em>italic</em></h5>";
    html += "<h6>Heading <em>italic</em></h6>";
    html += "<code>hello</code>";
    html += "<i>hello</i>";

    QString result = htmlToMarkdown(html);

    // Check that all expected elements are present
    QVERIFY(result.contains("# Heading *italic*"));
    QVERIFY(result.contains("## Heading **italic**"));
    QVERIFY(result.contains("### Heading *italic*"));
    QVERIFY(result.contains("#### Heading *italic*"));
    QVERIFY(result.contains("##### Heading *italic*"));
    QVERIFY(result.contains("###### Heading *italic*"));
    QVERIFY(result.contains("hello"));
    QVERIFY(!result.contains("<script>"));
    QVERIFY(!result.contains("<style>"));
    QVERIFY(!result.contains("<head>"));
}

void TestUtilsMisc::testHtmlToMarkdownTables() {
    QString html = R"(
<table>
    <tr>
        <th>Name</th>
        <th>Age</th>
        <th>City</th>
    </tr>
    <tr>
        <td>John</td>
        <td>30</td>
        <td>New York</td>
    </tr>
    <tr>
        <td>Jane</td>
        <td>25</td>
        <td>London</td>
    </tr>
</table>
)";

    QString result = htmlToMarkdown(html);

    // Check for table structure
    QVERIFY(result.contains("| Name | Age | City |"));
    QVERIFY(result.contains("| --- | --- | --- |"));
    QVERIFY(result.contains("| John | 30 | New York |"));
    QVERIFY(result.contains("| Jane | 25 | London |"));
}

void TestUtilsMisc::testHtmlToMarkdownTableSpecialChars() {
    QString html = R"(
<table>
    <tr>
        <th>Operator</th>
        <th>Description</th>
    </tr>
    <tr>
        <td>|</td>
        <td>Pipe operator</td>
    </tr>
    <tr>
        <td>||</td>
        <td>Logical OR</td>
    </tr>
</table>
)";

    QString result = htmlToMarkdown(html);

    // Pipes should be escaped in table cells
    QVERIFY(result.contains("| Operator | Description |"));
    QVERIFY(result.contains("\\|"));
}


void TestUtilsMisc::testParseTaskList() {
    const auto listTag = QStringLiteral("<li style=\"list-style-type:square\">");
    const QString &t1 = "<li> [ ] task 1</li>";
    const QString &r1 = parseTaskList(t1, true);
    QString expec = listTag +
                    " <a class=\"task-list-item-checkbox\" "
                    "href=\"checkbox://_0\">&#9744;</a> task 1</li>";
    QVERIFY(r1 == expec);

    const QString &t2 = "<li> [x] task 2</li>";
    const QString &r2 = parseTaskList(t2, true);
    expec = listTag +
            " <a class=\"task-list-item-checkbox\" "
            "href=\"checkbox://_0\">&#9745;</a> task 2</li>";
    QVERIFY(r2 == expec);

    const QString &t3 = "<li> [x] task 3</li>";
    const QString &r3 = parseTaskList(t3, false);
    expec = listTag + " &#9745; task 3</li>";
    QVERIFY(r3 == expec);

    const QString &t4 = "<li> [ ] task 4</li>";
    const QString &r4 = parseTaskList(t4, false);
    expec = listTag + " &#9744; task 4</li>";
    QVERIFY(r4 == expec);
}

void TestUtilsMisc::testUnescapeHtml() {
    QString html = "<h1>hello</h1>\n<p>hello</p>";
    QString result = unescapeHtml(html);
    QString expected = "hello\n\n\nhello";
    QVERIFY(result == expected);
}

void TestUtilsMisc::testHtmlSpecialChars() {
    QString str = "& \" \' < >";
    QString result = htmlspecialchars(str);
    QString expected = "&amp; &quot; &apos; &lt; &gt;";
    QVERIFY(result == expected);
}

void TestUtilsMisc::testToHumanReadableByteSize() {
    qint64 bytes = 8712492;
    QString result = toHumanReadableByteSize(bytes);
    QString expected = "8.31 MB";
    QVERIFY(result == expected);
}

void TestUtilsMisc::testTransformEvernoteImportCodeBlock() {
    QString content = R"(<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE en-export SYSTEM "http://xml.evernote.com/pub/evernote-export2.dtd">
<en-export export-date="20181021T174838Z" application="Evernote/Windows" version="6.x">
<note><title>test</title><content><![CDATA[<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE en-note SYSTEM "http://xml.evernote.com/pub/enml2.dtd">

<en-note><div>This is a test note with some code blocks</div><div style="box-sizing: border-box; padding: 8px; font-family: Monaco, Menlo, Consolas, &quot;Courier New&quot;, monospace; font-size: 12px; color: rgb(51, 51, 51); border-radius: 4px; background-color: rgb(251, 250, 248); border: 1px solid rgba(0, 0, 0, 0.15);-en-codeblock:true;"><div>&lt;?php</div><div><br/></div><div>phpinfo();</div><div><br/></div><div>?&gt;</div></div><div><br/></div></en-note>]]></content><created>20181021T111619Z</created><updated>20181021T111738Z</updated><note-attributes><author>myemail@example.com</author><source>desktop.win</source><source-application>evernote.win32</source-application></note-attributes></note></en-export>
)";

    QVERIFY(testEvernoteImportText(content) == R"(This is a test note with some code blocks

```
<?php

phpinfo();

?>
```)");
}

void TestUtilsMisc::testTransformEvernoteImportCodeBlock2() {
    QString content = R"(<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE en-export SYSTEM "http://xml.evernote.com/pub/evernote-export2.dtd">
<en-export export-date="20181029T161628Z" application="Evernote/Windows" version="6.x">
<note><title>File manager</title><content><![CDATA[<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE en-note SYSTEM "http://xml.evernote.com/pub/enml2.dtd">

<en-note><div><br/></div><div style="-en-codeblock: true; box-sizing: border-box; padding: 8px; font-family: Monaco, Menlo, Consolas, &quot;Courier New&quot;, monospace; font-size: 12px; color: rgb(51, 51, 51); border-top-left-radius: 4px; border-top-right-radius: 4px; border-bottom-right-radius: 4px; border-bottom-left-radius: 4px; background-color: rgb(251, 250, 248); border: 1px solid rgba(0, 0, 0, 0.14902); background-position: initial initial; background-repeat: initial initial;"><div>&nbsp; &nbsp; /**</div><div>&nbsp; &nbsp; &nbsp;* Gets the configuration path depending on the operation system</div><div>&nbsp; &nbsp; &nbsp;*</div><div>&nbsp; &nbsp; &nbsp;* @return bool|string</div><div>&nbsp; &nbsp; &nbsp;*/</div><div>&nbsp; &nbsp; private static function getConfigPath()</div><div>&nbsp; &nbsp; {</div><div>&nbsp; &nbsp; &nbsp; &nbsp; $file_manager = new pm_ServerFileManager();</div><div>&nbsp; &nbsp; &nbsp; &nbsp; if ($file_manager-&gt;fileExists(&apos;/etc/httpd/conf.d/pagespeed.conf&apos;)) {</div><div>&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; // CentOS</div><div>&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; return &apos;/etc/httpd/conf.d/pagespeed.conf&apos;;</div><div>&nbsp; &nbsp; &nbsp; &nbsp; } elseif ($file_manager-&gt;fileExists(&apos;/etc/apache2/mods-available/pagespeed.conf&apos;)) {</div><div>&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; // Ubuntu</div><div>&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; return &apos;/etc/apache2/mods-available/pagespeed.conf&apos;;</div><div>&nbsp; &nbsp; &nbsp; &nbsp; }</div><div>&nbsp; &nbsp; &nbsp; &nbsp; return false;</div><div>&nbsp; &nbsp; }</div></div><div><br/></div></en-note>]]></content><created>20170703T093015Z</created><updated>20170703T093022Z</updated><note-attributes><author>myemail@domain.com</author><source>desktop.win</source><source-application>evernote.win32</source-application></note-attributes></note></en-export>
)";

    QVERIFY(testEvernoteImportText(content) == R"(```
    /**

* Gets the configuration path depending on the operation system

*

* @return bool|string

*/

private static function getConfigPath()

{

$file_manager = new pm_ServerFileManager();

if ($file_manager->fileExists('/etc/httpd/conf.d/pagespeed.conf')) {

// CentOS

return '/etc/httpd/conf.d/pagespeed.conf';

} elseif ($file_manager->fileExists('/etc/apache2/mods-available/pagespeed.conf')) {

// Ubuntu

return '/etc/apache2/mods-available/pagespeed.conf';

}

return false;

}
```)");
}

void TestUtilsMisc::testTransformEvernoteImportRussian() {
    QString content = R"(<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE en-export SYSTEM "http://xml.evernote.com/pub/evernote-export2.dtd">
<en-export export-date="20191111T145250Z" application="Evernote/Windows" version="6.x">
<note><title>15-3.1</title><content><![CDATA[<?xml version="1.0" encoding="UTF-8"?><!DOCTYPE en-note SYSTEM "http://xml.evernote.com/pub/enml2.dtd"><en-note style="word-wrap: break-word; -webkit-nbsp-mode: space; -webkit-line-break: after-white-space;"><div>&#1059; &#1084;&#1077;&#1085;&#1103; &#1076;&#1086;&#1084;&#1072; &#1076;&#1074;&#1077; &#1082;&#1086;&#1084;&#1085;&#1072;&#1090;&#1099;, &#1082;&#1091;&#1093;&#1085;&#1103;, &#1090;&#1091;&#1072;&#1083;&#1077;&#1090; &#1080; &#1074;&#1072;&#1085;&#1085;&#1072;.</div></en-note>]]></content><created>20150714T065518Z</created><updated>20150714T073706Z</updated><note-attributes><source>mobile.android</source></note-attributes></note></en-export>
)";

    QVERIFY(testEvernoteImportText(content) ==
            R"(У меня дома две комнаты, кухня, туалет и ванна.)");
}

void TestUtilsMisc::testTransformEvernoteImportHyperlinks() {
    QString content = R"(<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE en-export SYSTEM "http://xml.evernote.com/pub/evernote-export2.dtd">
<en-export export-date="20210924T214029Z" application="Evernote/Windows" version="6.x">
<note><title>Fwd: Your 4 HIgh Value Trainings Access</title><content><![CDATA[<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE en-note SYSTEM "http://xml.evernote.com/pub/enml2.dtd">
<en-note bgcolor="#FFFFFF" text="#000000">
    <br clear="none"/>
    <div><br clear="none"/>
      <p>
        <a href="https://seanmize.infusionsoft.com/app/linkClick/24286/45b5a4e4c9f46ab6/13909690/64482e72b8e684a3">4 High Value Trainings Access</a>
      </p>
    </div>
    <br clear="none"/>
  </en-note>]]></content><created>20151005T190855Z</created><updated>20151005T190915Z</updated><tag>~ Reference</tag><tag>R - Purchases</tag><tag>RP - Sean Mize</tag><tag>RPSM - 4 High Value Trainings</tag><note-attributes><subject-date>20151005T190849Z</subject-date><author>Rick Smith &lt;rick@netguerrilla.org&gt;</author><source>mail.smtp</source><application-data key="mg-post-tier">1</application-data></note-attributes></note></en-export>
)";

    QVERIFY(
        testEvernoteImportText(content) ==
        R"([4 High Value Trainings Access](https://seanmize.infusionsoft.com/app/linkClick/24286/45b5a4e4c9f46ab6/13909690/64482e72b8e684a3))");
}

void TestUtilsMisc::testGetBaseUrlFromUrlString() {
    QString url = "https://www.example.com/path/to/file.html";
    QString baseUrl = "https://www.example.com";
    QString baseUrlWithBasePath = "https://www.example.com/path/to/";

    QVERIFY(getBaseUrlFromUrlString(url) == baseUrl);
    QVERIFY(getBaseUrlFromUrlString(url, true) == baseUrlWithBasePath);
}

void TestUtilsMisc::testCreateAbsolutePathsInHtml() {
    QString url = "https://www.example.com/path/to/file.html";
    QString html =
        R"(<html><head><title>Test</title></head><body><a href="/absolute.html">Link</a> <a href="relative.html">Link</a></body></html>)";
    QString expectedHtml =
        R"(<html><head><title>Test</title></head><body><a href="https://www.example.com/absolute.html">Link</a> <a href="https://www.example.com/path/to/relative.html">Link</a></body></html>)";

    QVERIFY(createAbsolutePathsInHtml(html, url) == expectedHtml);
}

void TestUtilsMisc::testDetectFileFormatTxt() {
    QString text =
        "This is a plain text file.\nIt doesn't match any of the specific formats.\nJust regular "
        "text content.";
    QCOMPARE(detectFileFormat(text), QString("txt"));
}

void TestUtilsMisc::testDetectFileFormatJson() {
    QString text = R"({
  "name": "John Doe",
  "age": 30,
  "email": "john@example.com",
  "address": {
    "street": "123 Main St",
    "city": "Anytown",
    "zip": "12345"
  },
  "phoneNumbers": [
    "555-1234",
    "555-5678"
  ]
})";
    QCOMPARE(detectFileFormat(text), QString("json"));
}

void TestUtilsMisc::testDetectFileFormatXml() {
    QString text = R"(<?xml version="1.0" encoding="UTF-8"?>
<root>
  <person id="1">
    <name>John Doe</name>
    <age>30</age>
    <email>john@example.com</email>
  </person>
  <person id="2">
    <name>Jane Smith</name>
    <age>28</age>
    <email>jane@example.com</email>
  </person>
</root>)";
    QCOMPARE(detectFileFormat(text), QString("xml"));
}

void TestUtilsMisc::testDetectFileFormatXmlWithoutDeclaration() {
    QString text = R"(<AppDeliveryID>12399_AE_W_Rsa_1</AppDeliveryID>
<Status>
<Code>0</Code>
<Text>SUCCESS</Text>
</Status>
<DualDeliveryID>131506</DualDeliveryID>
<ns4:AddressingResults/>)";
    QCOMPARE(detectFileFormat(text), QString("xml"));
}

void TestUtilsMisc::testDetectFileFormatHtml() {
    QString text = R"(<!DOCTYPE html>
<html>
<head>
    <title>Sample Page</title>
    <meta charset="utf-8">
</head>
<body>
    <h1>Hello World</h1>
    <p>This is a sample HTML page.</p>
    <ul>
        <li>Item 1</li>
        <li>Item 2</li>
    </ul>
</body>
</html>)";
    QCOMPARE(detectFileFormat(text), QString("html"));
}

void TestUtilsMisc::testDetectFileFormatCsv() {
    QString text = R"(Name,Age,Email,Department
John Doe,30,john@example.com,Engineering
Jane Smith,28,jane@example.com,Marketing
Bob Johnson,35,bob@example.com,Finance
Alice Williams,32,alice@example.com,HR)";
    QCOMPARE(detectFileFormat(text), QString("csv"));
}

void TestUtilsMisc::testDetectFileFormatIni() {
    QString text = R"([Database]
host=localhost
port=3306
user=root
password=secret

[Application]
debug=true
log_level=info
cache_enabled=1

; This is a comment
timeout=30)";
    QCOMPARE(detectFileFormat(text), QString("ini"));
}

void TestUtilsMisc::testDetectFileFormatNix() {
    QString text = R"({ pkgs ? import <nixpkgs> {} }:

with pkgs;

stdenv.mkDerivation {
  name = "my-application";
  version = "1.0.0";

  src = fetchurl {
    url = "https://example.com/my-application-1.0.0.tar.gz";
    sha256 = "0000000000000000000000000000000000000000000000000000";
  };

  buildInputs = [
    gcc
    cmake
    zlib
  ];

  configurePhase = ''
    cmake .
  '';

  buildPhase = ''
    make
  '';

  installPhase = ''
    make install
  '';
})";
    QCOMPARE(detectFileFormat(text), QString("nix"));
}

void TestUtilsMisc::testDetectFileFormatYaml() {
    QString text = R"(name: project-config
version: 1.0.0
description: Sample YAML configuration file

environment:
  production:
    database:
      host: db.example.com
      port: 5432
      user: admin
      password: secret
    cache:
      enabled: true
      ttl: 3600

  development:
    database:
      host: localhost
      port: 5432
      user: dev
      password: dev123
    cache:
      enabled: false

dependencies:
  - name: library1
    version: "^2.0.0"
  - name: library2
    version: "~1.5.0"

logging:
  level: info
  format: json
  path: /var/log/app.log)";
    QCOMPARE(detectFileFormat(text), QString("yaml"));
}

void TestUtilsMisc::testDetectFileFormatSql() {
    QString text = R"(CREATE TABLE employees (
    employee_id SERIAL PRIMARY KEY,
    first_name VARCHAR(50) NOT NULL,
    last_name VARCHAR(50) NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL,
    hire_date DATE NOT NULL,
    salary NUMERIC(10, 2),
    department_id INTEGER REFERENCES departments(id)
);

INSERT INTO employees (first_name, last_name, email, hire_date, salary, department_id)
VALUES
    ('John', 'Doe', 'john.doe@example.com', '2023-01-15', 75000.00, 1),
    ('Jane', 'Smith', 'jane.smith@example.com', '2023-02-20', 82000.00, 2),
    ('Bob', 'Johnson', 'bob.johnson@example.com', '2023-03-10', 65000.00, 1);

SELECT e.first_name, e.last_name, d.name AS department
FROM employees e
JOIN departments d ON e.department_id = d.id
WHERE e.salary > 70000
ORDER BY e.last_name ASC;)";
    QCOMPARE(detectFileFormat(text), QString("sql"));
}

void TestUtilsMisc::testDetectFileFormatJavaScript() {
    QString text = R"(// Sample JavaScript module for data processing
import { formatDate } from './utils.js';
import * as math from 'mathjs';

const CONFIG = {
  precision: 2,
  dateFormat: 'YYYY-MM-DD',
  threshold: 0.5
};

class DataAnalyzer {
  constructor(data) {
    this.data = data;
    this.results = null;
  }

  preprocess() {
    // Filter out invalid entries
    return this.data.filter(item => item.value !== null && !isNaN(item.value));
  }

  analyze() {
    const processed = this.preprocess();

    // Calculate statistics
    const values = processed.map(item => item.value);
    const result = {
      mean: math.mean(values).toFixed(CONFIG.precision),
      median: math.median(values).toFixed(CONFIG.precision),
      stdDev: math.std(values).toFixed(CONFIG.precision),
      timestamp: formatDate(new Date(), CONFIG.dateFormat)
    };

    this.results = result;
    return result;
  }

  generateReport() {
    if (!this.results) {
      this.analyze();
    }

    return `
      Data Analysis Report
      --------------------
      Date: ${this.results.timestamp}
      Sample size: ${this.data.length}
      Mean value: ${this.results.mean}
      Median value: ${this.results.median}
      Standard deviation: ${this.results.stdDev}
    `;
  }
}

function processDataset(dataset) {
  const analyzer = new DataAnalyzer(dataset);
  const results = analyzer.analyze();

  if (results.mean > CONFIG.threshold) {
    console.log("Warning: Mean value exceeds threshold");
  }

  return analyzer.generateReport();
}

export default {
  DataAnalyzer,
  processDataset
};)";
    QCOMPARE(detectFileFormat(text), QString("js"));
}

void TestUtilsMisc::testDetectFileFormatEdgeCases() {
    // Empty file
    QCOMPARE(detectFileFormat(""), QString("txt"));

    // Short text that doesn't match any format
    QCOMPARE(detectFileFormat("Hello world"), QString("txt"));

    // File with ambiguous format (INI vs CSV test case)
    QString ambiguousText = R"(zend_extension=xdebug.so
xdebug.remote_enable=1
xdebug.remote_autostart=1)";
    QCOMPARE(detectFileFormat(ambiguousText), QString("ini"));
}
