[![Travis Build Status](https://travis-ci.org/litehtml/litehtml.svg?branch=master)](https://travis-ci.org/litehtml/litehtml)

# What is litehtml?

**litehtml** is the lightweight HTML rendering engine with CSS2/CSS3 support. Note that **litehtml** itself does not draw any text, pictures or other graphics and that **litehtml** does not depend on any image/draw/font library. You are free to use any library to draw images, fonts and any other graphics. **litehtml** just parses HTML/CSS and places the HTML elements into the correct positions (renders HTML). To draw the HTML elements you have to implement the simple callback interface [document_container](https://github.com/litehtml/litehtml/wiki/document_container). This interface is really simple, check it out! The [document_container](https://github.com/litehtml/litehtml/wiki/document_container) implementation is required to render HTML correctly.

# Where litehtml can be used

**litehtml** can be used when you need to show HTML formatted text or even to create a mini-browser, but using it as a full-featured HTML engine is not recommended. Usually you don't need something like WebKit to show simple HTML tooltips or HTML-formatted text, **litehtml** is much better for these as it's more lightweight and easier to integrate into your application.

## HTML Parser

**litehtml** uses the [gumbo-parser](https://github.com/google/gumbo-parser) to parse HTML. Gumbo is an implementation of the HTML5 parsing algorithm implemented as a pure C99 library with no outside dependencies. It's designed to serve as a building block for other tools and libraries such as linters, validators, templating languages, and refactoring and analysis tools.

## Compatibility

**litehtml** is compatible with any platform suported by C++ and STL. For Windows MS Visual Studio 2013 is recommended. **litehtml** supports both UTF-8 and Unicode strings on Windows and UTF-8 strings on Linux and Haiku.

## Support for HTML and CSS standards

Unfortunately **litehtml** is not fully compatible with HTML/CSS standards. There is lots of work to do to make **litehtml** work as well as modern browsers. But **litehtml** supports most HTML tags and CSS properties. You can find the list of supported CSS properties in  [this table](https://docs.google.com/spreadsheet/ccc?key=0AvHXl5n24PuhdHdELUdhaUl4OGlncXhDcDJuM1JpMnc&usp=sharing). For most simple usecases the HTML/CSS features supported by **litehtml** are enough. Right now **litehtml** supports even some pages with very complex HTML/CSS designs. As an example the pages created with [bootstrap framework](http://getbootstrap.com/) are usually well formatted by **litehtml**.

## Testing litehtml

You can [download the simple browser](http://www.litehtml.com/download.html) (**litebrowser**) to test the **litehtml** rendering engine.

The litebrowser source codes are available on GitHub:
  * [For Windows](https://github.com/litehtml/litebrowser)
  * [For Linux](https://github.com/litehtml/litebrowser-linux)
  * [For Haiku](https://github.com/adamfowleruk/litebrowser-haiku)

## License

**litehtml** is distributed under [New BSD License](https://opensource.org/licenses/BSD-3-Clause).
The **gumbo-parser** is disributed under [Apache License, Version 2.0](http://www.apache.org/licenses/LICENSE-2.0)

## Support litehtml project

If you like the work **litehtml** is doing please consider a small donation:

[ ![PayPal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif) ](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=UHBQG6EAFCRBA) [ ![Patreon](https://img.shields.io/badge/patreon-Become%20a%20Patron!-red.png?style=for-the-badge&logo=patreon) ](https://www.patreon.com/tordex)

Bitcoin: **1CS1174GVSLbP33TBp8RFwqPS6KmQK6kLY**

![BitCoin](https://www.tordex.com/assets/images/litehtml-bitcoin.png)
