import { createWriteStream } from 'node:fs'
import { resolve } from 'node:path'
import { SiteConfig } from 'vitepress'

const links: { url: string; lastmod?: number }[] = []

export const sitemap = {
  name: 'sitemap',
  async buildEnd(config: SiteConfig) {
    const sitemap = `<?xml version="1.0" encoding="UTF-8"?>
<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">
${links
  .map(
    ({ url, lastmod }) => `  <url>
    <loc>https://www.qownnotes.org${url}</loc>
    ${lastmod ? `<lastmod>${new Date(lastmod).toISOString()}</lastmod>` : ''}
  </url>`
  )
  .join('\n')}
</urlset>`

    const sitemapPath = resolve(config.outDir, 'sitemap.xml')
    const writeStream = createWriteStream(sitemapPath)
    writeStream.write(sitemap)
    writeStream.end()
  },
  transformPageData(pageData: any) {
    links.push({
      url: pageData.relativePath.replace(/((^|\/)index)?\.md$/, '$2'),
      lastmod: pageData.lastUpdated
    })
  }
}

