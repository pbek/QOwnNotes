#!/usr/bin/env php
<?php
/**
 * Downloads all blog articles from the old webpage including their images
 */

require __DIR__ . '/vendor/autoload.php';
use League\HTMLToMarkdown\HtmlConverter;

const OUTPUT_DIR = "../src/blog/";
const IMAGE_DIR = "../src/.vuepress/public/img/blog/";

@mkdir(OUTPUT_DIR);
@mkdir(IMAGE_DIR);

$converter = new HtmlConverter();

//$xml = simplexml_load_string(file_get_contents("http://old.qownnotes.org/sitemap.xml"));
$xml = simplexml_load_string(file_get_contents("https://old.qownnotes.org/rss/feed/qownnotes-blog"));

foreach($xml->xpath("channel/item") as $item) {
    $title = (string) $item->title;
    $description = (string) $item->description;
    $pubDate = new \DateTime((string) $item->pubDate);

    $matches = [];
    $imageFrontMatter = "";
    if (preg_match('/<img src="(https:\/\/old.qownnotes.org\/.+\/([^\/]+?))"/', $description, $matches)) {
        $imageUrl = $matches[1];
        $fileName = $matches[2];

        $imageData = file_get_contents($imageUrl);
        file_put_contents(IMAGE_DIR . $fileName, $imageData);

        $description = str_replace($imageUrl, "/img/blog/" . $fileName, $description);
        $imageFrontMatter = "\nimage: /img/blog/$fileName";
    }

    $dateString = $pubDate->format("c");
    $order = $pubDate->format("YmdHis");
    $subHeaderDate = $pubDate->format("Y-m-d");
    $markdown = "---\ntitle: ${title}${imageFrontMatter}\ndate: ${dateString}\norder: ${order}\n---\n\n# ${title}\n\n" .
        "<v-subheader class=\"blog\">Date: ${subHeaderDate}</v-subheader>\n\n" .
        trim(preg_replace('/<.+?>/', '', $converter->convert($description)));
    $fileName = preg_replace('/[,!]/', '', $pubDate->format("Y-m-d") . "-" . str_replace(" ", "-", $title) . ".md");

    print("Creating $fileName...\n");
    file_put_contents(OUTPUT_DIR . $fileName, $markdown);
}
