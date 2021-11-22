# Gestor de fragmentos de órdenes de terminal

Descárguese el [Gestor de fragmentos de órdenes de terminal de QOwnNotes](https://github.com/qownnotes/qc) para ejecutar órdenes guardadas en notas de QOwnNotes en la consola.

You can use notes with a special tag to store command snippets, which you can execute from the command-line snippet manager.

## Configuration

![socket-server-token](/img/socket-server-token.png)

Before you are using the snippet manager you need to enable the *Web socket server* (2) in the *Browser extension / command snippets* (1) settings in QOwnNotes.

Then you need to show the security token (3) and copy it (4).

Now open the configuration file of the snippet manager with:

```bash
qc configure
```

And put the security token in the `token` attribute:

```toml
[QOwnNotes]
token = "yourtokenhere"
```

