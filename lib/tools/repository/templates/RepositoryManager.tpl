<!DOCTYPE html>
<html>
  <head>
    <title>{g->text text="Repository Manager"}</title>
    <link rel="stylesheet" type="text/css" href="templates/stylesheet.css"/>
  </head>
  <body>
    <h1>{g->text text="Repository Manager"}</h1>
    <div class="section">
      {g->text text="Plugins that have newer unpackaged versions are marked."}
    </div>

    <h2>{g->text text="Actions"}</h2>
    <div class="section">
    {if isset($upgradeRepositoryLink) }
      <a href="{$upgradeRepositoryLink}">{g->text text="Upgrade Repository"}</a>
      {g->text text="(package all marked plugins)"}
    {else}
      {g->text text="Repository is up-to-date."}
    {/if}
    </div>

    <h2>
      {g->text text="Plugins"}
    </h2>

    <div class="section">
      <table class="details" id="modules_listing">
        <tr>
          <th> {g->text text="Type"} </th>
          <th> {g->text text="Plugin Id"} </th>
          <th> {g->text text="Version"} </th>
          <th> {g->text text="Actions"} </th>
        </tr>
      {foreach from=$pluginInfo key=index item=plugin}
        <tr>
          <td>
            {$plugin.type}
          </td>
          <td>
            {$plugin.id}
          </td>
          <td>
            {$plugin.version}
          </td>
          <td>
          {foreach from=$plugin.actions key=action item=url}
            <a href="{$url}">{$action}</a>&nbsp;&nbsp;&nbsp;
          {/foreach}
          </td>
        </tr>
        {/foreach}
      </table>
    </div>

  </body>
</html>
