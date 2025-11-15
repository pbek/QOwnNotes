# QOwnNotes Documentation (VitePress)

This is the documentation website for QOwnNotes, now powered by VitePress.

## Getting Started

### Prerequisites

- Node.js 18+
- npm or yarn

### Installation

```bash
npm install
```

### Development

Start the development server:

```bash
npm run dev
```

The site will be available at `http://localhost:5173` (VitePress default port).

### Building

Build the static site:

```bash
npm run build
```

The built files will be in `src/.vitepress/dist`.

### Preview

Preview the built site locally:

```bash
npm run preview
```

## Project Structure

```
webpage/
├── src/
│   ├── .vitepress/
│   │   ├── config.mts          # Main configuration
│   │   ├── theme/
│   │   │   ├── index.ts        # Theme customization
│   │   │   └── custom.css      # Custom styles
│   │   ├── plugins/
│   │   │   └── sitemap.ts      # Sitemap generator
│   │   └── public/             # Static assets (symlinked)
│   ├── index.md                # English home page
│   ├── getting-started/        # Getting started docs
│   ├── installation/           # Installation guides
│   ├── contributing/           # Contribution guides
│   ├── scripting/              # Scripting documentation
│   ├── editor/                 # Editor features
│   ├── faq/                    # FAQ
│   ├── de/                     # German translation
│   ├── es/                     # Spanish translation
│   ├── fr/                     # French translation
│   ├── it/                     # Italian translation
│   ├── fa/                     # Persian translation
│   ├── nl/                     # Dutch translation
│   ├── hu/                     # Hungarian translation
│   ├── ar/                     # Arabic translation
│   ├── pl/                     # Polish translation
│   └── ko/                     # Korean translation
├── scripts/
│   ├── run-dev.sh              # Development script
│   └── run-build.sh            # Build script
└── package.json
```

## Supported Languages

The documentation is available in 11 languages:

- 🇬🇧 English (default)
- 🇩🇪 German (Deutsch)
- 🇪🇸 Spanish (Español)
- 🇫🇷 French (Français)
- 🇮🇹 Italian (Italiano)
- 🇮🇷 Persian (فارسی) - RTL
- 🇳🇱 Dutch (Nederlands)
- 🇭🇺 Hungarian (Magyar)
- 🇸🇦 Arabic (العربية) - RTL
- 🇵🇱 Polish (Polski)
- 🇰🇷 Korean (한국어)

## Features

- ⚡ Lightning fast development with Vite
- 🌍 Multi-language support
- 🎨 Dark mode support
- 🔍 Full-text search (Algolia)
- 📱 Responsive design
- ♿ RTL language support
- 🔗 Clean URLs
- 🗺️ Automatic sitemap generation
- 🎯 SEO optimized
- 📝 Code syntax highlighting
- 🖼️ Image zoom
- 📋 Code copy button

## Configuration

The main configuration file is `src/.vitepress/config.mts`. It includes:

- Navigation menus for all languages
- Sidebar configuration
- Search settings (Algolia)
- SEO meta tags
- Social media links
- Locale-specific settings

## Adding Content

### Creating a New Page

1. Create a markdown file in the appropriate directory
2. Add frontmatter with metadata:

```markdown
---
title: Page Title
description: Page description for SEO
---

# Content here
```

3. Add the page to the sidebar in `config.mts`

### Translating Content

1. Create the same file structure in the language directory (e.g., `de/`)
2. Translate the content
3. Update the sidebar configuration for that locale in `config.mts`

## Customization

### Styles

Custom styles are in `src/.vitepress/theme/custom.css`. You can:

- Override CSS variables
- Add custom styles
- Modify component styling

### Theme

The theme can be extended in `src/.vitepress/theme/index.ts`:

- Add Vue components
- Register global components
- Add custom layouts

## Migration Notes

This site was migrated from VuePress v1 to VitePress. See:

- `VITEPRESS_MIGRATION.md` - Migration process details
- `MIGRATION_COMPLETE.md` - Migration summary

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test locally with `npm run dev`
5. Submit a pull request

## License

See the main QOwnNotes LICENSE file.

## Links

- [QOwnNotes](https://www.qownnotes.org)
- [GitHub Repository](https://github.com/pbek/QOwnNotes)
- [VitePress Documentation](https://vitepress.dev)
