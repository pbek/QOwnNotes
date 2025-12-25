# QOwnNotes Webpage

## Dev Dependencies

- [nodejs](https://nodejs.org/en/)

## Development

```shell
# clone the repository
git clone https://github.com/pbek/QOwnNotes.git

# change to the webpage directory
cd QOwnNotes/webpage

# install dependencies
npm install

# build the webpage in development mode with hot reloading
npm run dev
```

Then you can open the webpage in your browser at <http://0.0.0.0:8080/>.

Every change you make to the source code will be automatically reflected in the webpage.

If you run `npm run build` the webpage will be built in production mode in `webpage/src/.vuepress/dist`.

## Customization

- Styles are mainly stored in [index.scss](https://github.com/pbek/QOwnNotes/blob/main/webpage/src/.vuepress/styles/index.scss)
- Menus and configuration is stored are [config.js](https://github.com/pbek/QOwnNotes/blob/main/webpage/src/.vuepress/config.js)
