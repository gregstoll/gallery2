{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Getid3 Module"} </h2>
</div>

<div class="gbBlock"><h2 class="giError">
  {capture name="url"}
    {g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminPlugins"}
  {/capture}
  {g->text text="This module does not work on Windows, yet."}
</h2></div>
