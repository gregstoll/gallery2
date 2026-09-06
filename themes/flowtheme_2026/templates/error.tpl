{* flowtheme_2026 - error page *}
<!DOCTYPE html>
<html lang="{g->language}">
  <head>
    <meta charset="utf-8"/>
    <meta name="viewport" content="width=device-width, initial-scale=1"/>
    <meta name="color-scheme" content="light dark"/>
    {g->head}
    {if empty($head.title)}<title>{g->text text="Error"}</title>{/if}
    <link rel="stylesheet" type="text/css" href="{g->theme url="theme.css"}"/>
  </head>
  <body class="gallery">
    <main class="ft-main">
      <div class="ft-panel ft-error">
        <h1>{g->text text="Error"}</h1>
        {include file="gallery:`$theme.errorTemplate`" l10Domain=$theme.errorL10Domain}
        <p><a href="{g->url}">{g->text text="Back to the gallery"}</a></p>
      </div>
    </main>
    {g->trailer}
  </body>
</html>
