<!DOCTYPE html>
<html>
  <head>
    <title>{g->text text="Gallery Repository Tools"}</title>
    <link rel="stylesheet" type="text/css" href="templates/stylesheet.css"/>
  </head>
  <body>
    <h1>{g->text text="Gallery Repository Tools"}</h1>
    <div class="section">
      {g->text text="Some nice description here."}
    </div>

    {if (!$isSiteAdmin)}
    <h2> <span class="error">{g->text text="ERROR!"}</span> </h2>
    <div class="section">
      {g->text text="You are not logged in as a Gallery site administrator so you are
      not allowed to run the repository tools."}
    </div>
    {/if}

    <h2>
      {g->text text="Actions"}
    </h2>

    <div class="section">
      <a href="{g->url arg1="controller=PackagePlugin" arg2="action=showAvailablePlugins"}">
        {g->text text="Package Plugin"}
      </a>
    </div>
    <div class="section">
      <a href="{g->url arg1="controller=PackagePlugin" arg2="action=packagePlugins"}">
        {g->text text="Package All Plugins"}
      </a>{g->text text=" (long process)"}
    </div>
    <div class="section">
      <a href="{g->url arg1="controller=RepositoryManager" arg2="action=browse"}">
        {g->text text="Manage Repository"}
      </a>
    </div>
    <div class="section">
      <a href="{g->url arg1="controller=IndexGenerator" arg2="action=generate"}">
        {g->text text="Regenerate Index"}
      </a>
    </div>
  </body>
</html>
