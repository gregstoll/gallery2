<!DOCTYPE html>
<html>
  <head>
    <title>{g->text text="Package Plugins"}</title>
    <link rel="stylesheet" type="text/css" href="templates/stylesheet.css"/>
  </head>
  <body>
    <h1>{g->text text="Package Plugins"}</h1>
    <div class="section">
      {g->text text="Package specific plugins (regexp):"}
      <p>
        <form>
          <input class="textbox" type="text" name="{$formVariablePrefix}filter" size="60" value="" />
          <input type="hidden" name="{$formVariablePrefix}controller" value="PackagePlugin" />
          <input type="hidden" name="{$formVariablePrefix}action" value="packagePlugins" />
          {g->hiddenFormVars}
          <input type="submit" value="{g->text text="Package"}" />
        </form>
      </p>
    </div>

    <h2>
      {g->text text="Plugins"}
    </h2>

    <div class="section" style="width: 100%">
      <table class="details" id="modules_listing">
        <tr>
          <th> {g->text text="Type"} </th>
          <th> {g->text text="Plugin Id"} </th>
          <th> {g->text text="Status"} </th>
          <th> {g->text text="Actions"} </th>
        </tr>
        {foreach from=$plugins key=pluginId item=plugin}
        <tr>
          <td>
            {$plugin.type}
          </td>
          <td>
            {$pluginId}
          </td>
          <td>
            {$plugin.active}, {$plugin.available}
          </td>
          {foreach from=$plugin.links key=action item=url}
          <td>
            <a href="{$url}">{$action}</a>
          </td>
          {/foreach}
        </tr>
        {/foreach}
      </table>
    </div>

  </body>
</html>
