```cpp
static document::ptr  document::createFromString(
	const estring&       str,
	document_container*  container,
	const string&        master_styles = litehtml::master_css,
	const string&        user_styles = "");
```

### Terminology:

**BOM encoding** is the encoding suggested by the byte-order-mark (BOM). Can be UTF-8, UTF-16LE, or UTF-16BE.
Cannot be UTF-32 because it is not a valid HTML encoding. See bom_sniff.

**meta encoding** is an HTML encoding suggested by a valid <meta> charset tag.

valid <meta> charset tag:

- must be inside \<head>
- must have one of these forms:
  - \<meta charset="utf-8"> or
  - \<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
- encoding name must be one of the encoding labels https://encoding.spec.whatwg.org/#names-and-labels (see get_encoding)

**HTTP encoding** is the encoding specified in HTTP Content-Type header https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Content-Type

**user override encoding** - when your program allows user to manually choose encoding for particular page or site

---

### Call without specifying encoding:

```cpp
createFromString(string, container);
```

where string is `std::string` or `char*`

- if BOM is present, BOM encoding will be used
- otherwise, if valid <meta> tag is present, meta encoding will be used
- otherwise, UTF-8 will be used

### Call with encoding, confidence is certain:

```cpp
createFromString({string, encoding::big5}, container)
```

- if BOM is present, BOM encoding will be used
- otherwise, Big5 will be used

**NOTE**: encoding from <meta> tag will be ignored

### Call with encoding, confidence is tentative (very rare, you probably don't need this):

```cpp
createFromString({string, encoding::big5, confidence::tentative}, container)
```

- if BOM is present, BOM encoding will be used
- otherwise, if valid <meta> tag is present, meta encoding will be used
- otherwise, Big5 will be used

---

User override encoding and HTTP encoding must be passed with confidence certain, if both are present user
override encoding should take precedence.

If both user override encoding and HTTP encoding are unspecified, your program may guess encoding by using
encoding of the page when it was last visited or by performing frequency analysis or by URL domain or
by current user locale or smth else. Any such encoding should be passed with confidence tentative.
The precedence of these guesses is specified in the encoding sniffing algorithm, see litehtml::encoding_sniffing_algorithm
and https://html.spec.whatwg.org/multipage/parsing.html#encoding-sniffing-algorithm

litehtml implements only the 3 steps of this algorithm:

1. set encoding to BOM encoding if BOM is present
2. prescan the input to determine its encoding - this only called if no BOM found and user didn't specify encoding or specified tentative encoding
3. return an implementation-defined default character encoding (UTF-8) - this only called if there is no BOM, user didn't specify an encoding and
   prescan failed.

If your program is displaying html files from the web it is recommended to detect HTTP encoding, because
it is not very unusual for web pages to have encoding specified only in HTTP header or meta encoding be different
from HTTP encoding (HTTP encoding takes the precedence in this case).
