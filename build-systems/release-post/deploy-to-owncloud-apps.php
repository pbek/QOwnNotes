<?php

require_once "vendor/autoload.php";
require_once "config.php";

$snoopy = new \Snoopy\Snoopy();

//$appId = 168497;
$appId = 174762;

//$snoopy->fetch("https://api.owncloud.com/v1/content/data/$appId?version=9x1x1x0");
//var_dump($snoopy->results);

// API doc: https://www.freedesktop.org/wiki/Specifications/open-collaboration-services
$url = "https://api.owncloud.com/v1/content/edit/$appId";
$vars = array(
    "action" => "release",
    "name" => "QOwnNotes Test",
    "type" => "922",
    "version" => "16.10.3",
    "changelog" => "This is a log.",
);

print "Posting to '$url'...\n";

$snoopy->user = $OWNCLOUD_API_KEY;
$snoopy->submit($url, $vars);
var_dump($snoopy->results);
var_dump($snoopy->error);
