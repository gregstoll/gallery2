<!DOCTYPE html>
<html>
  <head>
    <title>{g->text text="Package Removal Results"}</title>
    <link rel="stylesheet" type="text/css" href="templates/stylesheet.css"/>
  </head>
  <body>
    <h1>{g->text text="Package Removal Successful"}</h1>
    <div class="section">
      {g->text text="You can review the results below."}
    </div>

    <h2>
      {g->text text="Files Removed"}
    </h2>

    <div class="section">
      <table class="details" id="modules_listing">
        {if count($files) == 0}
        <tr>
          <td>
            {g->text text="No files have been removed."}
          </td>
        </tr>
        {/if}
        {foreach from=$files item=path}
        <tr>
          <td>
            {$path}
          </td>
        </tr>
        {/foreach}
      </table>
    </div>

  </body>
</html>
