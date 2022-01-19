# QOwnNotes Webpage

## Dev Dependencies

- [yarn](https://yarnpkg.com/)
- [nodejs](https://nodejs.org/en/)

## Development

```shell
# clone the repository
git clone https://github.com/pbek/QOwnNotes.git

# change to the webpage directory
cd QOwnNotes/webpage

# install dependencies
yarn install

# build the webpage in development mode with hot reloading
yarn run dev
```

Then you can open the webpage in your browser at <http://0.0.0.0:8080/>.

Every change you make to the source code will be automatically reflected in the webpage.

If you run `yarn run build` the webpage will be built in production mode in `webpage/dist`.

## Customization

- Styles are mainly stored in [index.styl](https://github.com/pbek/QOwnNotes/blob/develop/webpage/src/.vuepress/styles/index.styl)
- Menus and configuration is stored are [config.js](https://github.com/pbek/QOwnNotes/blob/develop/webpage/src/.vuepress/config.js)
