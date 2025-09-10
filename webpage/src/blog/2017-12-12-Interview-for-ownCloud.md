---
title: Interview for ownCloud
date: 2017-12-12T12:10:43+00:00
order: 20171212121043
---

# Interview for ownCloud

<v-subheader class="blog">Date: 2017-12-12</v-subheader>

**Emil** from [**ownCloud**](https://owncloud.org/) asked me for an interview about **QOwnNotes**. Here are his questions and my answers.

## What problem did you try to solve with QOwnNotes, why did you write it?

In late 2014 I needed a way to quickly take notes with one keyboard shortcut and the current date in the filename (until I have the time to change it to something better). The notes should be stored as simple text-files in a directory so I can sync them easily. The desktop-software needs to run on **every popular operating system**.

After trying out several text-editors I found none that could really do that in the way I liked it, so I wrote my own. :)

I chose **Qt** and **C++** as platform because as long-time KDE user I love the framework and always wanted to create a Qt application to "scratch my own itch".

Since then **405 new versions** of **QOwnNotes** were released.

I cannot remember if I used **ownCloud** before or because I created QOwnNotes, but I loved ownCloud because it was FOSS and I could host it myself and as PHP-developer it also was a great fit for me. ownCloud notes met most of my requirements for note-taking in the web at that time.

## Which three QOwnNotes scripts do you consider most useful?

- I love scripts to create new notes from text I copied from web-applications like **Note from Jira issue** and **Note from Bitrix24 task**
- others like the script **@tag tagging in note text** because with it they can store their tags inside the notes
- if you need to modify the way the user-interface looks you can do so with **Custom user interface styling** or **Custom note preview styling** if you want to do the same with the preview

## What do you have to know to write a QOwnNotes script?

**Scripts** are written in **JavaScript** (inside QML files). There are a lot of examples on and you can always use the scripting repository at as reference for new scripts.

There is a documentation of all the API calls on .

You can use as many private and local scripts as you like and if you want make a script public you can create a pull-request on .

## How secure is your encryption of the notes, what algorithm do you use?

Note **encryption** is by default done by the [Botan library](http://botan.randombit.net/) with **AES-256**.

You can also use your [own encryption](http://docs.qownnotes.org/en/develop/scripting/README.html#encryptionhook) with the help of the scripting engine. There are already scripts for encrypting notes with **PGP** (PGP Encryption) or [Keybase](https://keybase.io/) (Keybase Encryption).

I guess the main weak point of the encryption would be the password you are using to encrypt a note.

## What data wouldn't you trust it with?

If you encrypt a note you loose the ability to find text in it with the note-search functionality, because the encrypted note-text is stored base64-encoded in the note text-file. The headline isn't encrypted.

## Do you plan to include more Code highlighting?

Only for the preview if a fully fledged browser will be used for the preview somewhere in the future, which would have advantages (usage of JavaScript libraries in the preview, fewer html rendering errors) and disadvantages (bad backward compatibility, issues with printing/exporting, larger applications size, larger memory footprint, possibly slower rendering speed).
