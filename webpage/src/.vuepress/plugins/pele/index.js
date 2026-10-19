import { getDirname, path } from "vuepress/utils";

const __dirname = getDirname(import.meta.url);

const variants = ["desktop", "mobile"];

/**
 * Renders Mermaid code blocks with Pele (https://pele.run)
 *
 * Add "desktop" or "mobile" after the language (```mermaid mobile) to only
 * show a diagram on wide or narrow screens, for example to provide simpler
 * diagrams for phones.
 *
 * @param {object} [options]
 * @param {string[]} [options.languages] Code block languages to render as diagrams
 */
export const pelePlugin = ({ languages = ["mermaid"] } = {}) => ({
  name: "vuepress-plugin-pele",

  extendsMarkdown: (md) => {
    const fence = md.renderer.rules.fence;

    md.renderer.rules.fence = (tokens, idx, options, env, self) => {
      const token = tokens[idx];
      const [language, ...flags] = token.info.trim().split(/\s+/);

      if (!languages.includes(language)) {
        return fence(tokens, idx, options, env, self);
      }

      const variant = flags.find((flag) => variants.includes(flag));
      const variantAttribute = variant ? ` variant="${variant}"` : "";

      // Encode the source so it can't break out of the attribute or be
      // interpreted by the Vue template compiler
      return `<PeleDiagram code="${encodeURIComponent(token.content)}"${variantAttribute}></PeleDiagram>\n`;
    };
  },

  clientConfigFile: path.resolve(__dirname, "client.js"),
});
