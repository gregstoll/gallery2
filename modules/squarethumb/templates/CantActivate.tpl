{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Square Thumbnail Module"} </h2>
</div>

<div class="gbBlock"><h2 class="giError">
  {capture name="url"}
    {g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminPlugins"}
  {/capture}
  {g->text text="This module requires you to have at least one graphics toolkit active in order for it to operate properly.  Please return to the %sModules%s page and activate a graphics toolkit." arg1="<a href=\"`$smarty.capture.url`\">" arg2="</a>"}
</h2></div>
