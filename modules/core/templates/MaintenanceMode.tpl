{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div id="gsContent" class="gcBorder1">
  <div class="gbBlock gcBackground1">
    <h2> {g->text text="Maintenance"} </h2>
  </div>

  <div class="gbBlock">
    <p class="giDescription">
      {g->text text="Site is temporarily down for maintenance."}
    </p>
    <a href="{g->url arg1="view=core.UserAdmin" arg2="subView=core.UserLogin"}">
      {g->text text="Admin Login"}
    </a>
  </div>
</div>
