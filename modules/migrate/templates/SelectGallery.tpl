{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Import from Gallery 1"} </h2>

  <p class="giDescription">
    {g->text text="Copy all or part of an existing Gallery 1 installation into your Gallery 2.  It won't modify your Gallery 1 data in any way."}
  </p>
</div>

{if (!$SelectGallery.hasToolkit)}
<div class="gbBlock"><p class="giError">
  {capture name="url"}
    {g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminPlugins"}
  {/capture}
  {g->text text="You don't have any Graphics Toolkit activated to handle JPEG images.  If you import now, you will not have any thumbnails.  Visit the %sModules%s page to activate a Graphics Toolkit." arg1="<a href=\"`$smarty.capture.url`\">" arg2="</a>"}
</p></div>
{/if}

<div class="gbBlock">
  <h1 class="giTitle"> {g->text text="Path to Gallery 1 albums directory"} </h1>
  <p class="giDescription">
    <i>{g->text text="Example: /var/www/albums"}</i>
  </p>

  <div>
    <input type="text" size="60"
     name="{g->formVar var="form[albumsPath]"}" value="{$form.albumsPath}"
      id='giFormPath' autocomplete="off"/>
    {g->autoComplete element="giFormPath"}
      {g->url arg1="view=core.SimpleCallback" arg2="command=lookupDirectories"
	      arg3="prefix=__VALUE__" htmlEntities=false}
    {/g->autoComplete}

    {if isset($form.error.albumsPath.missing)}
    <div class="giError">
      {g->text text="You did not enter a path."}
    </div>
    {/if}
    {if isset($form.error.albumsPath.invalid)}
    <div class="giError">
      {g->text text="The path that you entered is invalid."}
    </div>
    {/if}
  </div>

  <span>
    {g->text text="<b>Note:</b> Before you import any data you should make sure your Gallery 1 is installed correctly by adding a photo through the Gallery 1 web interface.  Make sure you resolve any errors you see there first."}
  </span>

  {if !empty($SelectGallery.recentPaths)}
  <script type="text/javascript">
    // <![CDATA[
    function selectPath(path) {ldelim}
      document.getElementById('siteAdminForm').elements['{g->formVar
	var="form[albumsPath]"}'].value = path;
    {rdelim}
    // ]]>
  </script>

  <h4 class="giTitle">
    {g->text text="Recently Used Paths"}
  </h4>
  <p>
  {foreach from=$SelectGallery.recentPaths key=path item=count}
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

{if $SelectGallery.mapCount>0 || isset($status.mapDeleted)}
<div class="gbBlock">
  <h3> {g->text text="URL Redirection"} </h3>

  <p class="giDescription">
    {g->text text="Gallery can redirect old Gallery1 URLs to the new Gallery2 pages."}
  </p>

  {if $SelectGallery.mapCount>0}
    <span>
      {g->text one="There is one G1-&gt;G2 map entry" many="There are %d G1-&gt;G2 map entries"
	       count=$SelectGallery.mapCount arg1=$SelectGallery.mapCount}
    </span>
    &nbsp;
    <span>
      <a href="{g->url arg1="controller=migrate.SelectGallery" arg2="form[action][deleteMap]=1"}"
	 onclick="return confirm('{g->text text="Deleting map entries will cause old G1 URLs to produce HTTP Not Found errors instead of redirecting to G2 pages.  Delete all entries?"}')">
	{g->text text="Delete All"}
      </a>
    </span>
  {/if}
  {if isset($status.mapDeleted)}
    <p class="giError">
	{g->text text="Map entries deleted successfully"}
    </p>
  {/if}
  {include file="gallery:modules/migrate/templates/Redirect.tpl"}
</div>
{/if}
