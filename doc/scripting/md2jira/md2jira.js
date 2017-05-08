#!/usr/bin/env nodejs

/*
 * Converts Markdown from Stdin to Jira code
 * 
 * Dependencies:
 * NodeJS
 * https://github.com/kylefarris/J2M
 * 
 * first you have to install jira2md:
 * npm install jira2md
 */

var fs = require('fs');
var md = fs.readFileSync('/dev/stdin').toString();
 
// Include the module 
var j2m = require('jira2md');
 
// If converting from Mardown to Jira Wiki Syntax: 
var jira = j2m.to_jira(md);

console.log(jira);
