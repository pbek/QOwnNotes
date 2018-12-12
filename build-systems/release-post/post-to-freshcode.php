<?php

require_once "vendor/autoload.php";
require_once "config.php";

$browser = new SimpleBrowser();
$page = $browser->get('http://freshcode.club/submit/qownnotes-test');
//var_dump( $page );

var_dump($browser->getFieldByName("description"));


var_dump($browser->setFieldByName('submitter', 'pbek'));
var_dump($browser->setFieldByName("version", "1.1"));
var_dump($browser->setFieldByName("changes", "lots of changes"));
var_dump($browser->setFieldByName('req[os]', '1'));
var_dump($browser->setFieldByName('req[cc]', '1'));
$page = $browser->clickSubmit('Submit Project/Release');
var_dump( $page );

