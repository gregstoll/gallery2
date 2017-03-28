{*
 * $Revision: 17254 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html>
  <head>
    <title>{$item.title|markup:strip|default:$item.pathComponent}</title>
  </head>
  <body>
    <p>{g->image image=$image item=$item}</p>
    <p><a href="{g->url}">{$galleryTitle|markup:strip}</a></p>
  </body>
</html>
