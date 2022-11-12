<!DOCTYPE html>
<html>
  <head>
    <title>{g->text text="Index Generation Results"}</title>
    <link rel="stylesheet" type="text/css" href="templates/stylesheet.css"/>
  </head>
  <body>
    <h1>{g->text text="Index Successfully Generated"}</h1>
    <div class="section">
      {g->text text="The index was created in %s. It contains %s modules and %s
      themes. Its contents can be reviewed " arg1=$outputDir arg2=$moduleCount arg3=$themeCount}
      <a href="{$browseRepositoryLink}">{g->text text="here."}</a>
    </div>
  </body>
</html>
