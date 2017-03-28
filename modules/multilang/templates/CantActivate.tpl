{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="MultiLanguage Settings"} </h2>
</div>

<div class="gbBlock">
    <div class="giWarning">
      {capture name="gettext"}
	<a href="http://php.net/gettext">{g->text text="gettext"}</a>
      {/capture}
      {g->text text="Your webserver does not support localization.  Please instruct your system administrator to reconfigure PHP with the %s option enabled." arg1=$smarty.capture.gettext}
    </div>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][continue]"}" value="{g->text text="Continue"}"/>
</div>
