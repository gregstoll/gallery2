<!DOCTYPE html>
<html>
  <head>
    <title>{g->text text="Package Results"}</title>
    <link rel="stylesheet" type="text/css" href="templates/stylesheet.css"/>
  </head>
  <body>
    <h1>{g->text text="Package Successful"}</h1>
    <div class="section">
      {g->text text="You can review the results below."}
    </div>

    <h2>
      {g->text text="Plugins Packaged"}
    </h2>

    <div class="section">
      <table class="details" id="modules_listing">
        <tr>
          <th> {g->text text="Plugin ID"} </th>
          <th> {g->text text="Written"} </th>
          <th> {g->text text="Skipped"} </th>
          <th> {g->text text="Error"} </th>
        </tr>
        {foreach from=$results item=plugin}
        <tr>
          <td>
            {$plugin.pluginId}
          </td>
          <td>
            {foreach from=$plugin.packageInfo.packagesWritten item=package}
              {$package}<br>
            {/foreach}
          </td>
          <td>
            {foreach from=$plugin.packageInfo.packagesSkipped item=package}
              {$package}<br>
            {/foreach}
          </td>
	  <td>
	    {if !empty($plugin.errors)}
	    <p class="Error">
	      {foreach from=$plugin.errors item=error}
	      {$error}<br/>
	      {/foreach}
	    </p>
	    {else}
	    &nbsp;
	    {/if}
	  </td>
        </tr>
        {/foreach}
      </table>
    </div>

  </body>
</html>
