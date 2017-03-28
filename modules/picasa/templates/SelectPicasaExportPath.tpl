{*
 * $Revision: 16871 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Import from Picasa 2"} </h2>

  <p class="giDescription">
    {g->text text="This operation takes data (images, descriptions, orderings etc.) from <strong>Picasa 2</strong> and brings it into <strong>Gallery 2</strong>."}
  </p>
</div>

{if (!$SelectPicasaExportPath.hasToolkit)}
  <div class="gbBlock"><p class="giError">
    {capture name="url"}
      {g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminPlugins"}
    {/capture}
    {g->text text="You don't have any Graphics Toolkit activated to handle JPEG images.  If you import now, you will not have any thumbnails.  Visit the %sModules%s page to activate a Graphics Toolkit." arg1="<a href=\"`$smarty.capture.url`\">" arg2="</a>"}
  </p></div>
{/if}

<div>
  {g->hiddenFormVars}
  <input type="hidden" name="{g->formVar var="controller"}" value="picasa.SelectPicasaExportPath"/>
  <input type="hidden" name="{g->formVar var="form[formName]"}" value="{$form.formName}"/>
</div>

<div class="gbBlock">
  <h4> {g->text text="Destination Album"} </h4>
  <p class="giDescription">
    {g->text text="Please select the gallery Album you want to import your picasa album into."}
  </p>

  <select name="{g->formVar var="form[destinationAlbumId]"}">
    {foreach from=$SelectPicasaExportPath.g2AlbumTree item=album}
      <option value="{$album.data.id}"{if $form.destinationAlbumId==$album.data.id
       } selected="selected"{/if}>
        {"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"|repeat:$album.depth}--
        {$album.data.title|markup:strip|default:$album.data.pathComponent}
      </option>
    {/foreach}
  </select>

  <h4> {g->text text="Path or File"} </h4>
  <p class="giDescription">
    {g->text text="This process will import Data from a Picasa 2 XML-Export. You must enter the path to the directory with the Picasa 2 files.  Example: <i>/path/to/picasa2xml</i>"}
  </p>

  <input type="text" size="60"
    name="{g->formVar var="form[picasaXmlPath]"}" value="{$form.picasaXmlPath}"
    id='giFormPath' autocomplete="off"/>
  {g->autoComplete element="giFormPath"}
  {g->url arg1="view=core.SimpleCallback" arg2="command=lookupDirectories" arg3="prefix=__VALUE__"
    forJavascript="true"}
  {/g->autoComplete}

  {if isset($form.error.picasaXmlPath.missing)}
    <div class="giError">
      {g->text text="You did not enter a path."}
    </div>
  {/if}
  {if isset($form.error.picasaXmlPath.invalid)}
    <div class="giError">
      {g->text text="The path that you entered does not contain valid Picasa export data."}
    </div>
  {/if}

  {if !empty($SelectPicasaExportPath.recentPaths)}
    <script type="text/javascript">
      // <![CDATA[
        function selectPath(path) {ldelim}
          document.getElementById("siteAdminForm").elements['{g->formVar var="form[picasaXmlPath]"}'].value = path;
          {rdelim}
      // ]]>
    </script>

    <h4 class="giTitle">
      {g->text text="Recently Used Paths"}
    </h4>
    <p>
      {foreach from=$SelectPicasaExportPath.recentPaths key=path item=count}
        {capture name="escapedPath"}{$path|replace:"\\":"\\\\"}{/capture}
        <a href="javascript:selectPath('{$smarty.capture.escapedPath}')">{$path}</a>
        <br/>
      {/foreach}
    </p>
  {/if}
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
    name="{g->formVar var="form[action][select]"}" value="{g->text text="Select"}"/>
</div>
