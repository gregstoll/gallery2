{*
 * $Revision: 17503 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Slideshow Settings"} </h2>
</div>

{if !empty($status)}
<div class="gbBlock">
  {if !empty($status.installed)}
  <h2 class="giSuccess">{g->text text="Installed PicLens version %s" arg1=$status.installed}
  {elseif !empty($status.downloadFailed)}
  <h2 class="giError">{g->text text="An error occurred while downloading PicLens!"}
  {elseif !empty($status.uninstalled)}
  <h2 class="giSuccess">{g->text text="Uninstalled PicLens"}
  {/if}
  </h2>
</div>
{/if}

<div class="gbBlock">
  <h2> PicLens </h2>
  <p class="giDescription">
    {g->text text="Gallery can optionally use a Flash and JavaScript based slideshow from %s to provide users with the best possible viewing experience.  Gallery can install %s for you automatically, or you can choose to use an HTML slideshow." arg1="<a href=\"http://piclens.com\">PicLens</a>" arg2="PicLens"}
  </p>
  {if $AdminSlideshow.piclens.current}
  <h3> {g->text text="Version %s installed." arg1=$AdminSlideshow.piclens.current} </h3>

  {if $AdminSlideshow.piclens.update}
  <p>
    {g->text text="There's a newer version of PicLens available."}
  </p>
  {/if}
  {elseif !$AdminSlideshow.piclens.update}
  <h3 class="giError">
    {g->text text="PicLens downloads are currently unavailable.  Please try again later."}
  </h3>
  {/if}
</div>

{if $AdminSlideshow.piclens.current || $AdminSlideshow.piclens.update}
<div class="gbBlock gcBackground1">
  {if !$AdminSlideshow.piclens.current}
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][install]"}" value="{g->text text="Install PicLens version %s" arg1=$AdminSlideshow.piclens.update}"/>
  {else}
  {if $AdminSlideshow.piclens.update}
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][install]"}" value="{g->text text="Update to PicLens version %s" arg1=$AdminSlideshow.piclens.update}"/>
  {/if}
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][uninstall]"}" value="{g->text text="Uninstall PicLens"}"/>
  {/if}
</div>
{/if}
